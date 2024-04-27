#include "stdafx.h"
#include "wtl.combobox.h"
#include "windows.gdi.rects.h"

CAdvComboBox::CAdvComboBox() 
    : Super()
    , ListBox()
    , EditBox(*this)
    , MyTextColor(0xdfdfdf)
    , MyHotTextColor(0xffffff)
    , MyBackColor(0x202020)
    , MyBackHotColor(0x5c5c5c)
    , MyPen(::CreatePen(PS_SOLID, 1, 0x7f7f7f))
    , MyBackBrush(::CreateSolidBrush(MyBackColor))
{
    MyButtonBackColor[0] = 0x7f7f7f;
    MyButtonBackColor[1] = 0x000000;
}

CAdvComboBox::~CAdvComboBox() 
{}

CAdvComboBox::EditCtrl::EditCtrl(CAdvComboBox& owner) 
    : Super()
    , Owner(owner)
{}

CAdvComboBox::EditCtrl::~EditCtrl() 
{}

BOOL CAdvComboBox::OnNcCreate(LPCREATESTRUCT cs)
{
    cs->style &= ~0xF;

    cs->style |= 0 
              | CBS_DROPDOWNLIST
              | CBS_OWNERDRAWVARIABLE
              | CBS_HASSTRINGS
              ;

    return TRUE;
}

int CAdvComboBox::OnCreate(LPCREATESTRUCT)
{
    int rv = (int)DefWindowProc();

    //HWND edit = GetEditCtrl();

    //if (edit)
    //    EditBox.SubclassWindow(edit);

    //::InvalidateRect(m_hWnd, NULL, TRUE);
    //::UpdateWindow(m_hWnd);

    return rv;
}

HBRUSH CAdvComboBox::OnCtlColorListBox(CDCHandle, HWND hwnd)
{
    if (!ListBox && (hwnd != m_hWnd))
        ListBox.SubclassWindow(hwnd);

    return NULL;
}

HBRUSH CAdvComboBox::OnCtlColorEdit(CDCHandle dc, HWND hwnd)
{
    if (!EditBox && (hwnd != m_hWnd))
    {
        EditBox.SubclassWindow(hwnd);
        EditBox.SetWindowLong(GWL_STYLE, WS_CHILD | WS_VISIBLE);
    }

    dc.SetBkMode(OPAQUE);
    dc.SetBkColor(MyBackColor);
    dc.SetTextColor(MyTextColor);
    
    return MyBackBrush;
}

void CAdvComboBox::ListBoxCtrl::OnNcPaint(CRgnHandle) 
{}

BOOL CAdvComboBox::ListBoxCtrl::OnEraseBkgnd(CDCHandle) 
{
    return TRUE;
}

void CAdvComboBox::ListBoxCtrl::OnPaint(CDCHandle) 
{
    CPaintDC dc(*this);
}

BOOL CAdvComboBox::EditCtrl::OnEraseBkgnd(CDCHandle) 
{
    return TRUE;
}

void CAdvComboBox::OnMeasureItem(int id, LPMEASUREITEMSTRUCT mi)
{
    CImageList icons = GetImageList();

    if (icons)
    {
        CSize iconSize;
        icons.GetIconSize(iconSize);

        mi->itemHeight = iconSize.cy + 2;
    }
}

BOOL CAdvComboBox::OnEraseBkgnd(CDCHandle)
{
    return TRUE;
}

void CAdvComboBox::OnNcPaint(CRgnHandle)
{}

void CAdvComboBox::OnDrawItem(int id, LPDRAWITEMSTRUCT di)
{
    CDCHandle dc(di->hDC);
    CRect rc(di->rcItem);

    dc.FillSolidRect(rc, MyBackColor);
    dc.SetBkMode(TRANSPARENT);

    if (-1 == di->itemID)
        return;

    COLORREF lcol;

    if (di->itemState & (ODS_FOCUS | ODS_SELECTED))
    {
        dc.FillSolidRect(rc, MyBackHotColor);
        lcol = dc.SetTextColor(MyHotTextColor);
    }
    else
    {
        lcol = dc.SetTextColor(MyTextColor);
    }

    CImageList icons = GetImageList();
    TCHAR textBuffer[512] = {0};
    COMBOBOXEXITEM it = {0};

    it.mask = CBEIF_IMAGE | CBEIF_TEXT;
    it.iItem = di->itemID;
    it.pszText = textBuffer;
    it.cchTextMax = _countof(textBuffer);

    if (GetItem(&it))
    {
        bool drawIcon = NULL != icons.m_hImageList;

        CSize iconSize;
        if (drawIcon)
            icons.GetIconSize(iconSize);

        CRect rcText(rc);
        rcText.DeflateRect(2, 2);

        if (drawIcon)
            rcText.left += iconSize.cx + 3;

        dc.DrawText(textBuffer, -1, rcText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        if (drawIcon)
            icons.Draw(dc, it.iImage, rc.left + 2, rc.top + 2, 0);
    }

    dc.SetTextColor(lcol);
}

void CAdvComboBox::EditCtrl::OnPaint(CDCHandle) 
{
    CPaintDC paintDc(*this);

    CRect rc;
    GetClientRect(rc);

    CDC dcMem(::CreateCompatibleDC(paintDc));
    CBitmap bmMem(::CreateCompatibleBitmap(paintDc, rc.Width(), rc.Height()));
    HBITMAP lbm = dcMem.SelectBitmap(bmMem);

    CDCHandle dc(dcMem);
    dc.SelectFont(Owner.GetFont());

    Cf::GradRect(dc, rc, Owner.MyButtonBackColor);
    Cf::FrameRect(dc, rc);

    CRect rcItem(rc);
    rcItem.DeflateRect(3, 3);
    rcItem.top -= 2;
    rcItem.bottom += 1;
    rcItem.right -= 15;

    DRAWITEMSTRUCT di = {0};
    di.hDC = dc;
    di.rcItem = rcItem;
    di.itemID = Owner.GetCurSel();
    Owner.OnDrawItem(-1, &di);

    dc.SelectPen(Owner.MyPen);
    Cf::FrameRect(dc, rcItem);

    CRect rcArrow = rc;
    rcArrow.left = rcItem.right;

    ::DrawTextW(dc, L"▼", 1, &rcArrow, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

    paintDc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcMem, 0, 0, SRCCOPY);
    dc.SelectBitmap(lbm);
}

void CAdvComboBox::EditCtrl::OnKeyUp(UINT code, UINT, UINT flags) 
{
    HWND parent = ::GetParent(m_hWnd);
    HWND root = ::GetParent(parent);

    if (root)
    {
        NMHDR nmh;
        
        nmh.code = MAKELPARAM(WM_KEYUP, code);
        nmh.idFrom = ::GetDlgCtrlID(parent);
        nmh.hwndFrom = parent;

        ::SendMessageW(root, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
    }
}
