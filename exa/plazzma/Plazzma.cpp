#include "stdafx.h"
#include "resource.h"
#include "gfx.h"
#include "ereport.h"
#include "kernel.h"

namespace
{
    enum Hardcoded
    {
        RequiredMaxX = 1024,
        RequiredMaxY = 768,
        RequiredBitDepth = 8,
        WM_CREATEPALLETE = WM_USER + 0x1000
    };

    static const char gWindowClassName[] = "wcd_stuff";
    static const char gWindowTitle[] = "Thatz PLaZZmA, DuDe!";
    static PALETTEENTRY gDefaultPalette[256] = {0};
}

LRESULT CALLBACK msgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) 
{
    switch(Msg)	
    {
    case WM_KEYDOWN:
        if (LOBYTE(wParam) == VK_ESCAPE) 
        {
            elogMessage("Esc pressed", 0);
            ::DestroyWindow(hWnd);
        }
        return 0;

    case WM_DESTROY:
        elogMessage("Post quit message", 0);
        PostQuitMessage(0);
        return 0;

    case WM_PALETTECHANGED:
        gfxSetPalette(gDefaultPalette, _countof(gDefaultPalette));
        return 0;
    }

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    ::CoInitialize(NULL);
    ::DefWindowProc(NULL, 0, 0, 0l);

    try
    {
        elogOpen("Plazzma.log");

        DWORD winVer = ::GetVersion();
        DWORD majWinVer = (DWORD)(LOBYTE(LOWORD(winVer)));
        DWORD minWinVer = (DWORD)(HIBYTE(LOWORD(winVer)));

        elogMessage(0, "Версия ос %d.%d", majWinVer, minWinVer);

        gfxBlendColors(gDefaultPalette, _countof(gDefaultPalette));

        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(wc));

        wc.cbSize = sizeof(wc);
        wc.hInstance = hInstance;
        wc.style = CS_HREDRAW || CS_VREDRAW;
        wc.hIcon = wc.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_MAIN);
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wc.lpfnWndProc = msgProc;
        wc.lpszClassName = gWindowClassName;
        wc.lpszMenuName = NULL;

        ATOM classAtom = RegisterClassEx(&wc);
        elogMessage("Регистрирую класс окна", classAtom ? 0 : ::GetLastError());
        if (!classAtom)
            return -1;

        int wcx = ::GetSystemMetrics(SM_CXFULLSCREEN);
        int wcy = ::GetSystemMetrics(SM_CYFULLSCREEN);

        HWND hMain = CreateWindowEx(0, wc.lpszClassName, gWindowTitle, 
            WS_POPUP, 0, 0, wcx, wcy,
            NULL, NULL, hInstance, NULL);

        elogMessage("Создаю окно", ::GetLastError());
        if (!hMain)		
            return -2;

        ShowWindow(hMain, 1);

        if (gfxInit(hMain, RequiredMaxX, RequiredMaxY, RequiredBitDepth) && 
            PlazzmaInit(256, 256)) 
        { 
            PlazzmaCalc();

            if (majWinVer < 6)
                gfxCreatePalette(gDefaultPalette, _countof(gDefaultPalette));

            elogMessage("Запуск цикла сообщений", 0);
            while (true) 
            {
                MSG msg = {0};
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
                {
                    if (msg.message == WM_QUIT) 
                        break;

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                } 

                BOOL vsync = FALSE;
                while (!vsync)
                    ddIface->GetVerticalBlankStatus(&vsync);
                
                PlazzmaBlender();
                gfxBlendColors(gDefaultPalette, _countof(gDefaultPalette));

                if (majWinVer < 6)
                    ::SendMessage(hMain, WM_PALETTECHANGED, 0, 0l);
                else
                {
                    gfxNewPalette(gDefaultPalette, _countof(gDefaultPalette));
                }
            }

            PlazzmaDestroy();
            elogMessage("All done, sending kill signal", 0);
        }	
    }
    catch (...)
    {
        elogMessage("Unhandled exception", ::GetLastError());
    }

    gfxDone();
    elogMessage("Normal termination", 0);
    elogClose();

    ::CoUninitialize();
    return 0;
}
