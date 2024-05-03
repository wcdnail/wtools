#pragma once

#include "resource.h"
#include "ErrorReport.h"
#include "SceneManagment.h"
#include "MainLayout.h"

class CApplication
{
public:
	CApplication();
	~CApplication();

	bool			InitInstance( HINSTANCE );
	void			Run			();
	virtual bool	InitD3D		();
	HINSTANCE		ModuleHandle() const { return m_hInst; }
	HWND			WindowHandle() const { return m_hWnd; }

	IDirect3DDevice9 *GetD3DDevice() const { return m_pD3DDev; }
	CSceneManager&	GetManager() { return m_SceneManager; }

private:
	void			OnFrameRender();
	UINT			GetClassStyle();
	DWORD			GetWindowStyle();
	DWORD			GetWindowStyleEx();
	int				GetWindowWidth();
	int				GetWindowHeight();
	int				GetWindowLeft();
	int				GetWindowTop();

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	const TCHAR *		m_pszClassName;
	HINSTANCE			m_hInst;
	HWND				m_hWnd;

protected:

	IDirect3D9 *		m_pD3D;
	IDirect3DDevice9 *	m_pD3DDev;
	D3DCOLOR			m_dwClearColor;

	bool				m_bStop;
	bool				m_bDevLost;
	bool				m_bAllowExitByEsc;

	CMainLayout			m_Layout;
	CSceneManager		m_SceneManager;
};

extern CApplication& GetApp();

