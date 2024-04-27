#ifndef _DH_dev_assist_h__
#define _DH_dev_assist_h__

#ifdef _MSC_VER
#pragma once
#endif

#include "wcdafx.api.h"
#include <string.hp.h>
#include <tchar.h>

namespace Dh
{
    enum WindowTypes
    {
        UnknownCtl              = 0,
        StaticClt               = 0x00000001,
        EditClt                 = 0x00000002,
        ButtonClt               = 0x00000004,
        ListBoxCtl              = 0x00000008,
        ComboBoxCtl             = 0x00000010,
        ToolBarCtl              = 0x00000020,
        ReBarCtl                = 0x00000040,
        StatusBarClt            = 0x00000080,
        CommonCtl               = 0x80000000,
    };

    WCDAFX_API int GetWindowType(HWND winHandle);

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
    WCDAFX_API CharString LvStyleStringA(DWORD);
    WCDAFX_API CharString LvStyleExStringA(DWORD);
    WCDAFX_API WidecharString LvStyleStringW(DWORD);
    WCDAFX_API WidecharString LvStyleExStringW(DWORD);

    WCDAFX_API WidecharString MessageToStrignW(MSG const* message);
}

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef UNICODE
#define VK_C2ST             VK_C2SW
#define LvStyleStringT      LvStyleStringW
#define LvStyleExStringT    LvStyleExStringW
#else
#define VK_C2ST             VK_C2SA
#define LvStyleStringA      LvStyleStringW
#define LvStyleExStringA    LvStyleExStringW
#endif

#endif // _DH_dev_assist_h__
