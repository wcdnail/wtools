#include "stdafx.h"
#include "wtl.colorizer.h"
#include "wtl.colorizer.inline.h"
#include "wtl.colorizer.control.h"
#include "windows.gdi.rects.h"
#include "dh.tracing.h"
#include "rect.putinto.h"
#include "win-5-6-7.features.h"
#include "cf-resources/resource.h"

namespace Cf
{
    WTL::CImageList Colorizer::SpecGxf;
    Colorizer::FactoryMap Colorizer::Factory;

    Colorizer::~Colorizer() {}

    Colorizer::Colorizer()
        : MyTextColor(0xdfdfdf)
        , MyBackColor(0x202020)
        , MyHotTextColor(0xffffff)
        , MyHotBackColor(0x5c5c5c)
        , MyPen(::CreatePen(PS_SOLID, 1, 0))
        , MyFocusPen(::CreatePen(PS_DOT, 1, 0x2f2f2f))
        , MyThickEdgePen(::CreatePen(PS_SOLID, 2, 0x5f5f5f))
        , MsgHandled(false)
        , MyHwnd(NULL)
        , Controls()
    {
        MyButtonBackColor[0] = 0x3f3f3f;
        MyButtonBackColor[1] = 0x000000;
        MyBackBrush[0] = ::CreateSolidBrush(MyBackColor);
        MyBackBrush[1] = ::CreateSolidBrush(0x505050);
        MyBackBrush[2] = ::CreateSolidBrush(0x070707);
        MyBorderPen[0] = ::CreatePen(PS_SOLID, 1, 0x070707);
        MyBorderPen[1] = ::CreatePen(PS_SOLID, 1, 0x303030);

        PerformInitStatix();
    }

    #pragma region ControlBase

    Colorizer::ControlBase::ControlBase(HWND hwnd, PCTSTR childClass) 
#ifdef _DEBUG
        : PrevClass()
#endif
    {
#ifdef _DEBUG
        TCHAR _class[256] = {0};
        int classLen = ::GetClassName(hwnd, _class, _countof(_class)-1);
        PrevClass = _class;

        TCHAR text[64] = {0};
        int len = ::GetWindowText(hwnd, text, _countof(text)-1);

        TCHAR dispText[64] = {0};
        Dh::MakePrintable(dispText, text, len);

        Dh::ThreadPrintf(_T("Colorize: \t\t++ %p [%s] `%s` ==> `%s`\n"), hwnd, dispText, PrevClass, childClass);
#endif
    }

    Colorizer::ControlBase::~ControlBase() {}

    Colorizer::ControlBase::ImplBase* Colorizer::ControlBase::Impl() { return NULL; }

    void Colorizer::ControlBase::OnDestroy(HWND hwnd, PCTSTR childClass)
    {
#ifdef _DEBUG
        TCHAR text[64] = {0};
        int len = ::GetWindowText(hwnd, text, _countof(text)-1);

        TCHAR dispText[64] = {0};
        Dh::MakePrintable(dispText, text, len);

        Dh::ThreadPrintf(L"Colorize: \t\t-- %p [%s] `%s` <== `%s`\n", hwnd, dispText, PrevClass, childClass);
#endif
    }

    #pragma endregion 

    #pragma region Controls factory

    template <typename T>
    static Colorizer::ControlBase* Colorizer::Creator(HWND hwnd, Colorizer* owner)
    {
        return new T(hwnd, owner);
    }

    template <typename T>
    void Colorizer::InsertToFactory()
    {
        CStringW _class = T::GetWndClassName();
        _class.MakeUpper();

        Factory[_class] = Creator<Control<T> >;

#ifdef _DEBUG
        Dh::ThreadPrintf(L"Colorize: Insert `%s` to factory\n", _class);
#endif
    }

    void Colorizer::PerformInitStatix()
    {
        if (Factory.empty())
        {
            InsertToFactory<WTL::CStatic>();
            InsertToFactory<WTL::CButton>();
            InsertToFactory<WTL::CScrollBar>();
            InsertToFactory<WTL::CComboBox>();
            InsertToFactory<WTL::CEdit>();
            InsertToFactory<WTL::CListBox>();
            InsertToFactory<WTL::CHeaderCtrl>();
          //InsertToFactory<WTL::CLinkCtrl>();
            InsertToFactory<WTL::CListViewCtrl>();
            InsertToFactory<WTL::CTreeViewCtrl>();
            InsertToFactory<WTL::CComboBoxEx>();
            InsertToFactory<WTL::CTabCtrl>();
            InsertToFactory<WTL::CIPAddressCtrl>();
            InsertToFactory<WTL::CPagerCtrl>();
            InsertToFactory<WTL::CProgressBarCtrl>();
            InsertToFactory<WTL::CTrackBarCtrl>();
            InsertToFactory<WTL::CUpDownCtrl>();
            InsertToFactory<WTL::CDateTimePickerCtrl>();
            InsertToFactory<WTL::CMonthCalendarCtrl>();
            InsertToFactory<WTL::CRichEditCtrl>();
        }

        if (!SpecGxf.m_hImageList)
        {
            SpecGxf.Create(IDB_SPECGFX, 9, 1, 0xff00ff);
        }
    }

    #pragma endregion 

    #pragma region Windows messages handler

    BOOL Colorizer::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID/* = 0*/)
    {
        MyHwnd = hWnd;
        bool prevMsgHandled = MsgHandled;
        BOOL rv = _ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
        MsgHandled = prevMsgHandled;
        return rv;
    }

    BOOL Colorizer::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        BOOL bHandled = TRUE;

        switch (dwMsgMapID)
        {
        case 0:
            MSG_WM_CREATE(OnCreate)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
            MSG_WM_CTLCOLOREDIT(OnCtlColorStatic)
          //MSG_WM_NCPAINT(OnNcPaint)
            MESSAGE_HANDLER_EX(WM_DRAWITEM, OnDrawItem)
            MESSAGE_HANDLER_EX(WM_NOTIFY, OnNotify)
            break;
        }

        return FALSE;
    }

    LRESULT Colorizer::OnDrawItem(UINT message, WPARAM wParam, LPARAM lParam)
    {
        LPDRAWITEMSTRUCT di = (LPDRAWITEMSTRUCT)lParam;
        HWND childHwnd = ::GetDlgItem(MyHwnd, di->CtlID);
        LRESULT rv = ::SendMessage(childHwnd, message, wParam, lParam);
        SetMsgHandled(TRUE);
        return rv;
    }
    
    LRESULT Colorizer::OnNotify(UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT rv = 0;
        LPNMHDR header = (LPNMHDR)lParam;

        SetMsgHandled(FALSE);

        switch (header->code)
        {
        case NM_CUSTOMDRAW:
      //case TVN_GETDISPINFOW:
      //case TVN_GETDISPINFOA:
            rv = ::SendMessage(header->hwndFrom, message, wParam, lParam);
            SetMsgHandled(TRUE);
            break;
        }

        return rv;
    }

    int Colorizer::OnCreate(LPCREATESTRUCT)
    {
        return DoInitialization(false);
    }

    BOOL Colorizer::OnInitDialog(HWND, LPARAM)
    {
        return DoInitialization(true);
    }

    #pragma endregion 

    #pragma region Initialization

    int Colorizer::DoInitialization(bool isDialog)
    {
        SetMsgHandled(FALSE);
        SuperclassEachChild(MyHwnd);

#ifdef _DEBUG222
        WTL::CListBox lbTemp(::GetDlgItem(MyHwnd, 1313));
        if (lbTemp.m_hWnd)
        {
            for (int i=0; i<64; i++)
            {
                lbTemp.AddString(_T("Line #0 f##434#$@ck!"));
                lbTemp.AddString(_T("Line #1"));
                lbTemp.AddString(_T("Line #2 do something $$% sdkjfklsdkjafks;fakjds;afkj;ldfsakjdsk"));
                lbTemp.AddString(_T("Line #3"));
                lbTemp.AddString(_T("Line #4 with thiz"));
            }
        }
#endif

        return isDialog ? TRUE : 0;
    }

    void Colorizer::OnDestroy()
    {
#ifdef _DEBUG
        WCHAR text[64] = {0};
        ::GetWindowTextW(MyHwnd, text, _countof(text)-1);
        Dh::ThreadPrintf(L"Colorize: - %p %s...\n", MyHwnd, text);
#endif

        SetMsgHandled(FALSE);
    }

    void Colorizer::SuperclassEachChild(HWND hwnd)
    {
#ifdef _DEBUG
        WCHAR text[64] = {0};
        ::GetWindowTextW(hwnd, text, _countof(text)-1);
        Dh::ThreadPrintf(L"Colorize: + %p %s...\n", hwnd, text);
#endif
        ::EnumChildWindows(hwnd, (WNDENUMPROC)InitChild, (LPARAM)this);
    }

    BOOL CALLBACK Colorizer::InitChild(HWND hwnd, Colorizer* self)
    {
        ControlMap::const_iterator ci = self->Controls.find(hwnd);
        bool already = ci != self->Controls.end();

        CStringW _class;
        int classLen = ::GetClassNameW(hwnd, _class.GetBufferSetLength(4096), 4096);
        _class.ReleaseBufferSetLength(classLen);
        _class.MakeUpper();

        FactoryMap::const_iterator fact = Factory.find(_class);
        bool exist = fact != Factory.end();

#ifdef _DEBUG
        if (!exist)
            Dh::ThreadPrintf(L"Colorize: \t\t!! %p `%s`\n", hwnd, _class);
#endif

        if (exist && !already)
        {
            ::ShowWindow(hwnd, SW_HIDE);

            ControlCreator create = fact->second;
            ControlBasePtr child(create(hwnd, self));

            self->Controls[hwnd] = child;

            ::ShowWindow(hwnd, SW_SHOW);
        }

        return TRUE;
    }

    #pragma endregion 

    #pragma region Common stuff

    void Colorizer::SetTextColor(CDCHandle dc) const
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);
        dc.SetBkColor(MyBackColor);
    }

    template <typename T>
    void Colorizer::OnEraseBackground(Control<T>& child, CDCHandle dc) const
    {
        CRect rc;
        child.GetClientRect(rc);
        dc.FillSolidRect(rc, MyBackColor);
        SetTextColor(dc);
    }

    BOOL Colorizer::OnEraseBkgnd(CDCHandle dc) const
    {
        CRect rc;
        ::GetClientRect(MyHwnd, rc);
        dc.FillSolidRect(rc, MyBackColor);
        SetTextColor(dc);
        return TRUE;
    }

    void Colorizer::OnNcPaint(CRgnHandle rgn) const
    {
        if (Win567().IsCompositionEnabled())
        {
            SetMsgHandled(FALSE);
            return ;
        }
        
        NcPainContext nc(MyHwnd, rgn);

        CRect rcClient;
        ::GetClientRect(MyHwnd, rcClient);

        WTL::CRgn clientRgn;
        clientRgn.CreateRectRgnIndirect(rcClient);

        nc.Dc.SelectClipRgn(clientRgn);
        nc.Dc.FillSolidRect(nc.Rect, 0xff00ff);

        unsigned style = 0, estyle = 0;
        unsigned bt = GetBorderType(MyHwnd, style, estyle);
        DrawControlBorder(nc.Dc, nc.Rect, bt);
    }

    HBRUSH Colorizer::OnCtlColorStatic(CDCHandle dc, HWND)
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);
        dc.SetBkColor(MyBackColor);
        return MyBackBrush[0];
    }

    #pragma endregion 

    #pragma region Border drawing

    Colorizer::BorderFlags Colorizer::GetBorderType(HWND hwnd, unsigned& style, unsigned& estyle)
    {
        style = ::GetWindowLong(hwnd, GWL_STYLE);
        estyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

        unsigned result = 0;

        if (style & WS_BORDER)
            result = BorderFrame;

        else if (estyle & WS_EX_CLIENTEDGE)
            result = BorderClientEdge;

        else if (estyle & WS_EX_STATICEDGE) 
            result = BorderStaticEdge;
                
        return (BorderFlags)result;
    }
    
    void Colorizer::DrawControlBorder(CDCHandle dc, CRect const& rcPaint, unsigned border) const
    {
        CRect rc = rcPaint;

        for (int i=0; i<2; i++)
        {
            HPEN lp = dc.SelectPen(MyBorderPen[0]);

            dc.MoveTo(rc.left, rc.bottom, NULL);
            dc.LineTo(rc.left, rc.top);
            dc.LineTo(rc.right, rc.top);

            if (BorderFrame != border)
                dc.SelectPen(MyBorderPen[1]);

            dc.LineTo(rc.right, rc.bottom);
            dc.LineTo(rc.left, rc.bottom);

            dc.SelectPen(lp);

            if (BorderClientEdge != border)
                break;

            rc.DeflateRect(1, 1);
        }
    }

    #pragma endregion

    #pragma region Text drawing

    template <typename T>
    void Colorizer::PutText(Control<T>& child, CDCHandle dc, CRect const& rc, CString const& text, bool deflate, int dx, int dy)
    {
        if (!text.IsEmpty())
        {
            UINT format = Details<T>::GetDrawTextFormat(child);

            CRect rcText(rc);

            if (deflate)
                rcText.DeflateRect(dx, dy);

            if (DT_SINGLELINE & format)
                dc.DrawText(text, text.GetLength(), rcText, format);

            else
            {
                CRect rcCalc;
                dc.DrawText(text, text.GetLength(), rcCalc, DT_CALCRECT);
                Rc::PutInto(rcText, rcCalc, format & 0xf);

                dc.DrawText(text, text.GetLength(), rcCalc, format);
            }
        }
    }

    #pragma endregion

    #pragma region GroupBox drawing

    void Colorizer::DrawGroupBox(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text) const 
    {
        CSize szText;
        if (!text.IsEmpty())
            dc.GetTextExtent(text, text.GetLength(), &szText);

        LONG c2y = szText.cy / 2;

        HPEN lp = dc.SelectPen(MyBorderPen[0]);
        dc.SelectBrush(MyBackBrush[0]);

        CRect rcFrame = rc;
        rcFrame.top += c2y;
        dc.RoundRect(rcFrame, CPoint(6, 6));
        
        dc.SelectPen(MyBorderPen[1]);
        rcFrame.DeflateRect(1, 1);
        dc.RoundRect(rcFrame, CPoint(6, 6));

        if (!text.IsEmpty())
        {
            CRect rcText = rc;

            unsigned place = child.GetWindowLong(GWL_STYLE) & 0xf00;

            if (BS_CENTER == place)
                rcText.left = (rcText.Width() - szText.cx) / 2;

            else if (BS_RIGHT == place)
                rcText.left = rcText.right - szText.cx - 8;

            else
                rcText.left += 8;

            rcText.right = rcText.left + szText.cx;
            rcText.bottom = rcText.top + szText.cy;

            int m = dc.SetBkMode(OPAQUE);
            dc.DrawText(text, text.GetLength(), rcText, DT_LEFT);
            dc.SetBkMode(m);
        }

        dc.SelectPen(lp);
    }

    #pragma endregion 

    #pragma region Button drawing

    void Colorizer::DrawButton(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text, bool flat, bool _default) const 
    {
        unsigned state = child.GetState();

        HPEN lp = dc.SelectPen(MyPen);

        if (!flat)
            Cf::GradRect(dc, rc, MyButtonBackColor);
        else
            dc.FillSolidRect(rc, MyBackColor);

        PutText<WTL::CButton>(child, dc, rc, text, true, 3, 3);

        Cf::FrameRect(dc, rc);

        if (state & BST_PUSHED)
        {
            dc.InvertRect(rc);
            // TODO: Make some noise here...
        }

        if (state & BST_FOCUS)
        {
            // TODO: Implement focus marker
            CRect rcFocus(rc);
            rcFocus.DeflateRect(2, 2);

            dc.SelectPen(MyFocusPen);
            Cf::FrameRect(dc, rcFocus);
        }

        dc.SelectPen(lp);
    }

    #pragma endregion

    #pragma region CheckBox

    void Colorizer::DrawCheckBox(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text) const 
    {
        dc.FillSolidRect(rc, MyBackColor);

        CRect rcBox(0, 0, rc.Height(), rc.Height());
        CRect rcText = rc;

        unsigned style = child.GetWindowLong(GWL_STYLE);
        unsigned estyle = child.GetWindowLong(GWL_EXSTYLE);

        if ((estyle & WS_EX_RIGHT) || (style & BS_LEFTTEXT))
        {
            rcBox.MoveToXY(rc.right - rcBox.Width(), rc.top);
            rcText.right -= rcBox.Width() + 2;
        }
        else
        {
            rcBox.MoveToXY(rc.left, rc.top);
            rcText.left += rcBox.Width() + 2;
        }

        HPEN lp = dc.SelectPen(MyBorderPen[0]);
        dc.SelectBrush(MyBackBrush[0]);
        dc.RoundRect(rcBox, CPoint(6, 6));
        dc.SelectPen(MyBorderPen[1]);
        rcBox.DeflateRect(1, 1);
        dc.RoundRect(rcBox, CPoint(6, 6));

        unsigned state = child.GetState();

        if (BST_CHECKED & state)
        {
            rcBox.DeflateRect(3, 3);

            dc.SelectPen(MyThickEdgePen);
            dc.MoveTo(rcBox.left, rcBox.bottom);
            dc.LineTo(rcBox.right, rcBox.top);
            dc.MoveTo(rcBox.right, rcBox.bottom);
            dc.LineTo(rcBox.left, rcBox.top);
        }
        else if (BST_INDETERMINATE & state)
        {
            rcBox.DeflateRect(2, 2);

            dc.SelectBrush(MyBackBrush[1]);
            dc.RoundRect(rcBox, CPoint(6, 6));
        }

        if (BST_FOCUS & state)
        {
            // TODO: Implement focus marker
            CRect rcFocus(rcText);
            rcFocus.DeflateRect(-2, 1, 1, 2);

            dc.SelectPen(MyFocusPen);
            Cf::FrameRect(dc, rcFocus);
        }

        PutText<WTL::CButton>(child, dc, rcText, text);

        dc.SelectPen(lp);
    }

    #pragma endregion 

    #pragma region RadioButton

    void Colorizer::DrawRadioButton(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text) const 
    {
        dc.FillSolidRect(rc, MyBackColor);

        CRect rcBox(0, 0, rc.Height(), rc.Height());
        CRect rcText = rc;

        unsigned style = child.GetWindowLong(GWL_STYLE);
        unsigned estyle = child.GetWindowLong(GWL_EXSTYLE);

        if ((estyle & WS_EX_RIGHT) || (style & BS_LEFTTEXT))
        {
            rcBox.MoveToXY(rc.right - rcBox.Width(), rc.top);
            rcText.right -= rcBox.Width() + 2;
        }
        else
        {
            rcBox.MoveToXY(rc.left, rc.top);
            rcText.left += rcBox.Width() + 2;
        }

        HPEN lp = dc.SelectPen(MyBorderPen[0]);
        dc.SelectBrush(MyBackBrush[0]);
        dc.Ellipse(rcBox);
        dc.SelectPen(MyBorderPen[1]);
        rcBox.DeflateRect(1, 1);
        dc.Ellipse(rcBox);

        unsigned state = child.GetState();

        if (BST_CHECKED & state)
        {
            rcBox.DeflateRect(2, 2);

            dc.SelectPen(MyBorderPen[0]);
            dc.SelectBrush(MyBackBrush[2]);
            dc.Ellipse(rcBox);
        }

        PutText<WTL::CButton>(child, dc, rcText, text);

        if (BST_FOCUS & state)
        {
            // TODO: Implement focus marker
            CRect rcFocus(rcText);
            rcFocus.DeflateRect(-2, 1, 1, 2);

            dc.SelectPen(MyFocusPen);
            Cf::FrameRect(dc, rcFocus);
        }

        dc.SelectPen(lp);
    }

    #pragma endregion

    #pragma region Combo & list boxes

    template <typename T>
    void Colorizer::DrawItem(Control<T>& child, int id, LPDRAWITEMSTRUCT di, bool eraseback /*= true*/) const
    {
        CDCHandle dc(di->hDC);
        CRect rc(di->rcItem);

        if (eraseback)
            dc.FillSolidRect(rc, MyBackColor);

        dc.SetBkMode(TRANSPARENT);

        if (-1 == di->itemID)
            return;

        if (di->itemState & ODS_SELECTED)
        {
            dc.FillSolidRect(rc, MyHotBackColor);
            dc.SetTextColor(MyHotTextColor);
        }
        else
        {
            dc.SetTextColor(MyTextColor);
        }

        HIMAGELIST ilist = child.GetImageList();
        int iindex = child.GetImageIndex(di->itemID);
        CString text = child.GetItemText(di->itemID);

        bool drawIcon = (-1 != iindex) && (NULL != ilist);

        int iconx = 0, icony = 0;
        if (drawIcon)
            ::ImageList_GetIconSize(ilist, &iconx, &icony);

        if (!text.IsEmpty())
        {
            CRect rcText(rc);
            rcText.DeflateRect(2, 2);

            if (drawIcon)
                rcText.left += iconx + 3;

            dc.DrawText(text, text.GetLength(), rcText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }

        if (drawIcon)
            ::ImageList_Draw(ilist, iindex, dc, rc.left + 2, rc.top + 2, 0);
    }

    template <typename T>
    void Colorizer::DrawComboFace(Control<T>& child, CDCHandle dc, CRect const& rc) const 
    {
        Cf::GradRect(dc, rc, MyButtonBackColor);
        Cf::FrameRect(dc, rc);

        int by = rc.Height();
      //bool framed = Details<T>::DropdownList != Details<T>::GetAppearType(child);

        CRect rcItem(rc);
        rcItem.DeflateRect(3, 3);
        rcItem.top -= 1;
        rcItem.bottom += 1;
        rcItem.right -= (by - 2);

        DRAWITEMSTRUCT di = {0};
        di.hDC = dc;
        di.rcItem = rcItem;
        di.itemID = child.GetCurSel();
        DrawItem<T>(child, di.itemID, &di, /*framed*/ false);

      //if (framed)
      //{
      //    dc.SelectPen(MyPen);
      //    Cf::FrameRect(dc, rcItem);
      //}

        CRect rcBox(0, 0, 9, 9);
        rcBox.MoveToXY(rc.right - by + 6, rc.top + 6);
        SpecGxf.DrawEx(SpecgDown, dc, rcBox, CLR_NONE, MyTextColor, ILD_TRANSPARENT);
    }

    #pragma endregion
}
