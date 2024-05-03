#pragma once

#include "resource.h"

class CMultiFilerApp : public CWinApp
{
public:
	CMultiFilerApp();

	//{{AFX_VIRTUAL(CMultiFilerApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMultiFilerApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
