#include "stdafx.h"

static const wchar_t* kWindowClass = L"BrokenGlassWindow";
static const wchar_t* kWindowTitle = L"BrokenGlass - Right click client area to toggle frame type.";
static const int kGlassBorderSize = 20;
static const int kNonGlassBorderSize = 40;

static bool g_glass = true;

#pragma comment(lib, "DWMAPI")

bool IsGlass() 
{
    BOOL composition_enabled = FALSE;
    return DwmIsCompositionEnabled(&composition_enabled) == S_OK && composition_enabled && g_glass;
}
void SetIsGlass(bool is_glass) 
{
    g_glass = is_glass;
}

void ToggleGlass(HWND hwnd) 
{
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) 
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT wr;
    HBRUSH br;
    RECT* nccr = NULL;
    RECT dirty;
    RECT dirty_box;
    MARGINS dwmm = {0};
    WINDOWPOS* wp = NULL;

    switch (message) 
    {
    case WM_CREATE:
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &wr);
        br = GetSysColorBrush(IsGlass() ? COLOR_APPWORKSPACE : COLOR_WINDOW);
        FillRect(hdc, &wr, br);
        EndPaint(hwnd, &ps);
        break;

    case WM_NCPAINT:
        if (IsGlass())
            return DefWindowProc(hwnd, message, w_param, l_param);

        ::GetWindowRect(hwnd, &wr);
        if (!w_param|| w_param == 1) 
        {
            dirty = wr;
            dirty.left = dirty.top = 0;
        } 
        else 
        {
            ::GetRgnBox((HRGN)w_param, &dirty_box);
            if (!::IntersectRect(&dirty, &dirty_box, &wr))
                return 0;
            ::OffsetRect(&dirty, -wr.left, -wr.top);
        }

        hdc = ::GetWindowDC(hwnd);
        br = ::CreateSolidBrush(RGB(255,0,0));
        ::FillRect(hdc, &dirty, br);
        ::DeleteObject(br);
        ::ReleaseDC(hwnd, hdc);
        break;

    case WM_NCACTIVATE:
        // Force paint our non-client area otherwise Windows will paint its own.
        RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
        break;

    case WM_NCCALCSIZE:
        nccr = w_param ? &reinterpret_cast<NCCALCSIZE_PARAMS*>(l_param)->rgrc[0] : reinterpret_cast<RECT*>(l_param);

        nccr->bottom -= IsGlass() ? kGlassBorderSize : kNonGlassBorderSize;
        nccr->right -= IsGlass() ? kGlassBorderSize : kNonGlassBorderSize;
        nccr->left += IsGlass() ? kGlassBorderSize : kNonGlassBorderSize;
        nccr->top += IsGlass() ? kGlassBorderSize : kNonGlassBorderSize;

        return WVR_REDRAW;

    case WM_RBUTTONDOWN:
        SetIsGlass(!g_glass);
        ToggleGlass(hwnd);
        break;

    case 0x31e: // WM_DWMCOMPOSITIONCHANGED:
        ToggleGlass(hwnd);
        break;    

    case 0xae: // WM_NCUAHDRAWCAPTION:
    case 0xaf: // WM_NCUAHDRAWFRAME:
        return IsGlass() ? DefWindowProc(hwnd, message, w_param, l_param) : 0;

    case WM_WINDOWPOSCHANGED:
        dwmm.cxLeftWidth = kGlassBorderSize;
        dwmm.cxRightWidth = kGlassBorderSize;
        dwmm.cyTopHeight = kGlassBorderSize;
        dwmm.cyBottomHeight = kGlassBorderSize;
        DwmExtendFrameIntoClientArea(hwnd, &dwmm);
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, w_param, l_param);
    }

    return 0;
}

ATOM RegisterClazz(HINSTANCE instance) 
{
    WNDCLASSEX wcex = {0};

    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = instance;
    wcex.lpszClassName = kWindowClass;

    return ::RegisterClassEx(&wcex);
}

int WINAPI RunTinyWindow(HINSTANCE instance, HINSTANCE, LPSTR, int show_command) 
{
    ::RegisterClazz(instance);
    HWND hwnd = ::CreateWindow(kWindowClass, kWindowTitle, WS_OVERLAPPEDWINDOW, 100, 100, 400, 300, NULL, NULL, instance, NULL);
    ::ShowWindow(hwnd, show_command);

    MSG msg = {0};
    while (::GetMessage(&msg, NULL, 0, 0)) 
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return 0;
}
