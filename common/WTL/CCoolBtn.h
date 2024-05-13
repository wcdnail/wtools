#pragma once
// CoolBtn.h : header file
//

#include <atlstr.h>
#include <atlctrls.h>

/////////////////////////////////////////////////////////////////////////////
// CCoolBtn window


class CCoolBtn : public WTL::CButton
{
// Construction
public:
    CCoolBtn();
    BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
    BOOL SetButtonImage(UINT nResourceId, COLORREF crMask);
    BOOL AddMenuItem(UINT nMenuId,const ATL::CString strMenu, UINT nFlags);
    BOOL EnableMenuItem(UINT nMenuId,const CString strMenu, BOOL nEnable);
    BOOL RemoveMenuItem(UINT nMenuId,const CString strMenu, UINT nMenuFlags);
    void AddImage( int nIndex, CBitmap& bmpEnabled, CBitmap& bmpDisabled );
    void ChangeMenuFlags(UINT nMenuId, UINT nMenuFlags);
    void SetDefaultButton(BOOL bDefault);

// Attributes
protected:
  CMenu        m_menu;
  CBitmap      m_btnImage;
  CImageList   m_IL;
  BOOL         m_bPushed;
  BOOL         m_bMenuPushed;
  BOOL         m_bMenuLoaded;
  BOOL         m_bDefaultBtn;
  BOOL         m_bLoaded;
  BITMAP       m_bm;
  CWnd*        m_pParentWnd;
  COLORREF     m_crMask;
  HBITMAP      m_hbmpDisabled;  


// Operations
public:
protected:
  void DrawArrow(CDC* pDC,CPoint ArrowTip);
  BOOL HitMenuBtn(CPoint point);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCoolBtn)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCoolBtn();

    // Generated message map functions
protected:
    //{{AFX_MSG(CCoolBtn)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSysColorChange();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
