#include "stdafx.h"
#include "dev.assist.h"

#define WM_UAHDESTROYWINDOW     0x0090
#define WM_UAHDRAWMENU          0x0091
#define WM_UAHDRAWMENUITEM      0x0092
#define WM_UAHINITMENU          0x0093
#define WM_UAHMEASUREMENUITEM   0x0094
#define WM_UAHNCPAINTMENUPOPUP  0x0095

namespace DH
{
    uint64_t GetWindowType(HWND winHandle)
    {
        TCHAR cls[256] = { 0 };
        size_t    clen = static_cast<size_t>(GetClassName(winHandle, cls, _countof(cls)));
        if (clen > 0) {
            struct WTYPE_NAME
            {
                PCTSTR  name;
                uint64_t   n;
            };
            static const WTYPE_NAME names[] = 
            {
                {     _T("Static"), StaticClt },
                {       _T("Edit"), EditClt },
                {     _T("Button"), ButtonClt },
                {    _T("Listbox"), ListBoxCtl },
                {   _T("Combobox"), ComboBoxCtl },
                { TOOLBARCLASSNAME, ToolBarCtl | CommonCtl },
                {   REBARCLASSNAME, ReBarCtl | CommonCtl },
                {  STATUSCLASSNAME, StatusBarClt | CommonCtl },
            };

            for (const auto& it: names) {
                if (0 == _tcsncicmp(cls, it.name, clen)) {
                    return it.n;
                }
            }
          ATLTRACE(_T("Unknown class '%s' of %p\n"), cls, winHandle);
        }
        return UnknownCtl;
    }

//////////////////////////////////////////////////////////////////////////
// Multibyte group

#define _ON_CONST(X) case X: return #X
    PCSTR WM_C2SA(UINT inspected) {
        switch (inspected) {
#include "window.messages.consts.h"
        }
        return "";
    }
    PCSTR VK_C2SA(UINT inspected) {
        switch (inspected) {
#include "virtual.keys.consts.h"
        }
        return "";
    }
#define _BEG_CONST_GROUP(X)         \
        if (0 != (winType & X)) {    \
            switch (code) {
#define _END_CONST_GROUP()            \
            };                         \
        }
    PCSTR WM_NC_C2SA(UINT code, HWND win) {
        uint64_t winType = GetWindowType(win);
#include "notify.codes.h"
        return "";
    }
    PCSTR WM_NC_C2SA(UINT inspected) {
        switch (inspected) {
#include "notify.codes.plain.h"
        }
        return "";
    }
#undef _ON_CONST
#define _ON_BITMASK(mask)       \
    if (0 != (mask & style)) {   \
        if (!result.empty()) {    \
            result += " ";         \
        }                           \
        result += #mask;             \
    }
    LString LvStyleStringA(DWORD style)
    {
        LString result;
#include "list.view.styles.h"
        return result;
    }

    LString LvStyleExStringA(DWORD style)
    {
        LString result;
#include "list.view.exstyles.h"
        return result;
    }

#undef _ON_BITMASK

//////////////////////////////////////////////////////////////////////////
// Wide group

#define _ON_CONST(X) case X: return L#X
    PCWSTR WM_C2SW(UINT inspected) {
        switch (inspected) {
#include "window.messages.consts.h"
        }
        return L"";
    }
    PCWSTR VK_C2SW(UINT inspected) {
        switch (inspected) {
#include "virtual.keys.consts.h"
        }
        return L"";
    }
    PCWSTR WM_NC_C2SW(UINT code, HWND win) {
        uint64_t winType = GetWindowType(win);
#include "notify.codes.h"
        return L"";
    }
    PCWSTR WM_NC_C2SW(UINT inspected) {
        switch (inspected) {
#include "notify.codes.plain.h"
        }
        return L"";
    }
#undef _ON_CONST

#define _ON_BITMASK(mask)       \
    if (0 != (mask & style)) {   \
        if (!result.empty()) {    \
            result += L" ";        \
        }                           \
        result += L#mask;            \
    }

    WString LvStyleStringW(DWORD style)
    {
        WString result;
#include "list.view.styles.h"
        return result;
    }
    WString LvStyleExStringW(DWORD style)
    {
        WString result;
#include "list.view.exstyles.h"
        return result;
    }
#undef _ON_BITMASK

    WString MessageToStrignW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        wchar_t buffer[512] = { 0 };
        int destLen = _snwprintf_s(buffer, _countof(buffer),
            L"%p %08x %08x%08x %32s %04x",
            hWnd,
            static_cast<uint32_t>(wParam),
            static_cast<uint32_t>(lParam >> 32),
            static_cast<uint32_t>(lParam & 0xffffffff),
            WM_C2SW(uMsg), uMsg
        );
        return WString{ buffer, static_cast<size_t>(destLen) };
    }

    WString MessageToStrignW(const PMSG pMsg)
    {
        return MessageToStrignW(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
    }
}
