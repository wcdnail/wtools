#include "stdafx.h"
#include "win32.host.window.h"
#include <string.utils.error.code.h>
#include <windows.uses.ole.h>
#include <windows.gui.leaks.h>
#include "dialogz.find.h"
#include "res/resource.h"

#define WIN32_HOST_APP_CLASS _T("W32_WHOSTWIN")

namespace Simple
{
    ATOM MyRegisterClass(HINSTANCE);
    bool InitInstance(HINSTANCE, int);
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void DropFailBox(PCTSTR caption, PCTSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        CString message;
        message.FormatV(format, ap);
        va_end(ap);

        ::MessageBox(NULL, message, caption, MB_ICONSTOP);
    }

    int AppEntry(HINSTANCE instance, HINSTANCE, LPTSTR, int showCommand)
    {
        ::setlocale(LC_ALL, "");
        ::DefWindowProc(NULL, 0, 0, 0L);
        Cf::GUILeaks guiLeaks;
        Initialize::OLE oleUser;

        INITCOMMONCONTROLSEX iccx = { sizeof(iccx), 0
            //| ICC_LISTVIEW_CLASSES   
            //| ICC_TREEVIEW_CLASSES   
            //| ICC_BAR_CLASSES        
            //| ICC_TAB_CLASSES        
            //| ICC_UPDOWN_CLASS       
            //| ICC_PROGRESS_CLASS     
            //| ICC_HOTKEY_CLASS       
            //| ICC_ANIMATE_CLASS      
            | ICC_WIN95_CLASSES      
            //| ICC_DATE_CLASSES       
            //| ICC_USEREX_CLASSES     
            //| ICC_COOL_CLASSES       
#if (_WIN32_IE >= 0x0400)
            //| ICC_INTERNET_CLASSES   
            //| ICC_PAGESCROLLER_CLASS 
            //| ICC_NATIVEFNTCTL_CLASS 
#endif
#if (_WIN32_WINNT >= 0x0501)
            //| ICC_STANDARD_CLASSES   
            //| ICC_LINK_CLASS         
#endif
        };

        HRESULT rv = S_OK;

        if (!::InitCommonControlsEx(&iccx))
        {
            rv = GetLastError();

            DropFailBox(_T("ќшибка при старте")
                , _T("—бой InitCommonControlsEx(%p[%d, %08x]) - %s")
                , &iccx, iccx.dwSize, iccx.dwICC
                , Str::ErrorCode<TCHAR>::SystemMessage(rv));
        }

        ATOM atom = MyRegisterClass(instance);
        if (!InitInstance(instance, showCommand))
        {
            rv = ::GetLastError();

            DropFailBox(_T("ќшибка при создании")
                , _T("—бой ::InitInstance() - %s")
                , Str::ErrorCode<TCHAR>::SystemMessage(rv));
        }
        else
        {
            CMessageLoop loop;
            rv = loop.Run();
        }

        if (!::UnregisterClass(WIN32_HOST_APP_CLASS, instance))
        {
            rv = ::GetLastError();

            DropFailBox(_T("ќшибка при де-регистрации")
                , _T("—бой ::UnregisterClass(...) - %s")
                , Str::ErrorCode<TCHAR>::SystemMessage(rv));
        }

        return (int)rv;
    }

    static ATOM MyRegisterClass(HINSTANCE instance)
    {
        CIcon icon(::LoadIcon(instance, MAKEINTRESOURCE(IDI_MAINFRAME)));
        CCursor cursor(::LoadCursor(NULL, IDC_ARROW));

        ATOM rv = 0;
        {
            WNDCLASSEX w;
            w.cbSize        = sizeof(WNDCLASSEX);
            w.style         = CS_HREDRAW | CS_VREDRAW;
            w.lpfnWndProc   = WndProc;
            w.cbClsExtra    = 0;
            w.cbWndExtra    = 0;
            w.hInstance     = instance;
            w.hIcon         = icon;
            w.hCursor       = cursor;
            w.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
            w.lpszMenuName  = NULL;
            w.lpszClassName = WIN32_HOST_APP_CLASS;
            w.hIconSm       = icon;

            rv = ::RegisterClassEx(&w);
        }

        return rv;
    }

    static bool InitInstance(HINSTANCE instance, int showCommand)
    {
        std::wstring modname(4096, L'\0');
        ::GetModuleFileNameW(instance, &modname[0], 4095);
        boost::filesystem::path modpath = modname;

        std::wstring curdir(4096, L'\0');
        ::GetCurrentDirectory(4095, &curdir[0]);

        CStringW caption;
        caption.Format(L"%s [%s] [%s]"
            , modpath.filename().c_str()
            , modpath.remove_filename().c_str()
            , curdir.c_str());

        HWND mainFrame = ::CreateWindowEx(WS_EX_APPWINDOW 
            , WIN32_HOST_APP_CLASS, caption
            , WS_OVERLAPPEDWINDOW
            | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
            , CW_USEDEFAULT, CW_USEDEFAULT, 900, 400
            , NULL, NULL, instance, NULL);

        if (!mainFrame)
            return false;

        ::ShowWindow(mainFrame, showCommand);
        ::UpdateWindow(mainFrame);

        return true;
    }

    static LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_KEYDOWN:      Window::OnKeyDown(window, (UINT)wParam, (UINT)lParam & 0xffff, (UINT)((lParam & 0xffff0000) >> 16)); break;
        case WM_ACTIVATE:     Window::OnActivate(window, (UINT)LOWORD(wParam), (BOOL)HIWORD(wParam), (HWND)lParam); break;
        case WM_SETFOCUS:     Window::OnSetFocus(window, (HWND)wParam); break;
        case WM_CREATE:       Window::OnCreate(window, (LPCREATESTRUCT)lParam); break;
        case WM_COMMAND:      Window::OnCommand(window, LOWORD(wParam), HIWORD(wParam), lParam); break;
        case WM_PAINT:        Window::OnPaint(window); break;
        case WM_THEMECHANGED: Window::OnThemeChanged(window); break;
        case WM_SIZE:         Window::OnSize(window, message, wParam, lParam); break;
        case WM_LBUTTONDOWN:  Window::OnLButtonDown(window, message, wParam, lParam); break;  
        case WM_LBUTTONUP:    Window::OnLButtonUp(window, message, wParam, lParam); break;  
        case WM_MOUSEMOVE:    Window::OnMouseMove(window, message, wParam, lParam); break; 
        case WM_TIMER:        Window::OnTimer(window, message, wParam, lParam); break; 
        
        case WM_DESTROY:      
            Window::OnDestroy(window); 
            ::PostQuitMessage(0);

        default:
            return ::DefWindowProc(window, message, wParam, lParam);
        }

        return 0;
    }
}
