#pragma once
#if 0

#include "unit.tests.h"
#include "panel.view.h"
#include "panel.view.0.h"
#include "config.h"
#include <winuser.h>

namespace Native
{
    namespace Window
    {
#if (!_UT_GDIBASE)

#if _UT_LISTVIEWCTL
        extern Twins::PanelView gPanelView;
#endif

#if _UT_PANELVIEW
        extern Twins::PanelView0 gPanelView0;

        void OnPv0SelectItem(HWND window, WPARAM wParam, LPARAM lParam);
#endif
#endif

        void OnCommand(HWND window, int id, int code, LPARAM param);
        void OnPaint(HWND window);
        void OnDestroy(HWND window);
        void OnActivate(HWND window, UINT state, BOOL bMinimized, HWND wndOther);
        void OnSetFocus(HWND window, HWND wndOther);
        void OnSize(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        void OnSize(HWND window, UINT type, SIZE const& size);
        void OnThemeChanged(HWND window);
        int OnCreate(HWND window, LPCREATESTRUCT csPtr);
    }
}
#endif // 0
