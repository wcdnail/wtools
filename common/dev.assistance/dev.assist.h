#pragma once

#include "wcdafx.api.h"
#include "strint.h"
#include <cstdint>

namespace DH
{
    enum WindowTypes: uint64_t
    {
        UnknownCtl   = 0x0000000000000000,
        StaticClt    = 0x0000000000000001,
        EditClt      = 0x0000000000000002,
        ButtonClt    = 0x0000000000000004,
        ListBoxCtl   = 0x0000000000000008,
        ComboBoxCtl  = 0x0000000000000010,
        ToolBarCtl   = 0x0000000000000020,
        ReBarCtl     = 0x0000000000000040,
        StatusBarClt = 0x0000000000000080,
        CommonCtl    = 0x8000000000000000,
    };

    WCDAFX_API uint64_t GetWindowType(HWND winHandle);

    // Window message constant to string...
    WCDAFX_API PCSTR WM_C2SA(UINT);
    WCDAFX_API PCWSTR WM_C2SW(UINT);

    // Virtual key constant to string...
    WCDAFX_API PCSTR VK_C2SA(UINT);
    WCDAFX_API PCWSTR VK_C2SW(UINT);

    // Window notify code constant to string...
    WCDAFX_API PCSTR WM_NC_C2SA(UINT, HWND);
    WCDAFX_API PCWSTR WM_NC_C2SW(UINT, HWND);

    // List view styles constants to string...
    WCDAFX_API LString LvStyleStringA(DWORD);
    WCDAFX_API LString LvStyleExStringA(DWORD);
    WCDAFX_API WString LvStyleStringW(DWORD);
    WCDAFX_API WString LvStyleExStringW(DWORD);

    WCDAFX_API WString MessageToStrignW(MSG const* message);
}

#ifndef __PRETTY_FUNCTION__
#  define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef UNICODE
#  define VK_C2ST             VK_C2SW
#  define LvStyleStringT      LvStyleStringW
#  define LvStyleExStringT    LvStyleExStringW
#else
#  define VK_C2ST             VK_C2SA
#  define LvStyleStringA      LvStyleStringW
#  define LvStyleExStringA    LvStyleExStringW
#endif
