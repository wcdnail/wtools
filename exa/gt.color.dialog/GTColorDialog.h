// GTColorDialog.h : main header file for the GTCOLORDIALOG application
//

#if !defined(AFX_GTCOLORDIALOG_H__F818AFC7_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
#define AFX_GTCOLORDIALOG_H__F818AFC7_4DE0_11D6_A56D_525400EA266C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogApp:
// See GTColorDialog.cpp for the implementation of this class
//

class CGTColorDialogApp : public CWinApp
{
public:
	CGTColorDialogApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGTColorDialogApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGTColorDialogApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTCOLORDIALOG_H__F818AFC7_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
