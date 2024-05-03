#pragma once

#include "resource.h"

class CBasixApp: public CWinAppEx
{
public:
	CBasixApp();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

    UINT GetAppLook() const;
    void SetAppLook(UINT id);

private:
    UINT  m_nAppLook;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBasixApp theApp;
