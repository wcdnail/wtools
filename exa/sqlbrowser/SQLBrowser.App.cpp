#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SQLBrowser.App.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSqlBrowser, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSqlBrowser::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CSqlBrowser::OnFileNew)
END_MESSAGE_MAP()


CSqlBrowser::CSqlBrowser()
{
	SetAppID(_T("WCD.SQLBrowser.0100"));
}

CSqlBrowser::~CSqlBrowser()
{}

CSqlBrowser theApp;

BOOL CSqlBrowser::InitInstance()
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
	SetRegistryKey(_T("WCD\\SQLBrowser"));
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CMDIFrameWnd* pFrame = new CMainFrame(*this);
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

    HINSTANCE hInst = AfxGetResourceHandle();
	MdiMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_sqlbTYPE));
	MdiAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_sqlbTYPE));

	CMFCToolBar::m_bExtCharTranslation = TRUE;

    pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

int CSqlBrowser::ExitInstance()
{
	if (MdiMenu != NULL)
		FreeResource(MdiMenu);

	if (MdiAccel != NULL)
		FreeResource(MdiAccel);

	AfxOleTerm(FALSE);
	return CWinAppEx::ExitInstance();
}

void CSqlBrowser::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	pFrame->LockWindowUpdate();
	pFrame->CreateNewChild(RUNTIME_CLASS(CChildFrame), IDR_sqlbTYPE, MdiMenu, MdiAccel);
	pFrame->UnlockWindowUpdate();
}

void CSqlBrowser::OnAppAbout()
{}

void CSqlBrowser::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CSqlBrowser::LoadCustomState()
{}

void CSqlBrowser::SaveCustomState()
{}



