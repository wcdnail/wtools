#include "stdafx.h"
#include "Application.h"
#include "ErrorReport.h"
#include "DirectExtra.h"
#include "UI.h"
#include <typeinfo>

#define _DEBUG_SEH 1

#ifndef _NO_ADVANCED_BUG_REPORT
#include "afx/win.target.xp.h"
#include "afx/win.dll.h"
#include "afx/report.bugs.h"
#include "afx/error.seh.h"

LONG WINAPI SEH_flt(EXCEPTION_POINTERS *info) 
{
    SEHRecord seh(info);
    OutputDebugStringA(seh.Info());
    ShowBugReport(seh);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

static void Gen_SE()
{
    int j = 0;
    int i = 5 / j;

    char *p = (char*)0xebe0b0b0;
    *p = 'A';
}

static int Test_EH()
{
    PVOID result = (PVOID)0xaaaaaaee;

#ifndef _M_X64
    PVOID basePtr = 0;
    PVOID stackPtr = 0;
    __asm mov basePtr, ebp;
    __asm mov stackPtr, esp;
#endif

    __try
    {
        Gen_SE();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        result = (PVOID)GetExceptionCode();

        ATLTRACE(L"E: 0x%p\n", result);
        ATLTRACE(L"S: 0x%p\n", stackPtr);
        ATLTRACE(L"B: 0x%p\n", basePtr);
    }

    return (int)result;
}

void StartApp(HINSTANCE hInstance)
{
#if _DEBUG_SEH
    Test_EH();
    ::ExitProcess(0);
#endif

    try
    {
        EnterFunc();
        ::InitCommonControls();

        CApplication& MyApp = GetApp();
        MyApp.InitInstance(hInstance);
        MyApp.Run();
    }
    catch (Exception const& ex) 
    {
        OutputDebugStringA(ex.Dump());
#ifndef _NO_ADVANCED_BUG_REPORT
        ShowBugReport(ex);
#else
        MessageBoxA(NULL, ex.what(), NULL, MB_ICONSTOP);
#endif
    }
    catch (std::exception const& ex) 
    {
        OutputDebugStringA(ex.what());
#ifndef _NO_ADVANCED_BUG_REPORT
        ShowBugReport(ex);
#else
        MessageBoxA(NULL, ex.what(), NULL, MB_ICONSTOP);
#endif
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
#ifndef _NO_ADVANCED_BUG_REPORT
    __try 
    {
#endif
        StartApp(hInstance);
#ifndef _NO_ADVANCED_BUG_REPORT
    }
    __except(SEH_flt(GetExceptionInformation()))
    {}
#endif
	return 0;
}

CApplication& GetApp() 
{ 
	static CApplication App;
	return App; 
}

inline UINT  CApplication::GetClassStyle()		{ return CS_HREDRAW | CS_VREDRAW; }
inline DWORD CApplication::GetWindowStyleEx()	{ return 0; }

/*
inline DWORD CApplication::GetWindowStyle()		{ return WS_DLGFRAME; }
inline int	 CApplication::GetWindowWidth()		{ return 1024; }
inline int	 CApplication::GetWindowHeight()	{ return 768; }
inline int	 CApplication::GetWindowLeft()		{ return ( ::GetSystemMetrics( SM_CXSCREEN ) - GetWindowWidth() ) / 2; }
inline int	 CApplication::GetWindowTop()		{ return ( ::GetSystemMetrics( SM_CYSCREEN ) - GetWindowHeight() ) / 2; }
*/

inline DWORD CApplication::GetWindowStyle()		{ return WS_POPUP; }
inline int	 CApplication::GetWindowWidth()		{ return ::GetSystemMetrics( SM_CXSCREEN ); }
inline int	 CApplication::GetWindowHeight()	{ return ::GetSystemMetrics( SM_CYSCREEN ); }
inline int	 CApplication::GetWindowLeft()		{ return 0; }
inline int	 CApplication::GetWindowTop()		{ return 0; }


LRESULT CALLBACK CApplication::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		// LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CHAR: 
		break;

	case WM_KEYDOWN: 
		switch (wParam)
		{
		case VK_ESCAPE: 
			if (GetApp().m_bAllowExitByEsc) 
				::SendMessage(hWnd, WM_CLOSE, 0, 0); break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		if ( GetApp().m_pD3DDev ) GetApp().OnFrameRender();
		break;

	default:			
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;

}

CApplication::CApplication()
	: m_pszClassName	( L"[wcd]D3Dspace" )
	, m_hInst			( NULL )
	, m_hWnd			( NULL )
	, m_pD3D			( NULL )
	, m_pD3DDev			( NULL )
	, m_dwClearColor	( 0xfffafafa )
	, m_bStop			( false )
	, m_bDevLost		( false )
	, m_bAllowExitByEsc	( true )
{
	EnterFunc();
}

CApplication::~CApplication()
{
	EnterFunc();

	SAFE_RELEASE( m_pD3DDev );
	SAFE_RELEASE( m_pD3D );

	if ( !NoErrors() ) ShowMessages();

	::DestroyWindow( m_hWnd );
	::UnregisterClass( m_pszClassName, m_hInst );
}

void CApplication::Run()
{
	EnterFunc();

	if ( NoErrors() ) 
		m_Layout.CreateObjects();

	if ( !NoErrors() ) return ;

	HACCEL	hAccel	= ::LoadAccelerators( m_hInst, MAKEINTRESOURCE( IDC_R2D2 ) );
	MSG		wMsg;

	m_bStop = false;

	for (;;)
	{
		if (PeekMessage(&wMsg, 0, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == wMsg.message) m_bStop = true;

			if (!TranslateAccelerator(wMsg.hwnd, hAccel, &wMsg))
			{
				TranslateMessage(&wMsg);
				DispatchMessage(&wMsg);
			}
		}
		else
		{
			OnFrameRender();
		}

		if (m_bStop) break;
	}
}

bool CApplication::InitInstance(HINSTANCE hInstance)
{
	EnterFunc();

	m_hInst = hInstance;

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= GetClassStyle();
	wcex.lpfnWndProc	= WndProc;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE( IDI_R2D2 ));
	wcex.hCursor		= LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(16);
	wcex.lpszClassName	= m_pszClassName;

	if (!::RegisterClassEx(&wcex)) 
	{
		AddErrorMessage( L"создания класса главного окна %s", m_pszClassName );
		return false;
	}

	m_hWnd = ::CreateWindowEx(GetWindowStyleEx(), m_pszClassName, NULL
		, GetWindowStyle()
		, GetWindowLeft()
		, GetWindowTop()
		, GetWindowWidth()
		, GetWindowHeight()
		, NULL, NULL, m_hInst, NULL);

	if ( !m_hWnd )
	{
		AddErrorMessage( L"создания главного окна" );
		return false;
	}

	if (!InitD3D())
	{
		return false;
	}

	::ShowWindow( m_hWnd, SW_SHOWNORMAL );
	::UpdateWindow( m_hWnd );

	return true;
}

bool CApplication::InitD3D()
{
	EnterFunc();

	m_pD3D = ::Direct3DCreate9( D3D_SDK_VERSION );
	if ( !m_pD3D )
	{
		AddErrorMessage( L"запроса IDirect3D9" );
		return false;
	}

	HRESULT	hr;
	D3DPRESENT_PARAMETERS d3dpp = {0};

	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat	= D3DFMT_A8R8G8B8;
	d3dpp.BackBufferWidth	= GetWindowWidth();
	d3dpp.BackBufferHeight	= GetWindowHeight();
	d3dpp.BackBufferCount	= 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	AddLogD3DPresentParameters( d3dpp );

	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd
		, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDev );

	if ( FAILED(hr) || !m_pD3DDev ) 
	{
		AddErrorMessage( L"запроса IDirect3DDevice9" );
		return false;
	}

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	return true;
}

void CApplication::OnFrameRender() 
{
	HRESULT hr;

	m_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_dwClearColor, 1.0f, 0 );
	hr = m_pD3DDev->BeginScene();

	if ( SUCCEEDED( hr ) )
	{
		m_SceneManager.RenderScene();
		m_pD3DDev->EndScene();
	}

	hr = m_pD3DDev->Present( NULL, NULL, NULL, NULL );

	if( FAILED(hr) )
	{
		if		( D3DERR_DEVICELOST == hr )	m_bDevLost = true;
		else if ( D3DERR_DRIVERINTERNALERROR == hr ) m_bDevLost = true;
	}
}
