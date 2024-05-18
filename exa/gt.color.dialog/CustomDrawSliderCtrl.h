#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawSliderCtrl window

class CCustomDrawSliderCtrl : public CSliderCtrl
{
// Construction
public:
    CCustomDrawSliderCtrl();                // default constructor
    CCustomDrawSliderCtrl( COLORREF cr );   // constructs the control and sets its primary color


// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCustomDrawSliderCtrl)
    //}}AFX_VIRTUAL

// Implementation
public:
    COLORREF GetShadowColor() const;
    COLORREF GetHiliteColor() const;
    COLORREF GetPrimaryColor() const;
    void SetPrimaryColor( COLORREF cr );
    virtual ~CCustomDrawSliderCtrl();

    // Generated message map functions
protected:

    COLORREF m_crPrimary;
    COLORREF m_crShadow;
    COLORREF m_crHilite;
    COLORREF m_crMidShadow;
    COLORREF m_crDarkerShadow;

    CBrush m_normalBrush;
    CBrush m_focusBrush;

    //{{AFX_MSG(CCustomDrawSliderCtrl)
    afx_msg void OnReflectedCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

