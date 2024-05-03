#include "stdafx.h"
#include "mfc.tests.app.h"
#include "main.dialog.h"
#include "shell.icons.dlg.h"
#include <crash.report/crash.report.hxx>
#include <crash.report/crash.smtp.h>
#include <boost/exception/all.hpp>

#ifdef _DEBUG
#  define new DEBUG_NEW
#endif

Application gApp;

BEGIN_MESSAGE_MAP(Application, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

Application::Application()
{}

BOOL Application::InitInstance()
{
    AfxOleInit();
    try {
	    INITCOMMONCONTROLSEX InitCtrls;
	    InitCtrls.dwSize = sizeof(InitCtrls);
	    InitCtrls.dwICC = ICC_WIN95_CLASSES;
	    InitCommonControlsEx(&InitCtrls);
	    CWinApp::InitInstance();
	    if (!AfxSocketInit()) {
		    AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		    return FALSE;
	    }
	    AfxEnableControlContainer();
        CAutoPtr<CShellManager> pShellManager(new CShellManager);
	    SetRegistryKey(_T("TTTr"));
        MainDialog dlg;
        m_pMainWnd = &dlg;
        dlg.DoModal();
        m_pMainWnd = nullptr;
    }
    catch (...) {
        Crash::GenericExceptionHandler(::GetLastError());
    }
    AfxOleTerm();
	return FALSE;
}
