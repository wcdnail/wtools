// GTColorDialog.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GTColorDialog.h"
#include "GTColorDialogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogApp

BEGIN_MESSAGE_MAP(CGTColorDialogApp, CWinApp)
	//{{AFX_MSG_MAP(CGTColorDialogApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogApp construction

CGTColorDialogApp::CGTColorDialogApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGTColorDialogApp object

CGTColorDialogApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogApp initialization

BOOL CGTColorDialogApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if 0							// warning C4996: 'CWinApp::Enable3dControls': CWinApp::Enable3dControls is no longer needed. You should remove this call.
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	CGTColorDialogDlg dlg;
	m_pMainWnd = &dlg;
	dlg.SetColor(RGB(0,0,255));
	auto const nResponse = static_cast<int>(dlg.DoModal());
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
