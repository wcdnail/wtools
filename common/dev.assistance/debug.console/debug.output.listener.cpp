#include "stdafx.h"
#include "debug.output.listener.h"
#include "debug.console.h"
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <aclapi.h>
#include <sddl.h>
#include <string>

namespace DH
{
    class DebugOutputListener::StaticInit
    {
        ~StaticInit();
        StaticInit();

    public:
        static StaticInit& Instance();

        static bool SetPrivilege(HANDLE hToken, PCWSTR pszPrivilege, bool bEnable);
        static bool AdjustProcPrivileges();

        static HandlePtr CreateSecurityDesc(SECURITY_ATTRIBUTES& secAttrs);
        static HandlePtr CreateDbgMutex(PCWSTR pszPrefix, PSECURITY_ATTRIBUTES pSecurity);
        static HandlePtr CreateMapping(PCWSTR pszPrefix, DWORD dwSize, PSECURITY_ATTRIBUTES pSecurity);
        static HandlePtr MapSharedMemory(HANDLE hMapped, DWORD dwSize);
        static HandlePtr CreateSignal(PCWSTR pszPrefix, PCWSTR pszName, PSECURITY_ATTRIBUTES pSecurity);
        static void AdjustObjectDACL(HANDLE hObject);
        static void DeleteObjectDACL(HANDLE hObject);
    };

    DebugOutputListener::StaticInit::~StaticInit() = default;

    DebugOutputListener::StaticInit::StaticInit()
    {
        AdjustProcPrivileges();
    }

    DebugOutputListener::StaticInit& DebugOutputListener::StaticInit::Instance()
    {
        static StaticInit staticInit;
        return staticInit;
    }

    bool DebugOutputListener::StaticInit::SetPrivilege(HANDLE hToken, PCWSTR pszPrivilege, bool bEnable)
    {
        TOKEN_PRIVILEGES tp{0};
        LUID           luid{0};
        if (!LookupPrivilegeValueW(nullptr, pszPrivilege, &luid)) {
            return false;
        }
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr)) {
            return false;
        }
        return true;
    }

    bool DebugOutputListener::StaticInit::AdjustProcPrivileges()
    {
        HRESULT       hCode{S_OK};
        ATL::CStringW sFunc{L"NONE"};
        HANDLE       handle{nullptr};
        HandlePtr    pToken{};
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"OpenProcessToken");
            goto reportError;
        }
        HandlePtr{handle, CloseHandle}.swap(pToken);

        if (!SetPrivilege(pToken.get(), SE_DEBUG_NAME, true)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"SetPrivilege('%s')", SE_DEBUG_NAME);
            goto reportError;
        }
        if (!SetPrivilege(pToken.get(), SE_CREATE_GLOBAL_NAME, true)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"SetPrivilege('%s')", SE_CREATE_GLOBAL_NAME);
            goto reportError;
        }
        return true;
    reportError:
        auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
        DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
        return false;
    }

    DebugOutputListener::HandlePtr DebugOutputListener::StaticInit::CreateSecurityDesc(SECURITY_ATTRIBUTES& secAttrs)
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
        return HandlePtr{pSD, LocalFree};

    reportError:
        auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
        DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
        return {};
    }

    DebugOutputListener::HandlePtr DebugOutputListener::StaticInit::CreateDbgMutex(PCWSTR pszPrefix, PSECURITY_ATTRIBUTES pSecurity)
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
            DH::TPrintf(LTH_DBG_ASSIST L" CreateDbgMutex('%s') failed: 0x%08x %s\n", sTemp.GetString(),
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        return HandlePtr{hMutex, CloseHandle};
    }

    DebugOutputListener::HandlePtr DebugOutputListener::StaticInit::CreateMapping(PCWSTR pszPrefix, DWORD dwSize, PSECURITY_ATTRIBUTES pSecurity)
    {
        ATL::CStringW sTemp{};
        sTemp.Format(L"%sDBWIN_BUFFER", pszPrefix);
        HANDLE const mappingRaw{CreateFileMappingW(pSecurity, nullptr, PAGE_READWRITE, 0, dwSize, sTemp.GetString())};
        if (!mappingRaw) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            DH::TPrintf(LTH_DBG_ASSIST L" CreateFileMapping('%s') failed: 0x%08x %s\n", sTemp.GetString(),
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        return HandlePtr{mappingRaw, CloseHandle};
    }

    DebugOutputListener::HandlePtr DebugOutputListener::StaticInit::MapSharedMemory(HANDLE hMapped, DWORD dwSize)
    {
        HANDLE const hMemory{MapViewOfFile(hMapped, PAGE_READONLY, 0, 0, dwSize)};
        if (!hMemory) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            DH::TPrintf(LTH_DBG_ASSIST L" MapViewOfFile failed: 0x%08x %s\n",
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        return HandlePtr{hMemory, UnmapViewOfFile};
    }

    DebugOutputListener::HandlePtr DebugOutputListener::StaticInit::CreateSignal(PCWSTR pszPrefix, PCWSTR pszName, PSECURITY_ATTRIBUTES pSecurity)
    {
        ATL::CStringW sTemp{};
        sTemp.Format(L"%s%s", pszPrefix, pszName);
        HANDLE const hEvent{CreateEventW(pSecurity, FALSE, FALSE, sTemp.GetString())};
        if (INVALID_HANDLE_VALUE == hEvent) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            DH::TPrintf(LTH_DBG_ASSIST L" CreateEvent failed: 0x%08x %s\n",
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return {};
        }
        return HandlePtr{hEvent, CloseHandle};
    }

    void DebugOutputListener::StaticInit::AdjustObjectDACL(HANDLE hObject)
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

        return ;
    reportError:
        auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
        DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
    }

    void DebugOutputListener::StaticInit::DeleteObjectDACL(HANDLE hObject)
    {
        DWORD const dwCode{SetSecurityInfo(hObject, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, nullptr, nullptr)};
        if (ERROR_SUCCESS != dwCode) {
            DH::TPrintf(LTH_DBG_ASSIST L" SetSecurityInfo failed: 0x%08x %s\n",
                dwCode, Str::ErrorCode<>::SystemMessage(dwCode).GetString());
        }
    }

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
        UNREFERENCED_PARAMETER(StaticInit::Instance());
    }

    bool DebugOutputListener::Init(bool bGlobal, PCWSTR pszWindowName)
    {
        DWORD constexpr   dwMapBytes{4096};
        PCWSTR const       pszPrefix{bGlobal ? L"Global\\" : (pszWindowName ? pszWindowName : L"")};
        SECURITY_ATTRIBUTES secAttrs{0};
        HandlePtr         secDescPtr{StaticInit::CreateSecurityDesc(secAttrs)};
      //HandlePtr           mutexPtr{StaticInit::CreateDbgMutex(pszPrefix, &secAttrs)};
        HandlePtr         mappingPtr{StaticInit::CreateMapping(pszPrefix, dwMapBytes, &secAttrs)};
        HandlePtr         dataRdyPtr{StaticInit::CreateSignal(pszPrefix, L"DBWIN_BUFFER_READY", &secAttrs)};
        HandlePtr         buffRdyPtr{StaticInit::CreateSignal(pszPrefix, L"DBWIN_DATA_READY", &secAttrs)};
        ShmemPtr            shmemPtr{StaticInit::MapSharedMemory(mappingPtr.get(), dwMapBytes)};
        if (!mappingPtr || !shmemPtr || !dataRdyPtr || !buffRdyPtr) {
            return false;
        }
        shmemPtr.swap(shmemPtr_);
        buffRdyPtr.swap(buffReady_);
        dataRdyPtr.swap(dataReady_);
        mappingPtr.swap(mappingPtr_);
      //mutexPtr.swap(mutexPtr_);
        if constexpr (true) {
            StaticInit::AdjustObjectDACL(shmemPtr_.get());
            StaticInit::AdjustObjectDACL(buffReady_.get());
            StaticInit::AdjustObjectDACL(dataReady_.get());
        }
        else {
            StaticInit::DeleteObjectDACL(shmemPtr_.get());
            StaticInit::DeleteObjectDACL(buffReady_.get());
            StaticInit::DeleteObjectDACL(dataReady_.get());
        }
        SetEvent(buffReady_.get());
        return true;
    }

    bool DebugOutputListener::Start(PCWSTR pszWindowName, bool bGlobal)
    {
        if (IsDebuggerPresent()) {
            owner_.PutsWide(L"WARNING: IsDebuggerPresent == TRUE\n");
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
        HANDLE   events[]{ thrdStop_.get(), dataReady_.get() };
      //DWORD const myPid{GetCurrentProcessId()};
        while (true) {
            SetEvent(buffReady_.get());
            switch (DWORD const dwSignal{WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE)}) {
            case WAIT_OBJECT_0:
                return ;
            case WAIT_OBJECT_0+1: {
                auto const* pData = static_cast<DataBuffer const*>(shmemPtr_.get());
                owner_.PutsNarrow(pData->szText, pData->dwPid);
                ResetEvent(dataReady_.get());
                ResetEvent(buffReady_.get());
                break;
            }
            }
        }
    }
}

