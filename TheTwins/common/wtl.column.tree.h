/*****************************************************************************
* CColumnTreeCtrl
* Version: 1.1 
* Date: February 18, 2008
* Author: Oleg A. Krivtsov
* E-mail: olegkrivtsov@mail.ru
* Based on ideas implemented in Michal Mecinski's CColumnTreeCtrl class 
* (see copyright note below).
*
*****************************************************************************/

/*********************************************************
* Multi-Column Tree View
* Version: 1.1
* Date: October 22, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* You may freely use and modify this code, but don't remove
* this copyright note.
*
* There is no warranty of any kind, express or implied, for this class.
* The author does not take the responsibility for any damage
* resulting from the use of it.
*
* Let me know if you find this code useful, and
* send me any modifications and bug reports.
*
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

/*********************************************************
* Adopted to WTL by Michael Nikonov
*********************************************************/

#pragma once

#if 0
#include <atltypes.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlcrack.h>

#define _OWNER_DRAWN_TREE

typedef struct _CTVHITTESTINFO 
{ 
  POINT pt; 
  UINT flags; 
  HTREEITEM hItem; 
  int iSubItem;
} CTVHITTESTINFO;

class CColumnTreeCtrl;

class CCustomTreeChildCtrl: CWindowImpl<CCustomTreeChildCtrl, WTL::CTreeViewCtrl>
{
private:
    typedef CWindowImpl<CCustomTreeChildCtrl, WTL::CTreeViewCtrl> Super;

public:
	CCustomTreeChildCtrl();
	~CCustomTreeChildCtrl();
	
	bool GetBkImage(LVBKIMAGE* plvbkImage) const;
	bool SetBkImage(LVBKIMAGE* plvbkImage);

    using Super::Create;
    using Super::SetImageList;
    using Super::InsertItem;
    using Super::SetItemText;
    using Super::SetItemState;
    using Super::Expand;
    using Super::operator HWND;

    WTL::CTreeViewCtrl& AsTreeViewCtrl() { return *((WTL::CTreeViewCtrl*)this); }
    WTL::CTreeViewCtrl const& AsTreeViewCtrl() const { return *((WTL::CTreeViewCtrl*)this); }

private:
    friend class Super;
    friend class CColumnTreeCtrl;

	int m_nFirstColumnWidth;    // the width of the first column 
	int m_nOffsetX;      	    // offset of this window inside the parent 
	LVBKIMAGE m_bkImage;	    // information about background image
	WTL::CImageList m_imgBtns;	// tree buttons images (IDB_TREEBTNS)

	BOOL CheckHit(CPoint point);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    void OnPaint(CDCHandle);
    void OnTimer(UINT_PTR nIDEvent);
    BOOL OnEraseBkgnd(CDCHandle);
    BOOL OnToolTipNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

#ifdef _OWNER_DRAWN_TREE
    LRESULT CustomDrawNotify(LPNMTVCUSTOMDRAW lpnm);
    LRESULT OwnerDraw(CDCHandle dc);
    int OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
#endif

    DECLARE_WND_SUPERCLASS(_T("CCustomTreeChildCtrl"), Super::GetWndClassName())

    BEGIN_MSG_MAP_EX(CCustomTreeChildCtrl)
	    MSG_WM_KEYDOWN(OnKeyDown)
	    MSG_WM_MOUSEMOVE(OnMouseMove)
	    MSG_WM_LBUTTONDOWN(OnLButtonDown)
	    MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	    MSG_WM_PAINT(OnPaint)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
	    MSG_WM_TIMER(OnTimer)
        //ON_NOTIFY(TTN_NEEDTEXT, OnToolTipNeedText)
        //ON_WM_STYLECHANGED()
    END_MSG_MAP()
};

class CColumnTreeCtrl: CWindowImpl<CColumnTreeCtrl, WTL::CStatic>
{
private:
    typedef CWindowImpl<CColumnTreeCtrl, WTL::CStatic> Super;

public:
	CColumnTreeCtrl();
	~CColumnTreeCtrl();

    using Super::Create;
    using Super::operator HWND;
	
	CCustomTreeChildCtrl& GetTreeCtrl() { return m_Tree; }
	CHeaderCtrl& GetHeaderCtrl() { return m_Header; }

	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = 0, int nWidth = -1, int nSubItem = -1);
	BOOL DeleteColumn(int nCol);
	void SetFirstColumnMinWidth(UINT uMinWidth);
	CString GetItemText(HTREEITEM hItem, int nSubItem);
	void SetItemText(HTREEITEM hItem, int nSubItem, LPCTSTR lpszText);

	HTREEITEM HitTest(CPoint pt, UINT* pFlags = NULL) const;
	HTREEITEM HitTest(CTVHITTESTINFO* pHitTestInfo) const;
	
private:
    friend class Super;

	enum ChildrenIDs 
    {
        HeaderID = 1, 
        TreeID = 2, 
        HScrollID = 3, 
        Header2ID = 4,
        MAX_COLUMN_COUNT = 16
    };
	
	CCustomTreeChildCtrl m_Tree;
	CScrollBar m_horScroll;
	CHeaderCtrl m_Header;
	CHeaderCtrl m_Header2;
	int m_cyHeader;
	int m_cxTotal;
	int m_xPos;
	int m_xOffset;
	int m_uMinFirstColWidth;
	BOOL m_bHeaderChangesBlocked;
	int m_arrColWidths[MAX_COLUMN_COUNT];
	DWORD m_arrColFormats[MAX_COLUMN_COUNT];
	
	void Initialize();
	void UpdateColumns();
	void RepositionControls();

	void OnDraw(CDCHandle) {}
	int OnCreate(LPCREATESTRUCT);
	void OnPaint(CDCHandle);
	void OnSize(UINT type, CSize sz);
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);
	void OnHeaderItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	void OnCancelMode();
	void OnEnable(BOOL bEnable);
	void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	LRESULT OnNotify(UINT, WPARAM wParam, LPARAM lParam);
	BOOL OnEraseBkgnd(CDCHandle);

    DECLARE_WND_SUPERCLASS(_T("CColumnTreeCtrl"), Super::GetWndClassName())

    BEGIN_MSG_MAP_EX(CColumnTreeCtrl)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_SIZE(OnSize)
        MSG_WM_HSCROLL(OnHScroll)
        MSG_WM_SETTINGCHANGE(OnSettingChange)
        MSG_WM_ENABLE(OnEnable)
        MESSAGE_HANDLER_EX(WM_NOTIFY, OnNotify)
        //REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
};
#endif
