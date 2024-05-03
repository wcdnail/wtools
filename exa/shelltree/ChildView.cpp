// ShellTreeView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Demo.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


#define IDM_FIRST_SHELLMENUID	20000
#define IDM_LAST_SHELLMENUID	(IDM_FIRST_SHELLMENUID+1000)

BEGIN_MESSAGE_MAP(CChildView, CShellTreeCtrl)
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRightClick)
	ON_COMMAND(ID_TESTITEM1, OnTestitem1)
	ON_COMMAND(ID_TESTITEM2, OnTestitem2)
	ON_COMMAND(ID_TESTITEM3, OnTestitem3)
	ON_COMMAND(ID_TESTITEM4, OnTestitem4)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDM_FIRST_SHELLMENUID, IDM_LAST_SHELLMENUID, OnShellCommand)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::Create(CWnd *pParent, UINT nID, DWORD dwStyle)
{
	return CreateEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, _T("ShellTree Control"),
		WS_VISIBLE|WS_CHILD|dwStyle, CRect(0,0,0,0), pParent, nID, NULL);
}

void CChildView::OnRightClick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	CPoint pt(GetMessagePos());
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(pt.x, pt.y));
	
	*pResult = 0;
}

void CChildView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd->GetSafeHwnd() != m_hWnd)
	{
		Default();
		return;
	}

	// get clicked item
	UINT nFlags = 0;
	ScreenToClient(&point);
	HTREEITEM hItem = HitTest(point, &nFlags);

	if (!(nFlags & TVHT_ONITEM))
		return;

	// load menu
	CMenu menu;
	menu.LoadMenu(IDR_TESTMENU);
	CMenu *pPopupMenu = menu.GetSubMenu(0);

	// Ctrl key down, make a submenu
	BOOL bPopup = (GetAsyncKeyState(VK_CONTROL) & 0x8000);

	// search insert position
	UINT nInsertAt = 0;
	CMenu *pMenu = FindMenuItemByText(pPopupMenu, _T("&Shell"), nInsertAt);
	if (pMenu == NULL)
	{
		pMenu = pPopupMenu;
		nInsertAt = 0;
	}
	else if (bPopup)
	{
		// add a separator
		pMenu->InsertMenu(nInsertAt+1, MF_BYPOSITION|MF_SEPARATOR, 0);

		pMenu = pMenu->GetSubMenu(nInsertAt);
		nInsertAt = 0;
	}
	pMenu->DeleteMenu(nInsertAt, MF_BYPOSITION);

	GetItemContextMenu(hItem, m_shellMenu);
	if (!m_shellMenu.FillMenu(pMenu, nInsertAt, IDM_FIRST_SHELLMENUID,
		IDM_LAST_SHELLMENUID, CMF_NODEFAULT|CMF_EXPLORE))
	{
		// no items added
		if (bPopup)
			pMenu->AppendMenu(MF_GRAYED, ID_SHELLMENU, _T("(empty)"));
	}

	// display menu
	m_shellMenu.SetOwner(this);
	ClientToScreen(&point);
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, this);
	m_shellMenu.SetOwner(NULL);
}

void CChildView::OnShellCommand(UINT nID) 
{
	// shell command
	m_shellMenu.InvokeCommand(nID);
}

void CChildView::OnTestitem1() 
{
	// TODO: Add your command handler code here
	AfxMessageBox(_T("Clicked Test Item 1!"));
}

void CChildView::OnTestitem2() 
{
	// TODO: Add your command handler code here
	AfxMessageBox(_T("Clicked Test Item 2!"));
}

void CChildView::OnTestitem3() 
{
	// TODO: Add your command handler code here
	AfxMessageBox(_T("Clicked Test Item 3!"));
}

void CChildView::OnTestitem4() 
{
	// TODO: Add your command handler code here
	AfxMessageBox(_T("Clicked Test Item 4!"));
}

CMenu* CChildView::FindMenuItemByText(CMenu *pMenu, LPCTSTR pszText, UINT& nIndex, BOOL bRecursive)
{
	if (!::IsMenu(pMenu->GetSafeHmenu()))
		return NULL;

	CString text;
	UINT count = pMenu->GetMenuItemCount();
	for (UINT id=0; id < count; id++)
	{
		if (pMenu->GetMenuString(id, text, MF_BYPOSITION) > 0
			&& text == pszText)
		{
			nIndex = id;
			return pMenu;
		}
		// search recursively in sub-menus
		if (bRecursive && pMenu->GetMenuState(id, MF_BYPOSITION) & MF_POPUP)
		{
			CMenu *pSubMenu = FindMenuItemByText(pMenu->GetSubMenu(id),
				pszText, nIndex, bRecursive);
			if (pSubMenu != NULL)
				return pSubMenu;
		}
	}
	return NULL;
}
