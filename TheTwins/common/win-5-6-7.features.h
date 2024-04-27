#pragma once

#include "wcdafx.api.h"
#include "info/runtime.information.h"
#include <windef.h>
#include <uxtheme.h>
#include <boost/noncopyable.hpp>

class Win567Impl: boost::noncopyable
{
public:
    // XP SP2 stuff...
    //
    // IsWow64Process <kernel32>
    typedef BOOL (WINAPI *IW64P)(HANDLE, PBOOL);
    //
    // End of XP SP2 stuff...

    // Vista stuff...
    // 
    // IO <kernel32>
    // GetQueuedCompletionStatusEx
    typedef BOOL (WINAPI *GQCSE)(HANDLE, LPOVERLAPPED_ENTRY, ULONG, PULONG, DWORD, BOOL);

    //
    // Composition <dwmapi>
    // DwmIsCompositionEnabled
    typedef HRESULT (WINAPI *ICE)(BOOL*);

    //
    // DwmExtendFrameIntoClientArea
    typedef HRESULT (WINAPI* EFICA)(HWND, const MARGINS*);

    //
    // End of Vista stuff...

    WCDAFX_API bool OnVista() const;
    WCDAFX_API bool OnVistaOrHigher() const;
    WCDAFX_API bool On7() const;

    WCDAFX_API bool UnderWow64(HANDLE process = ::GetCurrentProcess()) const;

    // XP SP2
    WCDAFX_API BOOL IsWow64Process(HANDLE process, PBOOL wow64Process) const;

    // Vista stuff...
    WCDAFX_API BOOL GetQueuedCompletionStatusEx(HANDLE port, LPOVERLAPPED_ENTRY ovEntry, ULONG count, PULONG numRemoved, DWORD milliseconds, BOOL alertable) const;

    WCDAFX_API bool IsCompositionEnabled() const;
    WCDAFX_API HRESULT IsCompositionEnabled(BOOL* enabled) const;
    WCDAFX_API HRESULT ExtendFrameIntoClientArea(HWND hwnd, MARGINS const* margns) const;

private:
    Runtime::InfoStore::SystemInfo::VersionInfo const& version_;
    IW64P isWow64Process_;
    GQCSE getQueuedCompletionStatusEx_;
    ICE dwmIsCompositionEnabled_;
    EFICA dwmExtendFrameIntoClientArea_;

private:
    friend WCDAFX_API Win567Impl const& Win567();

    Win567Impl();
    ~Win567Impl();

    template<class Rv> 
    static Rv LoadProcAddress(PCWSTR dllName, PCSTR procName);
};

extern WCDAFX_API Win567Impl const& Win567();
