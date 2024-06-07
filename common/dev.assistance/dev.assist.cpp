#include "stdafx.h"
#include "dev.assist.h"

// ReSharper disable CppClangTidyModernizeMacroToEnum
// ReSharper disable CppDefaultCaseNotHandledInSwitchStatement

#define WM_UAHDESTROYWINDOW     0x0090
#define WM_UAHDRAWMENU          0x0091
#define WM_UAHDRAWMENUITEM      0x0092
#define WM_UAHINITMENU          0x0093
#define WM_UAHMEASUREMENUITEM   0x0094
#define WM_UAHNCPAINTMENUPOPUP  0x0095

#define DA_BEG_WT_GROUP(X)          \
        if (X == wType) {            \
            switch (code) {
#define DA_END_WT_GROUP()           \
            };                       \
        }
#define DA_BEG_WT_GROUP_OTHERS()    \
        switch (code) {
#define DA_END_WT_GROUP_OTHERS()    \
        };                       

namespace DH
{
    uint64_t GetWindowType(HWND hWnd)
    {
        TCHAR szClass[256]{0};
        auto const   clLen{static_cast<size_t>(GetClassName(hWnd, szClass, _countof(szClass)))};
        if (clLen > 0) {
            struct WTYPE_NAME
            {
                PCTSTR  name;
                uint64_t   n;
            };
            static constexpr WTYPE_NAME wtItems[]{
                {     _T("Static"), WT_Static },
                {       _T("Edit"), WT_Edit },
                {     _T("Button"), WT_Button },
                {    _T("Listbox"), WT_ListBox },
                {   _T("Combobox"), WT_ComboBox },
                { TOOLBARCLASSNAME, WT_ToolBar },
                {   REBARCLASSNAME, WT_ReBar },
                {  STATUSCLASSNAME, WT_StatusBar }, // CommonCtl
            };
            for (const auto& [name, n]: wtItems) {
                if (0 == _tcsncicmp(szClass, name, clLen)) {
                    return n;
                }
            }
            ATLTRACE(_T("Unknown class '%s' of %p\n"), szClass, hWnd);
        }
        return WT_Unknown;
    }

    //------------------------------------------------------------------------
    // Narrow strings group

    #define DA_NUM_TO_STR(X) case X: return #X

    PCSTR WMNumToStrA(UINT inspected) {
        switch (inspected) {
        #include "window.messages.consts.h"
        }
        return "";
    }
    PCSTR VirtKeyToStrA(UINT inspected) {
        switch (inspected) {
        #include "virtual.keys.consts.h"
        }
        return "";
    }
    PCSTR NotifyCodeToStrA(UINT code, HWND win) {
        uint64_t const wType{GetWindowType(win)};
        #include "notify.codes.h"
        return "";
    }
    PCSTR NotifyCodeToStrA(UINT inspected) {
        switch (inspected) {
        #include "notify.codes.plain.h"
        }
        return "";
    }
    #undef DA_NUM_TO_STR
    #define DA_BITMASK_TO_STR(mask) \
        if (0 != ((mask) & style)) { \
            if (!result.empty()) {    \
                result += " ";         \
            }                           \
            result += #mask;             \
        }
    LString LvStyleStrA(DWORD style)
    {
        LString result;
        #include "list.view.styles.h"
        return result;
    }
    LString LvStyleExStrA(DWORD style)
    {
        LString result;
        #include "list.view.exstyles.h"
        return result;
    }
    LString WinPosFlagsStrA(UINT style)
    {
        LString result;
        #include "window.pos.flags.h"
        return result;
    }
    #undef DA_BITMASK_TO_STR

    //------------------------------------------------------------------------
    // Wide strings group
    #define DA_NUM_TO_STR(X) case X: return L#X
    PCWSTR WMNumToStrW(UINT inspected) {
        switch (inspected) {
        #include "window.messages.consts.h"
        }
        return L"";
    }
    PCWSTR VirtKeyToStrW(UINT inspected) {
        switch (inspected) {
        #include "virtual.keys.consts.h"
        }
        return L"";
    }
    PCWSTR NotifyCodeToStrW(UINT code, HWND win) {
        uint64_t const wType{GetWindowType(win)};
        #include "notify.codes.h"
        return L"";
    }
    PCWSTR NotifyCodeToStrW(UINT inspected) {
        switch (inspected) {
        #include "notify.codes.plain.h"
        }
        return L"";
    }
    #undef DA_NUM_TO_STR
    #define DA_BITMASK_TO_STR(mask)       \
        if (0 != (mask & style)) {   \
            if (!result.empty()) {    \
                result += L" ";        \
            }                           \
            result += L#mask;            \
        }
    WString LvStyleStrW(DWORD style)
    {
        WString result;
        #include "list.view.styles.h"
        return result;
    }
    WString LvStyleExStrW(DWORD style)
    {
        WString result;
        #include "list.view.exstyles.h"
        return result;
    }
    WString WinPosFlagsStrW(UINT style)
    {
        WString result;
        #include "window.pos.flags.h"
        return result;
    }
    #undef DA_BITMASK_TO_STR

    //------------------------------------------------------------------------
    // 'Dumpers'

    WString MessageToStrignW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        wchar_t buffer[512]{0};
        int const   destLen{_snwprintf_s(buffer, _countof(buffer),
                                         L"%p %08x %08x%08x %32s %04x",
                                         hWnd,
                                         static_cast<uint32_t>(wParam),
                                         static_cast<uint32_t>(lParam >> 32),
                                         static_cast<uint32_t>(lParam & 0xffffffff),
                                         WMNumToStrW(uMsg), uMsg)};
        return WString{ buffer, static_cast<size_t>(destLen) };
    }

    WString MessageToStrignW(const PMSG pMsg)
    {
        return MessageToStrignW(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
    }
} // namespace DH
