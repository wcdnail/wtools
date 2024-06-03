#include "stdafx.h"
#include "debug.output.listener.h"
#include "debug.console.h"
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <aclapi.h>
#include <string>

namespace DH
{
    DebugOutputListener::~DebugOutputListener()
    {
        Stop();
    }

    DebugOutputListener::DebugOutputListener(DebugConsole const& owner)
        :           owner_{owner}
        ,        thrdStop_{CreateEvent(nullptr, FALSE, FALSE, nullptr), CloseHandle}
        ,        mutexPtr_{}
        ,       buffReady_{}
        ,       dataReady_{}
        ,      mappingPtr_{}
        ,        shmemPtr_{}
        , securityDescPtr_{}
        ,    thrdListener_{}
    {
    }

    namespace
    {
        struct IntSecurityDesc
        {
            PSID          pEveryoneSID{nullptr};
            PSID             pAdminSID{nullptr};
            PACL                  pACL{nullptr};
            PSECURITY_DESCRIPTOR pDesc{nullptr};

            void Free()
            {
                if (pEveryoneSID) {
                    FreeSid(pEveryoneSID);
                    pEveryoneSID = nullptr;
                }
                if (pAdminSID) {
                    FreeSid(pAdminSID);
                    pAdminSID = nullptr;
                }
                if (pACL) {
                    LocalFree(pACL);
                    pACL = nullptr;
                }
                if (pDesc) {
                    LocalFree(pDesc);
                    pDesc = nullptr;
                }
            }
        };

        void IntFreeSecDesc(IntSecurityDesc* pDesq)
        {
            if (pDesq) {
                pDesq->Free();
                delete pDesq;
            }
        }

        bool IntInitSecAttrs(SECURITY_ATTRIBUTES& secAttrs, IntSecurityDesc& secDesq)
        {
            HRESULT                         hCode{S_OK};
            ATL::CStringW                   sFunc{L"NONE"};
            SID_IDENTIFIER_AUTHORITY sidAuthWorld{SECURITY_WORLD_SID_AUTHORITY};
            SID_IDENTIFIER_AUTHORITY    sidAuthNT{SECURITY_NT_AUTHORITY};
            EXPLICIT_ACCESS              exAccess[2]{0};
            
            if (!AllocateAndInitializeSid(&sidAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &secDesq.pEveryoneSID)) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc = L"AllocateAndInitializeSid(SECURITY_WORLD_SID_AUTHORITY)";
                goto reportError;
            }
            if (!AllocateAndInitializeSid(&sidAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &secDesq.pAdminSID)) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc = L"AllocateAndInitializeSid(SECURITY_NT_AUTHORITY)";
                goto reportError;
            }
            // Everyone READ only
            exAccess[0].grfAccessPermissions = KEY_READ;
            exAccess[0].grfAccessMode = SET_ACCESS;
            exAccess[0].grfInheritance = NO_INHERITANCE;
            exAccess[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            exAccess[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            exAccess[0].Trustee.ptstrName  = static_cast<LPTSTR>(secDesq.pEveryoneSID);
            // Admin FULL access
            exAccess[1].grfAccessPermissions = KEY_ALL_ACCESS;
            exAccess[1].grfAccessMode = SET_ACCESS;
            exAccess[1].grfInheritance = NO_INHERITANCE;
            exAccess[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            exAccess[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
            exAccess[1].Trustee.ptstrName = static_cast<LPTSTR>(secDesq.pAdminSID);

            hCode = SetEntriesInAcl(_countof(exAccess), exAccess, nullptr, &secDesq.pACL);
            if (ERROR_SUCCESS != hCode) {
                sFunc = L"SetEntriesInAcl";
                goto reportError;
            }

            secDesq.pDesc = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
            if (!secDesq.pDesc) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc = L"LocalAlloc";
                goto reportError;
            }

            if (!InitializeSecurityDescriptor(secDesq.pDesc, SECURITY_DESCRIPTOR_REVISION)) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc = L"InitializeSecurityDescriptor";
                goto reportError;
            }

            if (!SetSecurityDescriptorDacl(secDesq.pDesc, TRUE, secDesq.pACL, FALSE)) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc = L"SetSecurityDescriptorDacl";
                goto reportError;
            }

            secAttrs.nLength = sizeof(secAttrs);
            secAttrs.lpSecurityDescriptor = secDesq.pDesc;
            secAttrs.bInheritHandle = TRUE;
            return true;
        reportError:
            auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
            DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
            secDesq.Free();
            return false;
        }

        HANDLE IntOpenEvent(DWORD dwAccess, BOOL bInheritHandle, ATL::CComBSTR const& bsName, LPSECURITY_ATTRIBUTES pSecAttrs)
        {
            HRESULT       hCode{S_OK};
            ATL::CStringW sFunc{L"NONE"};
            HANDLE        hTemp{INVALID_HANDLE_VALUE};
            hTemp = OpenEvent(dwAccess, bInheritHandle, bsName);
            if (!hTemp) {
                hCode = static_cast<HRESULT>(GetLastError());
                if (hCode != ERROR_FILE_NOT_FOUND) {
                    sFunc.Format(L"OpenEvent('%s')", bsName.m_str);
                    goto reportError;
                }
                hTemp = CreateEvent(pSecAttrs, FALSE, FALSE, bsName);
                if (!hTemp) {
                    hCode = static_cast<HRESULT>(GetLastError());
                    sFunc.Format(L"CreateEvent('%s')", bsName.m_str);
                    goto reportError;
                }
            }
            return hTemp;
        reportError:
            auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
            DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
            return nullptr;
        }
    }

    bool DebugOutputListener::Init()
    {
        HRESULT                     hCode{S_OK};
        ATL::CStringW               sFunc{L"NONE"};
        ATL::CStringW            sMessage{};
        HANDLE                   mutexRaw{nullptr};
        HANDLE                    buffRdy{nullptr};
        HANDLE                    dataRdy{nullptr};
        HANDLE                 mappingRaw{nullptr};
        PVOID                    shmemRaw{nullptr};
        HandlePtr                mutexPtr{};
        HandlePtr              buffRdyPtr{};
        HandlePtr              dataRdyPtr{};
        HandlePtr              mappingPtr{};
        ShmemPtr                 shmemPtr{};
        ATL::CComBSTR const    bsWinMutex{L"DBWinMutex"};
        ATL::CComBSTR const  bsWinBuffRdy{L"DBWIN_BUFFER_READY"};
        ATL::CComBSTR const  bsWinDataRdy{L"DBWIN_DATA_READY"};
        ATL::CComBSTR const     bsWinFile{L"DBWIN_BUFFER"};
        SECURITY_ATTRIBUTES      secAttrs{0};
        IntSecurityDesc           secDesq{};
        HandlePtr              secDescPtr{};

        if (!IntInitSecAttrs(secAttrs, secDesq)) {
            return false;
        }
        HandlePtr{new IntSecurityDesc{secDesq}, IntFreeSecDesc}.swap(secDescPtr);

        mutexRaw = OpenMutex(MUTEX_MODIFY_STATE, FALSE, bsWinMutex);
        if (!mutexRaw) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"OpenMutex('%s')", bsWinMutex.m_str);
            goto reportError;
        }
        HandlePtr{mutexRaw, CloseHandle}.swap(mutexPtr);

        buffRdy = IntOpenEvent(EVENT_ALL_ACCESS, FALSE, bsWinBuffRdy, &secAttrs);
        if (!buffRdy) {
            return false;
        }
        HandlePtr{buffRdy, CloseHandle}.swap(buffRdyPtr);

        dataRdy = IntOpenEvent(SYNCHRONIZE, FALSE, bsWinDataRdy, &secAttrs);
        if (!dataRdy) {
            return false;
        }
        HandlePtr{dataRdy, CloseHandle}.swap(dataRdyPtr);

        mappingRaw = OpenFileMapping(FILE_MAP_READ, FALSE, bsWinFile);
        if (!mappingRaw) {
            hCode = static_cast<HRESULT>(GetLastError());
            if (hCode != ERROR_FILE_NOT_FOUND) {
                sFunc.Format(L"OpenFileMapping('%s')", bsWinFile.m_str);
                goto reportError;
            }
            mappingRaw = CreateFileMapping(INVALID_HANDLE_VALUE, &secAttrs, PAGE_READWRITE, 0, 4096, bsWinFile);
            if (!mappingRaw) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc.Format(L"CreateFileMapping('%s')", bsWinFile.m_str);
                goto reportError;
            }
        }
        HandlePtr{mappingRaw, CloseHandle}.swap(mappingPtr);

        shmemRaw = MapViewOfFile(mappingRaw, SECTION_MAP_READ, 0, 0, 0);
        if (!shmemRaw) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"MapViewOfFile('%s')", bsWinFile.m_str);
            goto reportError;
        }
        HandlePtr{shmemRaw, UnmapViewOfFile}.swap(shmemPtr);

        shmemPtr.swap(shmemPtr_);
        mappingPtr.swap(mappingPtr_);
        dataRdyPtr.swap(dataReady_);
        buffRdyPtr.swap(buffReady_);
        mutexPtr.swap(mutexPtr_);
        secDescPtr.swap(securityDescPtr_);
        return true;

    reportError:
        sMessage = Str::ErrorCode<>::SystemMessage(hCode);
        DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
        return false;
    }

    bool DebugOutputListener::Start()
    {
        if (thrdListener_.joinable()) {
            return true;
        }
        if (!Init()) {
            return false;
        }
        std::thread{&DebugOutputListener::Listener, this}.swap(thrdListener_);
        return thrdListener_.joinable();
    }

    bool DebugOutputListener::Stop()
    {
        if (!thrdListener_.joinable()) {
            return false;
        }
        ::SetEvent(thrdStop_.get());
        thrdListener_.join();
        return true;
    }

    void DebugOutputListener::Listener() const
    {   
        HANDLE   events[]{ dataReady_.get(), thrdStop_.get() };
        DWORD const myPid{GetCurrentProcessId()};
        while (true) {
            SetEvent(buffReady_.get());
            DWORD const dwSignal{WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE)};
            if (WAIT_OBJECT_0 == dwSignal) {   
                struct DBUF_D 
                {
                    DWORD pid;
                    char string[1];
                };
                auto const* pData = static_cast<DBUF_D const*>(shmemPtr_.get());
                if (myPid == pData->pid) {
                    owner_.Puts(pData->string);
                }
            }
            else if ((WAIT_OBJECT_0+1) == dwSignal) {
                ::ResetEvent(thrdStop_.get());
                break;
            }
        }
        SetEvent(thrdStop_.get());
    }
}
