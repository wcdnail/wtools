#pragma once

namespace Simple
{
    namespace Window
    {
        void OnCommand(HWND window, int id, int code, LPARAM param);
        void OnPaint(HWND window);
        void OnDestroy(HWND window);
        void OnActivate(HWND window, UINT state, BOOL bMinimized, HWND wndOther);
        void OnSetFocus(HWND window, HWND wndOther);
        void OnSize(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        void OnThemeChanged(HWND window);
        int OnCreate(HWND window, LPCREATESTRUCT csPtr);
        void OnLButtonDown(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        void OnLButtonUp(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        void OnMouseMove(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        void OnTimer(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        void OnKeyDown(HWND window, UINT code, UINT, UINT flags);
        void ShowFindDialog(PCWSTR dirpath);
    }
}
