#pragma once

#include <wcdafx.api.h>
#include <strint.h>
#include <cstdint>

namespace DH
{
    enum WindowTypes: uint64_t
    {
        WT_Unknown = static_cast<uint64_t>(-1),
        WT_Static = 0,
        WT_Edit,
        WT_Button,
        WT_ListBox,
        WT_ComboBox,
        WT_ToolBar,
        WT_ReBar,
        WT_StatusBar,
    };

    WCDAFX_API uint64_t GetWindowType(HWND hWnd);

    // Window message constant to string...
    WCDAFX_API PCSTR WMNumToStrA(UINT);
    WCDAFX_API PCWSTR WMNumToStrW(UINT);

    // Virtual key constant to string...
    WCDAFX_API PCSTR VirtKeyToStrA(UINT);
    WCDAFX_API PCWSTR VirtKeyToStrW(UINT);

    // Window notify code constant to string...
    WCDAFX_API PCSTR NotifyCodeToStrA(UINT, HWND);
    WCDAFX_API PCSTR NotifyCodeToStrA(UINT);
    WCDAFX_API PCWSTR NotifyCodeToStrW(UINT, HWND);
    WCDAFX_API PCWSTR NotifyCodeToStrW(UINT);

    // List view styles constants to string...
    WCDAFX_API LString LvStyleStrA(DWORD);
    WCDAFX_API LString LvStyleExStrA(DWORD);
    WCDAFX_API WString LvStyleStrW(DWORD);
    WCDAFX_API WString LvStyleExStrW(DWORD);

    // Window Position Flags SWP_***
    WCDAFX_API LString WinPosFlagsStrA(UINT);
    WCDAFX_API WString WinPosFlagsStrW(UINT);

    // 'Dumpers'
    WCDAFX_API WString MessageToStrignW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    WCDAFX_API WString MessageToStrignW(const PMSG pMsg);
}

#ifndef __PRETTY_FUNCTION__
#  define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef UNICODE
#  define VK_C2ST             VirtKeyToStrW
#  define LvStyleStringT      LvStyleStrW
#  define LvStyleExStringT    LvStyleExStrW
#else
#  define VK_C2ST             VirtKeyToStrA
#  define LvStyleStrA      LvStyleStrW
#  define LvStyleExStrA    LvStyleExStrW
#endif
