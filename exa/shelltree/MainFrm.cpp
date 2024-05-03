// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Demo.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

#define IDM_FIRST_SHELLMENUID	10000
#define IDM_LAST_SHELLMENUID	(IDM_FIRST_SHELLMENUID+1000)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_TEST_SETITEMASROOT, OnTestSetitemasroot)
	ON_COMMAND(ID_TEST_SETITEMASROOTWITHFILES, OnTestSetitemasrootwithfiles)
	ON_COMMAND(ID_TEST_ADDITEMTOROOT, OnTestAdditemtoroot)
	ON_COMMAND(ID_TEST_ADDITEMTOROOTWITHFILES, OnTestAdditemtorootwithfiles)
	ON_WM_INITMENUPOPUP()
	ON_WM_INITMENU()
	ON_COMMAND(ID_TEST_REFRESHITEM, OnTestRefreshitem)
	ON_COMMAND(ID_TEST_TEXTCALLBACK, OnTestTextcallback)
	ON_COMMAND(ID_TEST_IMAGECALLBACK, OnTestImagecallback)
	ON_COMMAND(ID_TEST_CHILDRENCALLBACK, OnTestChildrencallback)
	ON_UPDATE_COMMAND_UI(ID_TEST_TEXTCALLBACK, OnUpdateTestTextcallback)
	ON_UPDATE_COMMAND_UI(ID_TEST_IMAGECALLBACK, OnUpdateTestImagecallback)
	ON_UPDATE_COMMAND_UI(ID_TEST_CHILDRENCALLBACK, OnUpdateTestChildrencallback)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDM_FIRST_SHELLMENUID, IDM_LAST_SHELLMENUID, OnShellCommand)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(this))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	// populate root
	m_wndView.AddRootFolderContent(NULL);

	// create status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	// avoid repainting when resized by changing the class style
	WNDCLASS wc;
	GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wc);
	cs.lpszClass = AfxRegisterWndClass(0, wc.hCursor, wc.hbrBackground, wc.hIcon);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

// TEST commands

void CMainFrame::OnTestSetitemasroot() 
{
	HTREEITEM hItem = m_wndView.GetSelectedItem();
	if (hItem != NULL)
	{
		CShellPidl pidl = m_wndView.GetItemIDList(hItem);
		SetRedraw(FALSE);
		m_wndView.DeleteAllItems();
		SetRedraw(TRUE);
		m_wndView.AddRootFolderContent(pidl);
	}
}

void CMainFrame::OnTestSetitemasrootwithfiles() 
{
	HTREEITEM hItem = m_wndView.GetSelectedItem();
	if (hItem != NULL)
	{
		CShellPidl pidl = m_wndView.GetItemIDList(hItem);
		SetRedraw(FALSE);
		m_wndView.DeleteAllItems();
		SetRedraw(TRUE);
		m_wndView.AddRootFolderContent(pidl, STCF_INCLUDEALL);
	}
}

void CMainFrame::OnTestAdditemtoroot() 
{
	HTREEITEM hItem = m_wndView.GetSelectedItem();
	if (hItem != NULL)
		m_wndView.AddRootItem(m_wndView.GetItemIDList(hItem), STCF_SHOWFULLPATH);
}

void CMainFrame::OnTestAdditemtorootwithfiles() 
{
	HTREEITEM hItem = m_wndView.GetSelectedItem();
	if (hItem != NULL)
		m_wndView.AddRootItem(m_wndView.GetItemIDList(hItem),
			STCF_SHOWFULLPATH|STCF_INCLUDEALL);
}

void CMainFrame::OnTestRefreshitem() 
{
	HTREEITEM hItem = m_wndView.GetSelectedItem();
	if (hItem != NULL)
		m_wndView.RefreshSubItems(hItem);
}

void CMainFrame::OnTestTextcallback() 
{
	m_wndView.SetCallbackMask(m_wndView.GetCallbackMask() ^ TVIF_TEXT);
}

void CMainFrame::OnTestImagecallback() 
{
	m_wndView.SetCallbackMask(m_wndView.GetCallbackMask()
		^ (TVIF_IMAGE | TVIF_SELECTEDIMAGE));
}

void CMainFrame::OnTestChildrencallback() 
{
	m_wndView.SetCallbackMask(m_wndView.GetCallbackMask() ^ TVIF_CHILDREN);
}

void CMainFrame::OnUpdateTestTextcallback(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndView.GetCallbackMask() & TVIF_TEXT) ? 1 : 0);
}

void CMainFrame::OnUpdateTestImagecallback(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndView.GetCallbackMask() & TVIF_IMAGE) ? 1 : 0);
}

void CMainFrame::OnUpdateTestChildrencallback(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndView.GetCallbackMask() & TVIF_CHILDREN) ? 1 : 0);
}

// menu handling functions

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	UpdateShellMenu(pPopupMenu);

	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CMainFrame::OnShellCommand(UINT nID) 
{
	// shell command
	m_shellMenu.InvokeCommand(nID);
}

void CMainFrame::UpdateShellMenu(CMenu *pPopupMenu)
{
	static HTREEITEM hOldItem = NULL;

	// check if it's the right popup menu
	if (!IsMenuId(pPopupMenu, ID_SHELLMENU))
		return;

	// check the selected tree item
	HTREEITEM hItem = m_wndView.GetSelectedItem();
	if (hOldItem == hItem)
		return;	// same item, no changes

	hOldItem = hItem;

	// delete old menu
	int count = (int)pPopupMenu->GetMenuItemCount();
	while (--count >= 0)
		pPopupMenu->DeleteMenu(count, MF_BYPOSITION);

	if (hItem == NULL || !m_wndView.GetItemContextMenu(hItem, m_shellMenu))
	{
		pPopupMenu->AppendMenu(MF_GRAYED, ID_SHELLMENU, _T("(empty)"));
	}
	else
	{
		m_shellMenu.SetOwner(this);
		m_shellMenu.FillMenu(pPopupMenu, 1, IDM_FIRST_SHELLMENUID,
			IDM_LAST_SHELLMENUID, CMF_EXPLORE|CMF_NODEFAULT);
	}
	
	// update the menu identification
	SetMenuId(pPopupMenu, ID_SHELLMENU);
}

void CMainFrame::OnInitMenu(CMenu* pMenu) 
{
	CFrameWnd::OnInitMenu(pMenu);
	
	CMenu *pPopupMenu = pMenu->GetSubMenu(4);	// "Shell" menu
	SetMenuId(pPopupMenu, ID_SHELLMENU);
}

void CMainFrame::SetMenuId(CMenu *pPopupMenu, DWORD id)
{
	// save id in the first item's data
	MENUITEMINFO mii;
	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;
	mii.dwItemData = id;
	::SetMenuItemInfo(pPopupMenu->GetSafeHmenu(), 0, TRUE, &mii);
}

BOOL CMainFrame::IsMenuId(CMenu *pPopupMenu, DWORD id)
{
	// compare id from the first item's data
	MENUITEMINFO mii;
	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;
	pPopupMenu->GetMenuItemInfo(0, &mii, TRUE);
	return (mii.dwItemData == id);
}
