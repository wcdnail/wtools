#include "stdafx.h"
#include "dev.assist.h"

namespace Dh
{
    int GetWindowType(HWND winHandle)
    {
        TCHAR className[32];
        int classNameLen = GetClassName(winHandle, className, _countof(className));
        if (classNameLen > 0)
        {
            struct winTypeName
            {
                PCTSTR name;
                int n;
            };

            static const winTypeName names[] = 
            {
              { _T("Static"), StaticClt }
            , { _T("Edit"), EditClt }
            , { _T("Button"), ButtonClt }
            , { _T("Listbox"), ListBoxCtl }
            , { _T("Combobox"), ComboBoxCtl }
            , { TOOLBARCLASSNAME, ToolBarCtl | CommonCtl }
            , { REBARCLASSNAME, ReBarCtl | CommonCtl }
            , { STATUSCLASSNAME, StatusBarClt | CommonCtl }
            };

            for (size_t i=0; i<_countof(names); i++)
            {
                if (0 == _tcsncicmp(className, names[i].name, classNameLen))
                    return names[i].n;
            }

          //ATLTRACE(_T("Unknown class `%s` of %x\n"), className, winHandle);
        }
        return UnknownCtl;
    }

//////////////////////////////////////////////////////////////////////////
// Multibyte group

#define _ON_CONST(X) case X: return #X

    PCSTR WM_C2SA(UINT x)
    {
        switch (x) 
        {
#include "window.messages.consts.h"
        }
        return "";
    }

    PCSTR VK_C2SA(UINT x)
    {
        switch (x) 
        {
#include "virtual.keys.consts.h"
        }

        return "";
    }

#define _BEG_CONST_GROUP(X)     \
        if (0 != (winType & X)) \
        {                       \
            switch (code)       \
            {                   \

#define _END_CONST_GROUP()      \
            };                  \
        }

    PCSTR WM_NC_C2SA(UINT code, HWND win)
    {
        int winType = GetWindowType(win);
#include "notify.codes.h"
        return "";
    }

#undef _ON_CONST

#define _ON_BITMASK(mask) \
    if (0 != (mask & style)) \
    { \
        if (!result.empty()) \
            result += " "; \
        result += #mask; \
    }

    CharString WCDAFX_API LvStyleStringA(DWORD style)
    {
        CharString result;
#include "list.view.styles.h"
        return result;
    }

    CharString WCDAFX_API LvStyleExStringA(DWORD style)
    {
        CharString result;
#include "list.view.exstyles.h"
        return result;
    }

#undef _ON_BITMASK

//////////////////////////////////////////////////////////////////////////
// Wide group

#define _ON_CONST(X) case X: return L#X

    PCWSTR WM_C2SW(UINT x)
    {
        switch (x) 
        {
#include "window.messages.consts.h"
        }
                
        return L"";
    }

    PCWSTR VK_C2SW(UINT x)
    {
        switch (x) 
        {
#include "virtual.keys.consts.h"
        }

        return L"";
    }

    PCWSTR WM_NC_C2SW(UINT code, HWND win)
    {
        int winType = GetWindowType(win);
#include "notify.codes.h"
        return L"";
    }

#undef _ON_CONST


#define _ON_BITMASK(mask) \
    if (0 != (mask & style)) \
    { \
        if (!result.empty()) \
            result += L" "; \
        result += L#mask; \
    }

    WidecharString WCDAFX_API LvStyleStringW(DWORD style)
    {
        WidecharString result;
#include "list.view.styles.h"
        return result;
    }

    WidecharString WCDAFX_API LvStyleExStringW(DWORD style)
    {
        WidecharString result;
#include "list.view.exstyles.h"
        return result;
    }

#undef _ON_BITMASK

    WidecharString MessageToStrignW(MSG const* message)
    {
        wchar_t buffer[512] = {0};
        _snwprintf_s(buffer, _countof(buffer),
            L"%08x %08x %08x%08x `%s`",
            message->message,
            static_cast<uint32_t>(message->wParam),
            static_cast<uint32_t>(message->lParam >> 32),
            static_cast<uint32_t>(message->lParam & 0xffffffff),
            WM_C2SW(message->message)
        );
        return buffer;
    }
}
