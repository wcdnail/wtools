#include "stdafx.h"
#if 0
#include "unit.tests.h"
#include "fs.enum.h"
#include "fs.links.h"
#include "df.atl.trace.h"
#include "strut.error.code.h"
#include "panel.view.h"
#include "config.h"
#include "strut.error.code.h"
#include "native.window.h"
#include "wtl.window.h"
#include "memory.stuff.h"
#include "my.window.messages.h"
#include <crash.report/run.context.h>
#include "res/resource.h"
#include <string>
#include <algorithm>
#include <boost/bind.hpp>

HINSTANCE hInst;
PCTSTR szTitle = _T("WCD: CFUT");
PCTSTR szWindowClass = _T("[WCD]CF_UT_CLS");

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#if _CK_FS_ENUM
void TempReadDir(PCTSTR pathName)
{
    HANDLE dir = Twins::FS::OpenDirectory(pathName, false);
    if (INVALID_HANDLE_VALUE != dir) 
    {
        char buffer[4096] = {0};
        DWORD readed = 0;
        ::ReadFile(dir, buffer, sizeof(buffer), &readed, NULL);
        HRESULT hr = ::GetLastError();
        Twins::Debug::Printf(_T("RDLINK: `%s` %s %d (0x%x)\n"), pathName, Twins::ErrorCode<TCHAR>::SystemMessage(hr), hr, hr);
        ::CloseHandle(dir);
    }
}
#endif

extern "C" void crash_report(char const* reason, char const* source, int line, int errcode, RUN_CONTEXT const* rc, int terminate) {}
extern "C" RUN_CONTEXT_PTR capture_context(void) { return NULL; }

void DropFailBox(PCTSTR caption, PCTSTR format, ...)
{
    va_list ap;
    va_start(ap, format);
    CString message;
    message.FormatV(format, ap);
    va_end(ap);

    ::MessageBox(NULL, message, caption, MB_ICONSTOP);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    {
        MemoryStuff memStuff;
    }

    ::setlocale(LC_ALL, "");

    HRESULT mainRv = S_OK;
    mainRv = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(mainRv));

    ::SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    ::DefWindowProc(NULL, 0, 0, 0L);

    INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_BAR_CLASSES
        //| ICC_LISTVIEW_CLASSES 
        //| ICC_TREEVIEW_CLASSES
        | ICC_BAR_CLASSES
        //| ICC_TAB_CLASSES
        //| ICC_PROGRESS_CLASS
        //| ICC_HOTKEY_CLASS
        //| ICC_USEREX_CLASSES
        //| ICC_COOL_CLASSES 
        //| ICC_PAGESCROLLER_CLASS
        //| ICC_NATIVEFNTCTL_CLASS
    };

    if (!::InitCommonControlsEx(&iccx))
        ::DropFailBox(_T("ќшибка при старте"), _T("—бой InitCommonControlsEx(%p[%d, %08x]) - %s"), &iccx, iccx.dwSize, iccx.dwICC, Twins::ErrorCode<TCHAR>::SystemMessage(GetLastError()));

#if _CK_FS_ENUM
    TempReadDir(_T("c:\\Documents and Settings"));
    TempReadDir(_T("c:\\Users\\Default User"));

    Twins::FS::Enumerate(_T("c:\\"), _T("Documents and Settings"), _T("*"), Twins::FS::Enumerator());
    Twins::FS::Enumerate(_T("c:\\"), _T("Users\\Default User"), _T("*"), Twins::FS::Enumerator());
    Twins::FS::Enumerate(_T("c:\\"), _T("Documents and Settings\\Default User"), _T("*"), Twins::FS::Enumerator());


#else

    CAppModule _Module;
    CMessageLoop loop;

    mainRv = _Module.Init(NULL, hInst);
    _Module.AddMessageLoop(&loop);

#if _USE_WTL_WINDOW
    HLWindow mainWindow(loop);
    if (!mainWindow.Create())
    {
        ::DropFailBox(_T("ќшибка при создании"), _T("—бой HLWindow::Create() - %s"), Twins::ErrorCode<TCHAR>::SystemMessage(::GetLastError()));
        return -2;
    }

    mainWindow.ShowWindow(SW_SHOW);
    mainWindow.UpdateWindow();
#else
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
    {
        ::DropFailBox(_T("ќшибка при создании"), _T("—бой ::InitInstance() - %s"), Twins::ErrorCode<TCHAR>::SystemMessage(::GetLastError()));
        return -1;
    }
#endif // _USE_WTL_WINDOW

    mainRv = loop.Run();
    _Module.RemoveMessageLoop();
    _Module.Term();

#endif

    ::CoUninitialize();
    return (int)mainRv;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowEx(WS_EX_APPWINDOW 
       //| WS_EX_COMPOSITED
       , szWindowClass, szTitle
       , WS_OVERLAPPEDWINDOW
       | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
       , CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
    case WM_ACTIVATE:   Native::Window::OnActivate(hWnd, (UINT)LOWORD(wParam), (BOOL)HIWORD(wParam), (HWND)lParam); break;
    case WM_SETFOCUS:   Native::Window::OnSetFocus(hWnd, (HWND)wParam); break;
    case WM_CREATE:     Native::Window::OnCreate(hWnd, (LPCREATESTRUCT)lParam); break;
	case WM_COMMAND:    Native::Window::OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), lParam); break;
	case WM_PAINT:      Native::Window::OnPaint(hWnd); break;
	case WM_DESTROY:    Native::Window::OnDestroy(hWnd); break; 
#if _UT_PANELVIEW
    case Twins::WM::PanelViewSelectItem : Native::Window::OnPv0SelectItem(hWnd, wParam, lParam); break; 
#endif 
    case WM_SIZE:       
    {
        Native::Window::OnSize(hWnd, message, wParam, lParam);

        SIZE sz = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        Native::Window::OnSize(hWnd, (UINT)wParam, sz); 
        break;
    }
    case WM_THEMECHANGED: Native::Window::OnThemeChanged(hWnd); break;
    default:            
        return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
#endif // 0
