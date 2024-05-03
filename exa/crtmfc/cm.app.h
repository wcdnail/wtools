#pragma once

#include "resource.h"

class CMainFrame;
class CPropertiesWnd;
class CFileView;
class CClassView;

class CApp: public CWinAppEx
{
public:
	CApp();

private:
    friend class CMainFrame;
    friend class CPropertiesWnd;
    friend class CFileView;
    friend class CClassView;

    COleTemplateServer m_server;
    UINT  m_nAppLook;
    BOOL  m_bHiColorIcons;

	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};

extern CApp _App;
