#pragma once

#include "wcdafx.api.h"
#include <atltypes.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlgdi.h>

class CAdvComboBox: public CWindowImpl<CAdvComboBox, WTL::CComboBoxEx>
{
private:
    typedef CWindowImpl<CAdvComboBox, WTL::CComboBoxEx> Super;

public:
	WCDAFX_API CAdvComboBox();
	WCDAFX_API ~CAdvComboBox();
	
private:
    class ListBoxCtrl: public CWindowImpl<ListBoxCtrl, WTL::CListBox>
    {
    public:
        typedef CWindowImpl<ListBoxCtrl, WTL::CListBox> Super;
    
        ListBoxCtrl() {}
        ~ListBoxCtrl() {}

    private:
        void OnNcPaint(CRgnHandle);
        void OnPaint(CDCHandle); 
        BOOL OnEraseBkgnd(CDCHandle);

        DECLARE_WND_SUPERCLASS(_T("CAdvComboBox::ListBoxCtrl"), Super::GetWndClassName())

        BEGIN_MSG_MAP_EX(ListBoxCtrl)
            MSG_WM_NCPAINT(OnNcPaint)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
        END_MSG_MAP()
    };

    class EditCtrl: public CWindowImpl<EditCtrl, WTL::CListBox>
    {
    public:
        typedef CWindowImpl<EditCtrl, WTL::CListBox> Super;

        EditCtrl(CAdvComboBox& owner);
        ~EditCtrl();

    private:
        CAdvComboBox& Owner;

        void OnPaint(CDCHandle);
        BOOL OnEraseBkgnd(CDCHandle);
        void OnKeyUp(UINT code, UINT, UINT flags);

        DECLARE_WND_SUPERCLASS(_T("CAdvComboBox::EditCtrl"), Super::GetWndClassName())

        BEGIN_MSG_MAP_EX(EditCtrl)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_KEYUP(OnKeyUp)
        END_MSG_MAP()
    };

    friend class Super;
    friend class EditCtrl;

    ListBoxCtrl ListBox;
    EditCtrl EditBox;
    COLORREF MyTextColor;
    COLORREF MyHotTextColor;
    COLORREF MyBackColor;
    COLORREF MyBackHotColor;
    CPen MyPen;
    CBrush MyBackBrush;
    COLORREF MyButtonBackColor[2];

    BOOL OnNcCreate(LPCREATESTRUCT cs);
    int OnCreate(LPCREATESTRUCT);
    HBRUSH OnCtlColorListBox(CDCHandle dc, HWND hwnd);
    HBRUSH OnCtlColorEdit(CDCHandle dc, HWND hwnd);
    void OnDrawItem(int id, LPDRAWITEMSTRUCT di);
    void OnMeasureItem(int id, LPMEASUREITEMSTRUCT mi);
    BOOL OnEraseBkgnd(CDCHandle);
    void OnNcPaint(CRgnHandle);

    DECLARE_WND_SUPERCLASS(_T("CAdvComboBox"), Super::GetWndClassName())

    BEGIN_MSG_MAP_EX(CAdvComboBox)
        MSG_WM_NCCREATE(OnNcCreate)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
        MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_MEASUREITEM(OnMeasureItem)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_NCPAINT(OnNcPaint)
    END_MSG_MAP()
};
