#include "stdafx.h"
#include "debug.output.listener.h"
#include "debug.console.h"
#include <string.utils.error.code.h>
#include <aclapi.h>
#include <sddl.h>
#include <string>

namespace DH
{
    using HandlePtr = std::shared_ptr<void>;

    static constexpr DWORD dwCurrentPID{static_cast<DWORD>(-1)};

#if 0
    static HandlePtr CreateSecurityDesc(DebugConsole const& rMaster, SECURITY_ATTRIBUTES& secAttrs)
    {
        HRESULT            hCode{S_OK};
        ATL::CStringW      sFunc{L"NONE"};
        PSECURITY_DESCRIPTOR pSD{nullptr};
        auto const         bConv{ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:(A;;GRGWGX;;;WD)(A;;GA;;;SY)(A;;GA;;;BA)(A;;GRGWGX;;;AN)(A;;GRGWGX;;;RC)(A;;GRGWGX;;;S-1-15-2-1)S:(ML;;NW;;;LW)",
            SECURITY_DESCRIPTOR_REVISION1,
            &pSD, nullptr)};
        if (!bConv) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"ConvertStringSecurityDescriptorToSecurityDescriptorW");
            goto reportError;
        }
        if (!pSD) {
            pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
            if (!pSD) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc.Format(L"LocalAlloc");
                goto reportError;
            }
            if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION1)) {
                hCode = static_cast<HRESULT>(GetLastError());
                sFunc.Format(L"InitializeSecurityDescriptor");
                LocalFree((HLOCAL)pSD);
                goto reportError;
            }
        }
        secAttrs.nLength = sizeof(secAttrs);
        secAttrs.lpSecurityDescriptor = pSD;
        secAttrs.bInheritHandle = TRUE;
        rMaster.FormatWide(dwCurrentPID, L"%s OK\n", __FUNCTIONW__);
        return HandlePtr{pSD, LocalFree};

    reportError:
        auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
        rMaster.FormatWide(dwCurrentPID, L"ERROR: %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
        return {};
    }

    static HandlePtr CreateDbgMutex(DebugConsole const& rMaster, PCWSTR pszPrefix, PSECURITY_ATTRIBUTES pSecurity)
    {
        ATL::CStringW sTemp{};
        sTemp.Format(L"%sDBWinMutex", pszPrefix);
        HANDLE hMutex{OpenMutexW(GENERIC_ALL, FALSE, sTemp.GetString())};
        if (!hMutex && ERROR_ACCESS_DENIED == GetLastError()) {
            hMutex = OpenMutexW(SYNCHRONIZE, FALSE, sTemp.GetString());
        }
        if (hMutex) {
            using SetSecurityInfoFn = void (__fastcall *)(HANDLE, SE_OBJECT_TYPE, SECURITY_INFORMATION);
            auto const  hAdvapi{GetModuleHandleW(L"ADVAPI32")};
            auto const pFarProc{GetProcAddress(hAdvapi, "SetSecurityInfo")};
            reinterpret_cast<SetSecurityInfoFn>(pFarProc)(hMutex, SE_KERNEL_OBJECT, LUA_TOKEN);
        }
        else {
            hMutex = CreateMutexW(pSecurity, FALSE, sTemp.GetString());
        }
        if (!hMutex) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            rMaster.FormatWide(dwCurrentPID, L"ERROR: CreateDbgMutex['%s'] failed: 0x%08x %s\n", sTemp.GetString(),
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        rMaster.FormatWide(dwCurrentPID, L"%s['%s'] OK\n", __FUNCTIONW__, sTemp.GetString());
        return HandlePtr{hMutex, CloseHandle};
    }
#endif

    static HandlePtr CreateDbgMapping(DebugConsole const& rMaster, PCWSTR pszPrefix, DWORD dwSize, PSECURITY_ATTRIBUTES pSecurity)
    {
        ATL::CStringW sTemp{};
        sTemp.Format(L"%sDBWIN_BUFFER", pszPrefix);
        HANDLE mappingRaw{CreateFileMappingW(pSecurity, nullptr, PAGE_READWRITE, 0, dwSize, sTemp.GetString())};
        auto hCode{static_cast<HRESULT>(GetLastError())};
        if (ERROR_ALREADY_EXISTS == hCode) {
            CloseHandle(mappingRaw);
            mappingRaw = nullptr;
            SetLastError(static_cast<DWORD>(hCode));
        }
        if (!mappingRaw) {
            hCode = static_cast<HRESULT>(GetLastError());
            rMaster.FormatWide(dwCurrentPID, L"ERROR: CreateFileMapping['%s'] failed: 0x%08x %s\n", sTemp.GetString(),
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        rMaster.FormatWide(dwCurrentPID, L"%s['%s'] OK\n", __FUNCTIONW__, sTemp.GetString());
        return HandlePtr{mappingRaw, CloseHandle};
    }

    static HandlePtr MapDbgShared(DebugConsole const& rMaster, HANDLE hMapped, DWORD dwSize)
    {
        HANDLE const hMemory{MapViewOfFile(hMapped, PAGE_READONLY, 0, 0, dwSize)};
        if (!hMemory) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            rMaster.FormatWide(dwCurrentPID, L"ERROR: MapViewOfFile failed: 0x%08x %s\n",
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        rMaster.FormatWide(dwCurrentPID, L"%s OK\n", __FUNCTIONW__);
        return HandlePtr{hMemory, UnmapViewOfFile};
    }

    static HandlePtr CreateDbgSignal(DebugConsole const&    rMaster,
                                     bool              bManualReset,
                                     bool                  bInitial,
                                     PCWSTR               pszPrefix,
                                     PCWSTR                 pszName,
                                     PSECURITY_ATTRIBUTES pSecurity)
    {
        ATL::CStringW sTemp{};
        sTemp.Format(L"%s%s", pszPrefix, pszName);
        HANDLE const hEvent{CreateEventW(pSecurity, static_cast<BOOL>(bManualReset), static_cast<BOOL>(bInitial), sTemp.GetString())};
        if (!hEvent) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            rMaster.FormatWide(dwCurrentPID, L"ERROR: CreateEvent failed: 0x%08x %s\n",
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        rMaster.FormatWide(dwCurrentPID, L"%s['%s'][%d, %d] OK\n", __FUNCTIONW__, sTemp.GetString(), bManualReset, bInitial);
        return HandlePtr{hEvent, CloseHandle};
    }

    static void AdjustObjectDACL(DebugConsole const& rMaster, HANDLE hObject)
    {
        HRESULT                   hCode{S_OK};
        ATL::CStringW             sFunc{L"NONE"};
        PACL                   pOldDACL{nullptr};
        PACL                   pNewDACL{nullptr};
        PSECURITY_DESCRIPTOR        pSD{nullptr};
        PSID                       pSid{nullptr};
        SID_IDENTIFIER_AUTHORITY authNt{SECURITY_NT_AUTHORITY};
        EXPLICIT_ACCESS              ea{0};
        HandlePtr                pSDPtr{};
        HandlePtr               pSidPtr{};
        HandlePtr           pNewDACLPtr{};
        
        hCode = static_cast<HRESULT>(GetSecurityInfo(hObject, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                                                     nullptr, nullptr, &pOldDACL, nullptr, &pSD));
        if (ERROR_SUCCESS != hCode) {
            sFunc.Format(L"GetSecurityInfo");
            goto reportError;
        }
        HandlePtr{pSD, LocalFree}.swap(pSDPtr);
        if (!AllocateAndInitializeSid(&authNt, 1, SECURITY_AUTHENTICATED_USER_RID, 0, 0, 0, 0, 0, 0, 0, &pSid)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"AllocateAndInitializeSid");
            goto reportError;
        }
        HandlePtr{pSid, FreeSid}.swap(pSidPtr);
        ea.grfAccessMode = GRANT_ACCESS;
        ea.grfAccessPermissions = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;
        ea.grfInheritance = NO_INHERITANCE;
        ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
        ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
        ea.Trustee.ptstrName = static_cast<LPTSTR>(pSid);
        hCode = static_cast<HRESULT>(SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL));
        if (ERROR_SUCCESS != hCode) {
            sFunc.Format(L"SetEntriesInAcl");
            goto reportError;
        }
        HandlePtr{pNewDACL, LocalFree}.swap(pNewDACLPtr);
        hCode = static_cast<HRESULT>(SetSecurityInfo(hObject, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, pNewDACL, nullptr));
        if (ERROR_SUCCESS != hCode) {
            sFunc.Format(L"SetSecurityInfo");
            goto reportError;
        }

        rMaster.FormatWide(dwCurrentPID, L"%s OK\n", __FUNCTIONW__);
        return ;
    reportError:
        auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
        rMaster.FormatWide(dwCurrentPID, L"ERROR: %s[%s] failed: 0x%08x %s\n", __FUNCTIONW__,
            sFunc.GetString(), hCode, sMessage.GetString());
    }

    static void DeleteObjectDACL(DebugConsole const& rMaster, HANDLE hObject)
    {
        DWORD const dwCode{SetSecurityInfo(hObject, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, nullptr, nullptr)};
        if (ERROR_SUCCESS != dwCode) {
            rMaster.FormatWide(dwCurrentPID, L"ERROR: %s[SetSecurityInfo[%p]] failed: 0x%08x %s\n", __FUNCTIONW__, hObject,
                dwCode, Str::ErrorCode<>::SystemMessage(dwCode).GetString());
        }
        else {
            rMaster.FormatWide(dwCurrentPID, L"%s[%p] OK\n", __FUNCTIONW__, hObject);
        }
    }

    DebugOutputListener::~DebugOutputListener()
    {
        Stop();
    }

    DebugOutputListener::DebugOutputListener(DebugConsole const& rMaster)
        :          master_{rMaster}
        ,        thrdStop_{CreateEvent(nullptr, FALSE, FALSE, nullptr), CloseHandle}
        ,      mappingPtr_{}
        ,       buffReady_{}
        ,       dataReady_{}
        ,        shmemPtr_{}
        ,    thrdListener_{}
    {
    }

    bool DebugOutputListener::Init(bool bGlobal, PCWSTR pszWindowName)
    {
        DWORD constexpr   dwMapBytes{4096};
        PCWSTR const       pszPrefix{bGlobal ? L"Global\\" : (pszWindowName ? pszWindowName : L"")};
        HandlePtr mappingPtr{CreateDbgMapping(master_, pszPrefix, dwMapBytes, nullptr)};
        if (!mappingPtr) {
            return false;
        }
        HandlePtr buffRdyPtr{CreateDbgSignal(master_, false, true, pszPrefix, L"DBWIN_BUFFER_READY", nullptr)};
        if (!buffRdyPtr) {
            return false;
        }
        HandlePtr dataRdyPtr{CreateDbgSignal(master_, false, false, pszPrefix, L"DBWIN_DATA_READY", nullptr)};
        if (!dataRdyPtr) {
            return false;
        }
        HandlePtr shmemPtr{MapDbgShared(master_, mappingPtr.get(), dwMapBytes)};
        if (!shmemPtr) {
            return false;
        }
        shmemPtr.swap(shmemPtr_);
        dataRdyPtr.swap(dataReady_);
        buffRdyPtr.swap(buffReady_);
        mappingPtr.swap(mappingPtr_);
        if constexpr (false) {
            DeleteObjectDACL(master_, shmemPtr_.get());
            DeleteObjectDACL(master_, buffReady_.get());
            DeleteObjectDACL(master_, dataReady_.get());
        }
        else {
            AdjustObjectDACL(master_, shmemPtr_.get());
            AdjustObjectDACL(master_, buffReady_.get());
            AdjustObjectDACL(master_, dataReady_.get());
        }
        SetEvent(buffReady_.get());
        return true;
    }

    bool DebugOutputListener::Start(PCWSTR pszWindowName, bool bGlobal)
    {
        if (IsDebuggerPresent()) {
            master_.PutsWide(L"WARNING: IsDebuggerPresent == TRUE\n");
        }
        if (thrdListener_.joinable()) {
            return true;
        }
        if (!Init(bGlobal, pszWindowName)) {
            return false;
        }
        std::thread{&DebugOutputListener::Listener, this}.swap(thrdListener_);
        OutputDebugStringW(__FUNCTIONW__ L" OK\n");
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
        struct DataBuffer
        {
            DWORD    dwPid;
            char szText[1];
        };
        HANDLE const events[]{ thrdStop_.get(), dataReady_.get() };
        while (true) {
            SetEvent(buffReady_.get());
            switch (DWORD const dwSignal{WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE)}) {
            case WAIT_OBJECT_0:
                return ;
            case WAIT_OBJECT_0+1: {
                auto const* pData = static_cast<DataBuffer const*>(shmemPtr_.get());
                master_.PutsNarrow(pData->szText, pData->dwPid);
                ResetEvent(dataReady_.get());
                ResetEvent(buffReady_.get());
                break;
            }
            }
        }
    }
}

