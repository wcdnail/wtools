#include "string.buffer.h"
#include <stdlib.h>

#ifdef _MSC_VER
#  pragma warning(disable: 4312) // warning C4312: 'type cast': conversion from 'int' to 'HMENU' of greater size
#endif
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVER.h>
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <shellapi.h>

#define RIGHT_BOTTOM_CORNER 0
#define STD_COLORS_USED     0
#define _CRASH_REPORT_CX    800
#define _CRASH_REPORT_CY    500

typedef struct 
{
    HINSTANCE module;
    HWND parentWindow;
    HWND window;
    PCWSTR className;
    PCWSTR title;
    PCWSTR message;
    HFONT font;
}
REPORTER_INFO, *LPREPORTER_INFO, *PREPORTER_INFO;

static LRESULT WINAPI crash_report_window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH BackgroundBrush = NULL;
    static HPEN EdgePen = NULL;
    static COLORREF TextColor = 0;
    static COLORREF DisabledTextColor = 0;
    static HICON MainIcon;
    static HICON WindowIcon;

    enum
    {
        IDC_REASON    = 1001,
        IDC_LEFTICON  = 1002,
        IDC_BTN_FIRST = 1003,
        IDC_BTN_CLOSE = IDC_BTN_FIRST,
        IDC_BTN_COPY,
        IDC_BTN_SEND,
        IDC_BTN_LAST = IDC_BTN_SEND,
    };

    switch (message)
    {
    case WM_ERASEBKGND:
    {
        HDC dc = (HDC)wParam;
        HBRUSH br = BackgroundBrush;
        HGDIOBJ last = SelectObject(dc, br);

        RECT rc;
        GetClientRect(window, &rc);

        Rectangle(dc, 0, 0, rc.right, rc.bottom);

        SelectObject(dc, last);
        return (LRESULT)br;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORDLG:
    {
        HDC dc = (HDC)wParam;
        HBRUSH br = BackgroundBrush;
        LOGBRUSH lbr;
        GetObject(br, sizeof(lbr), &lbr);

        SetBkMode(dc, OPAQUE);
        SetBkColor(dc, lbr.lbColor);
        SetTextColor(dc, TextColor);

        return (LRESULT)br;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT di = (LPDRAWITEMSTRUCT)lParam;
        HDC              dc = di->hDC;
        RECT             rc = di->rcItem;
        int            tlen = 0;
        TCHAR      text[64] = {0};

        HGDIOBJ last = SelectObject(dc, BackgroundBrush);
        SelectObject(dc, EdgePen);
        Rectangle(dc, 0, 0, rc.right, rc.bottom);

        tlen = GetWindowText(di->hwndItem, text, _countof(text)-1);
        if (tlen) {
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, (ODS_DISABLED != di->itemState ? TextColor : DisabledTextColor));
            DrawText(dc, text, tlen, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        if (ODS_FOCUS == di->itemState) {
            RECT rcf = rc;
            InflateRect(&rcf, -2, -2);
            DrawFocusRect(dc, &rcf);
        }
        if (ODS_SELECTED == di->itemState) {
            InvertRect(dc, &rc);
        }
        SelectObject(dc, last);
        return 0;
    }

    case WM_COMMAND: {
        WORD  code = HIWORD(wParam);
        WORD    id = LOWORD(wParam);
        HWND child = (HWND)lParam;

        if (NULL != child) {
            if (BN_CLICKED == code) {
                switch (id) {
              //case IDC_BTN_SEND:
              //    SendByMail();
              //    break;
                case IDC_BTN_CLOSE:
                    DestroyWindow(window);
                    break;
                case IDC_BTN_COPY: {
                    if (OpenClipboard(NULL)) {
                        HWND text = GetDlgItem(window, IDC_REASON);
                        int len = GetWindowTextLengthA(text);
                        EmptyClipboard();
                        if (len > 0) {
                            HGLOBAL glob = GlobalAlloc(GHND, (len+1)*sizeof(char));
                            if (NULL != glob) {
                                PSTR textStr = (PSTR)GlobalLock(glob);
                                GetWindowTextA(text, textStr, len+1);
                                SetClipboardData(CF_TEXT, glob);
                                GlobalUnlock(glob);
                            }
                        }
                        CloseClipboard();
                    }
                    break;
                }
                }
            }
        }
        break;
    }
    case WM_CREATE: {
        TCHAR modulePath[4096] = {0};
        HICON      tempIcon[1] = {0};
        LPCREATESTRUCT      cs = (LPCREATESTRUCT)lParam;
        LPREPORTER_INFO   info = (LPREPORTER_INFO)cs->lpCreateParams;
        HWND              text = NULL;
        HWND              icon = NULL;
        RECT                rc;
        int                 nx;
        int                 ny;
        int                 bx;
        int                  i;
        static PCWSTR buttonText[] = {
            L"&X Close", 
            L"&C Copy", 
          //L"&S Send report",
        };
        HWND button[_countof(buttonText)];

        static const int    iconWidth = 54;
        static const int   iconHeight = 54;
        static const int  buttonWidth = 140;
        static const int buttonHeight = 24;
        // ##TODO: Customize colors....

#if STD_COLORS_USED
        TextColor = GetSysColor(COLOR_BTNTEXT);
        DisabledTextColor = GetSysColor(COLOR_GRAYTEXT);
        BackgroundBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        EdgePen = CreatePen(PS_SOLID, 1, TextColor);
#else
        TextColor = RGB(255, 253, 221);
        DisabledTextColor = RGB(255 - 64, 253 - 64, 221 - 64);
        BackgroundBrush = CreateSolidBrush(RGB(0, 0, 64));
        EdgePen = CreatePen(PS_SOLID, 1, RGB(255, 149, 149));
#endif

        GetClientRect(window, &rc);

#if RIGHT_BOTTOM_CORNER
        nx = GetSystemMetrics(SM_CXFULLSCREEN) - (GetSystemMetrics(SM_CXDLGFRAME) * 2) - cs->cx;
        ny = GetSystemMetrics(SM_CYFULLSCREEN) - cs->cy;
#else
        nx = (GetSystemMetrics(SM_CXSCREEN) - cs->cx)/2;
        ny = (GetSystemMetrics(SM_CYSCREEN) - cs->cy)/2;
#endif
        MoveWindow(window, nx, ny, cs->cx, cs->cy, FALSE);

        WindowIcon = LoadIcon(NULL, IDI_ERROR);

        GetModuleFileName(NULL, modulePath, _countof(modulePath)-1);
        ExtractIconEx(modulePath, 0, tempIcon, NULL, 1);
        MainIcon = tempIcon[0] ? tempIcon[0] : LoadIcon(NULL, IDI_WARNING);

        SendMessage(window, WM_SETICON, (WPARAM)1, (LPARAM)WindowIcon);
        SendMessage(window, WM_SETICON, (WPARAM)0, (LPARAM)WindowIcon);

        SendMessage(window, WM_SETFONT, (WPARAM)info->font, (LPARAM)TRUE);

        text = CreateWindowExW(0 
            //| WS_EX_STATICEDGE 
            , L"EDIT"
            , info->message
            , 0 
            //| WS_TABSTOP
            | WS_CHILD
            | WS_VSCROLL // | WS_HSCROLL | //WS_BORDER
            | ES_READONLY | ES_MULTILINE 
            | ES_AUTOVSCROLL | ES_AUTOHSCROLL
            | ES_WANTRETURN
            , iconWidth+4, 24, rc.right-(iconWidth+12), rc.bottom-(buttonHeight + 34)
            , window, (HMENU)IDC_REASON, info->module, NULL);

        SendMessage(text, WM_SETFONT, (WPARAM)info->font, (LPARAM)FALSE);
        ShowWindow(text, 1);

        icon = CreateWindowExW(0
            | WS_EX_TRANSPARENT
            , L"STATIC", NULL
            , WS_CHILD | WS_VISIBLE 
            | SS_ICON | SS_CENTER | SS_CENTERIMAGE
            , 2, 2, iconWidth, iconHeight
            , window, (HMENU)IDC_LEFTICON, info->module, NULL);

        SendMessage(icon, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)MainIcon);
        SendMessage(icon, WM_SETFONT, (WPARAM)info->font, (LPARAM)FALSE);

        bx = 2 + ((rc.right-4) - (buttonWidth * _countof(button)))/2;
        for (i=0; i<_countof(button); i++) {
            button[i] = CreateWindowExW(0, L"BUTTON", buttonText[i]
                , WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW 
                , bx, rc.bottom-(buttonHeight+2), buttonWidth, buttonHeight
                , window, (HMENU)(IDC_BTN_FIRST+i), info->module, NULL);

            bx += (buttonWidth + 2);
            SendMessage(button[i], WM_SETFONT, (WPARAM)info->font, (LPARAM)FALSE);
        }
        SetFocus(text);
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(EdgePen);
        DeleteObject(BackgroundBrush);
        DestroyIcon(MainIcon);
        DestroyIcon(WindowIcon);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(window, message, wParam, lParam);
    }

    return 0;
}

static BOOL create_crash_report_window(PREPORTER_INFO info)
{
    WNDCLASSEXW wc;
    ATOM regAtom;
    wc.cbSize           = sizeof(wc);
    wc.hInstance        = info->module;
    wc.lpszClassName    = info->className;
    wc.style            = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc      = crash_report_window_proc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hIcon            = NULL;
    wc.hIconSm          = wc.hIcon;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName     = NULL;
    regAtom = RegisterClassExW(&wc);
    if (0 == regAtom) {
        return (FALSE);
    }
    info->window = CreateWindowExW(0, info->className, info->title,
        WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, _CRASH_REPORT_CX, _CRASH_REPORT_CY, info->parentWindow, NULL, info->module, info);
    if (NULL != info->window) {
        ShowWindow(info->window, 1);
        UpdateWindow(info->window);
    }
    return (NULL != info->window);
}

void crash_report_window(STRING_BUFFER_PTR buffer)
{
    REPORTER_INFO rinfo;
    wchar_t* message = NULL;
    HMODULE moduleHandle = GetModuleHandle(NULL);
    assert(NULL != buffer);
    message = (wchar_t*)calloc(buffer->size, sizeof(wchar_t));
    if (NULL == message) {
        MessageBoxA(GetActiveWindow(), buffer->string, NULL, MB_ICONERROR);
        return ;
    }
    MultiByteToWideChar(CP_UTF8, 0, buffer->string, (int)buffer->size, message, (int)buffer->size);
    /* OutputDebugStringW(message); */
    rinfo.module       = moduleHandle;
    rinfo.parentWindow = GetActiveWindow();
    rinfo.window       = NULL;
    rinfo.className    = L"WCD_CRASH_REPORTER";
    rinfo.title        = GetCommandLineW();
    rinfo.message      = message;
    rinfo.font         = CreateFontW(-12, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, PROOF_QUALITY, 0, 
                            //L"Courier"
                            L"Courier"
                            //L"Terminal"
                            );
    if (!create_crash_report_window(&rinfo)) {
        MessageBoxW(rinfo.parentWindow, rinfo.message, rinfo.title, MB_ICONERROR);
    }
    else {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            if (IsDialogMessage(rinfo.window, &msg)) {
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        UnregisterClassW(rinfo.className, rinfo.module);
    }
    DeleteFont(rinfo.font);
    free((void*)message);
}
