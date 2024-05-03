// ShellTreeView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__E8D60B24_817B_4E23_A45C_DBF0AAF85655__INCLUDED_)
#define AFX_CHILDVIEW_H__E8D60B24_817B_4E23_A45C_DBF0AAF85655__INCLUDED_

#include "ShellTreeCtrl.h"
#include "ProgressFX.h"
#include "HourglassFX.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CHourglassFX< CProgressFX<CShellTreeCtrl> >
{
// Construction
public:
	CChildView();
	virtual ~CChildView();

// Attributes
private:
	static CMenu* FindMenuItemByText(CMenu* pMenu, LPCTSTR pszText, UINT &nIndex, BOOL bRecursive = TRUE);
	CShellContextMenu m_shellMenu;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd* pParent, UINT nID = AFX_IDW_PANE_FIRST, DWORD dwStyle = TVS_LINESATROOT | TVS_HASBUTTONS | TVS_HASLINES);

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRightClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTestitem1();
	afx_msg void OnTestitem2();
	afx_msg void OnTestitem3();
	afx_msg void OnTestitem4();
	//}}AFX_MSG
	afx_msg void OnShellCommand(UINT nID);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__E8D60B24_817B_4E23_A45C_DBF0AAF85655__INCLUDED_)
