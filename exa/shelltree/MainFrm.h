// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__D0E7B9AD_20E7_44BF_AA1C_7BF62D44F35C__INCLUDED_)
#define AFX_MAINFRM_H__D0E7B9AD_20E7_44BF_AA1C_7BF62D44F35C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CChildView    m_wndView;
	CShellContextMenu m_shellMenu;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnTestSetitemasroot();
	afx_msg void OnTestSetitemasrootwithfiles();
	afx_msg void OnTestAdditemtoroot();
	afx_msg void OnTestAdditemtorootwithfiles();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnTestRefreshitem();
	afx_msg void OnTestTextcallback();
	afx_msg void OnTestImagecallback();
	afx_msg void OnTestChildrencallback();
	afx_msg void OnUpdateTestTextcallback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTestImagecallback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTestChildrencallback(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnShellCommand(UINT nID);
	DECLARE_MESSAGE_MAP()

private:
	void UpdateShellMenu(CMenu* pPopupMenu);
	static BOOL IsMenuId(CMenu *pPopupMenu, DWORD id);
	static void SetMenuId(CMenu *pPopupMenu, DWORD id);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__D0E7B9AD_20E7_44BF_AA1C_7BF62D44F35C__INCLUDED_)
