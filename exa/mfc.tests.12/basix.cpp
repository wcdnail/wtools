#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "basix.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "BasixDoc.h"
#include "BasixView.h"
#include "CGridListCtrlExDlg.h"

CBasixApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CBasixApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CBasixApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

CBasixApp::CBasixApp()
    : m_nAppLook(0)
{
	SetAppID(_T("Basix.WCD.1.0"));
}

UINT CBasixApp::GetAppLook() const
{
    return m_nAppLook;
}

void CBasixApp::SetAppLook(UINT id)
{
    m_nAppLook = id;
}

BOOL CBasixApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction();
	AfxInitRichEdit2();
	SetRegistryKey(_T("WCD"));
	LoadStdProfileSettings(4);
	InitContextMenuManager();
	InitShellManager();
	InitKeyboardManager();
	InitTooltipManager();

#if 1
    CGridListCtrlExDlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();
#else
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_basixTYPE, RUNTIME_CLASS(CBasixDoc), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CBasixView));
	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);

    m_nAppLook = GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	CMainFrame* pMainFrame = new CMainFrame();
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}

	m_pMainWnd = pMainFrame;

    CMFCToolBar::m_bExtCharTranslation = TRUE;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

    pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
#endif

	return TRUE;
}

int CBasixApp::ExitInstance()
{
	AfxOleTerm(FALSE);
	return CWinAppEx::ExitInstance();
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CBasixApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CBasixApp::PreLoadState()
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

void CBasixApp::LoadCustomState()
{
}

void CBasixApp::SaveCustomState()
{
}



