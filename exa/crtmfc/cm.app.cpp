#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "cm.app.h"
#include "mainframe.h"

#include "childframe.h"
#include "ipframe.h"
#include "doc.h"
#include "view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


CApp::CApp()
{
	EnableHtmlHelp();

	m_bHiColorIcons = TRUE;

	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	SetAppID(_T("crtmfc.AppID.NoVersion"));
}

CApp _App;

static const CLSID clsid =
{ 0x3D532F87, 0xED85, 0x47B8, { 0xB1, 0x75, 0xA3, 0x78, 0x78, 0xBF, 0x9F, 0x63 } };

const GUID CDECL _tlid = { 0x3DC5B89B, 0x94AA, 0x4358, { 0xAE, 0xF3, 0xFC, 0x7E, 0x93, 0xA3, 0xA, 0x9 } };
const WORD _wVerMajor = 5;
const WORD _wVerMinor = 0;

BOOL CApp::InitInstance()
{
    {
        char buffer[256];
        errno_t error = ::_gcvt_s(buffer, 3.1415926, 64);
        ATLTRACE("TR: %d %s\n", error, buffer);
    }

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

    if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();

	AfxInitRichEdit2();

	SetRegistryKey(_T("Локальные приложения, созданные с помощью мастера приложений"));
	LoadStdProfileSettings(16);

	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	_App.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_CDocTYPE,
		RUNTIME_CLASS(CDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CMinView));

	if (!pDocTemplate)
		return FALSE;

	pDocTemplate->SetContainerInfo(IDR_CDocTYPE_CNTR_IP);
	pDocTemplate->SetServerInfo(
		IDR_CDocTYPE_SRVR_EMB, IDR_CDocTYPE_SRVR_IP,
		RUNTIME_CLASS(CInPlaceFrame));
	AddDocTemplate(pDocTemplate);

    m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

    COleTemplateServer::RegisterAll();

    CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
    m_pMainWnd->DragAcceptFiles();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
		return TRUE;

	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		UnregisterShellFileTypes();
		m_server.UpdateRegistry(OAT_INPLACE_SERVER, NULL, NULL, FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}
	else
	{
		m_server.UpdateRegistry(OAT_INPLACE_SERVER);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
	}

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

    pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

class CAboutDlg : public CDialogEx
{
public:
    enum { IDD = IDD_ABOUTBOX };

	CAboutDlg()
        : CDialogEx(CAboutDlg::IDD)
    {
        EnableActiveAccessibility();
    }

private:
	virtual void DoDataExchange(CDataExchange* pDX)
    {
        CDialogEx::DoDataExchange(pDX);
    }

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CApp::LoadCustomState()
{
}

void CApp::SaveCustomState()
{
}


