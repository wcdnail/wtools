#include "stdafx.h"
#include "win-5-6-7.features.h"
#include "string.utils.error.code.h"
#include "brute_cast.h"
#include "dh.tracing.h"

Win567Impl const& Win567()
{
    static Win567Impl inst;
    return inst;
}

template<class ResultType> 
ResultType Win567Impl::LoadProcAddress(PCWSTR dllName, PCSTR procName)
{
    HMODULE dll = ::GetModuleHandleW(dllName);
    if (nullptr == dll) {
        dll = ::LoadLibraryW(dllName);
        // ##TODO: Check for leaks
        if (nullptr == dll) {
            HRESULT hr = static_cast<HRESULT>(::GetLastError());
            auto err = Str::ErrorCode<>::SystemMessage(hr);
            Dh::ThreadPrintf(L"567Feats: Can't load `%s` - %d `%s`\n", dllName, hr, err.GetString());
            return nullptr;
        }
    }

    FARPROC rv = ::GetProcAddress(dll, procName);
    if (nullptr == rv) {
        HRESULT hr = static_cast<HRESULT>(::GetLastError());
        auto err = Str::ErrorCode<>::SystemMessage(hr);
        Dh::ThreadPrintf(L"567Feats: `%S`@`%s` - %d `%s`\n", procName, dllName, hr, err.GetString());
        return nullptr;
    }
    Dh::ThreadPrintf(L"567Feats: `%S`@`%s` OK\n", procName, dllName);
    return cast_ptr_to_ptr<ResultType>(rv);
}

Win567Impl::Win567Impl() 
    : version_(Runtime::Info().System.Version)
    , isWow64Process_(LoadProcAddress<IW64P>(_T("KERNEL32"), "IsWow64Process"))
    , getQueuedCompletionStatusEx_(OnVistaOrHigher() ? LoadProcAddress<GQCSE>(_T("KERNEL32"), "GetQueuedCompletionStatusEx") : nullptr)
    , dwmIsCompositionEnabled_(OnVistaOrHigher() ? LoadProcAddress<ICE>(_T("DWMAPI"), "DwmIsCompositionEnabled") : nullptr)
    , dwmExtendFrameIntoClientArea_(OnVistaOrHigher() ? LoadProcAddress<EFICA>(_T("DWMAPI"), "DwmExtendFrameIntoClientArea") : nullptr)
{}

Win567Impl::~Win567Impl() 
{}

bool Win567Impl::OnVista() const
{
    return (6 == version_.Major) && (0 == version_.Minor);
}

bool Win567Impl::OnVistaOrHigher() const
{
    return version_.Major > 5;
}

bool Win567Impl::On7() const
{
    return (6 == version_.Major) && (1 == version_.Minor);
}

template <class Prototype>
static bool CheckProc(Prototype const& prototype)
{
    if (nullptr == prototype)
    {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return false;
    }

    return true;
}

BOOL Win567Impl::IsWow64Process(HANDLE process, PBOOL wow64Process) const
{
    return CheckProc(isWow64Process_) ?
           isWow64Process_(process, wow64Process) :
           FALSE;
}

bool Win567Impl::UnderWow64(HANDLE process /*= ::GetCurrentProcess()*/) const
{
    BOOL underWow64 = FALSE;
    if (!this->IsWow64Process(process, &underWow64)) {
        return false;
    }
    return FALSE != underWow64;
}

BOOL Win567Impl::GetQueuedCompletionStatusEx(HANDLE port, LPOVERLAPPED_ENTRY ovEntry, ULONG count, PULONG numRemoved, DWORD milliseconds, BOOL alertable) const
{
    return CheckProc(getQueuedCompletionStatusEx_) ? 
           getQueuedCompletionStatusEx_(port, ovEntry, count, numRemoved, milliseconds, alertable) : 
           FALSE;
}

bool Win567Impl::IsCompositionEnabled() const
{
    BOOL enabled = FALSE;
    HRESULT hr = IsCompositionEnabled(&enabled);
    return SUCCEEDED(hr) && enabled;
}

HRESULT Win567Impl::IsCompositionEnabled(BOOL* enabled) const
{
    return CheckProc(dwmIsCompositionEnabled_) ? 
           dwmIsCompositionEnabled_(enabled) : 
           E_NOINTERFACE;
}

HRESULT Win567Impl::ExtendFrameIntoClientArea(HWND hwnd, MARGINS const* margns) const
{
    return CheckProc(dwmExtendFrameIntoClientArea_) ? 
           dwmExtendFrameIntoClientArea_(hwnd, margns) : 
           CO_E_NOT_SUPPORTED;
}
