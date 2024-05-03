///////////////////////////////////////////////////////////////////////////////////////////////////////
// wcdNail aka MuxMoH																				 //
// тестовое задание 4 info@gamesoft.ru																 //
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "res/resource.h"
#include "gfx.h"
#include "printing.h"
#include "3dlite.h"
#include "blin.h"

HWND hMain = NULL;
char *clsName = "wcd 3d line fuck off!";
char *wndName = "wcd gfx library";

#define wndWidth 800
#define wndHeight 600
#define BPP 16

int MaxX, MaxY;

LRESULT CALLBACK msgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	MaxX = GetSystemMetrics(SM_CXSCREEN);
	MaxY = GetSystemMetrics(SM_CYSCREEN);

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW || CS_VREDRAW;
	wc.hIcon = wc.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_MAINFRAME);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = msgProc;
	wc.lpszClassName = clsName;
	wc.lpszMenuName = NULL;
	
	if (!RegisterClassEx(&wc)) return -1;

	hMain = CreateWindowEx(0, wc.lpszClassName, wndName, 
						   WS_POPUPWINDOW, 
						   CW_USEDEFAULT, CW_USEDEFAULT,
						   CW_USEDEFAULT, CW_USEDEFAULT,
						   NULL, NULL, hInstance, NULL);
	
	if (!hMain) return -2;
	ShowWindow(hMain, 1);

	Blin *myVinil = new Blin();

	if (gfxInit(hMain, wndWidth, wndHeight, BPP)==DD_OK) { 

		Axises *axises = new Axises(40);

		axises->rotate(PI/4, 0, 0);
		myVinil->rotate(PI/4, 0, 0);

		fillSurface(ddfront, 0);
		loadFont8x16("res/first.bin");

		*myVinil << "res/vinyl-record.bmp";

		char screen_desc_string[64];
		sprintf(screen_desc_string, "%dx%d %d bpp", scrx, maxy, pixdepth);

		MSG msg;
		while (true) {

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

				if (msg.message == WM_QUIT) 
                    break;

				TranslateMessage(&msg);
       			DispatchMessage(&msg);
	
			} else {
				fillSurface(ddback, 0);

				locks(ddback);

                myVinil->rotate(.0, .01, .0);
                axises->rotate(.0, .01, .0);

				myVinil->show();
				myVinil->showedges();

				drawString(600, 16 + 2, screen_desc_string, 0xffde);
				drawString(600, 32 + 2, "wcdNail", 0xffae);
				drawString(600, 48 + 2, "рад представить:", 0xffae);
				drawString(600, 64 + 2, "myVinil tro...", 0xffae);
				
				axises->projection(100, 500);
				axises->show(63, 63<<6, 63<<12);

				unlocks(ddback);
				ddfront->Flip(ddback, 0);
			}
		}

		gfxDone();
		retResultAnalizer(hMain);
		return 0;
	}

	gfxDone();
	retResultAnalizer(hMain);
	return 0xFF;
}

LRESULT CALLBACK msgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch(Msg)	{

	case WM_KEYDOWN:
		if (LOBYTE(wParam) == VK_ESCAPE) SendMessage(hWnd, WM_DESTROY, 0, 0l);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	//case WM_SETCURSOR:
	//	SetCursor(NULL);
	//	return 0;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

