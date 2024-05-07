#include "stdafx.h"
#include "wtl.colorizer.h"
#include "wtl.colorizer.control.h"
#include "wtl.colorizer.control.specific.h"
#include "wtl.colorizer.helpers.h"
#include "cf-resources/resource.h"
#include "win-5-6-7.features.h"
#include "windows.gdi.rects.h"
#include "dh.tracing.defs.h"
#include "rect.putinto.h"

#ifdef _MSC_VER
#  pragma warning(disable: 4191) // 4191: 'reinterpret_cast': unsafe conversion from 'BOOL (__cdecl *)(HWND,CF::Colorizer *)' to 'WNDENUMPROC'
                                 //        Making a function call using the resulting pointer may cause your program to fail
#endif

namespace CF::Colorized
{
    using ControlCreator = ControlPtr (*)(HWND, Colorizer&);
    using     FactoryMap = std::unordered_map<CStringW, ControlCreator>;

    Colorizer::~Colorizer()
    {
    }

    Colorizer::Colorizer()
        :    ControlBase()
        ,       Controls()
        ,    MyTextColor(0xdfdfdf)
        ,    MyBackColor(0x202020)
        , MyHotTextColor(0xffffff)
        , MyHotBackColor(0x5c5c5c)
        ,          MyPen(::CreatePen(PS_SOLID, 1, 0))
        ,     MyFocusPen(::CreatePen(PS_DOT, 1, 0x2f2f2f))
        , MyThickEdgePen(::CreatePen(PS_SOLID, 2, 0x5f5f5f))
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

    IMPL_MSG_MAP_DISP_INH(Colorizer,
                          ProcessColorizerMessage,
                          OnColorizerMessage,
                          ControlBase::OnWindowMessage
            );

    IMPL_MSG_MAP_BEG(Colorizer, OnColorizerMessage)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_ERASEBKGND(OnEraseMyBkgnd)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
        MSG_WM_CTLCOLOREDIT(OnCtlColorStatic)
        MSG_WM_CTLCOLORBTN(OnCtlColorBtn)
        //MSG_WM_CTLCOLORLISTBOX(OnCtlColorStatic)
        //MSG_WM_CTLCOLORSCROLLBAR(OnCtlColorStatic)
        //MSG_WM_NCPAINT(OnNcPaint)
        MESSAGE_HANDLER_EX(WM_DRAWITEM, OnDrawItem)
        MESSAGE_HANDLER_EX(WM_NOTIFY, OnNotify)
    END_MSG_MAP()

#pragma region Controls factory

    WTL::CImageList& SpecGxf()
    {
        static WTL::CImageList imList;
        return imList;
    }

    FactoryMap& CtrlFactory()
    {
        static FactoryMap facMap;
        return facMap;
    }

    template <typename T>
    static ControlPtr Colorizer_Creator(HWND hwnd, Colorizer& owner)
    {
        return std::make_unique<T>(hwnd, owner);
    }

    template <typename T>
    static void Colorizer_FactoryAdd()
    {
        CStringW _class = T::GetWndClassName();
        _class.MakeUpper();
        CtrlFactory()[_class] = &Colorizer_Creator<Control<T>>;
        DBGTPrint(L"  ClrzrFactory| << '%s'\n", _class.GetString());
    }

    void Colorizer::PerformInitStatix()
    {
        if (CtrlFactory().empty()) {
            Colorizer_FactoryAdd<ZStatic>();
            Colorizer_FactoryAdd<ZButton>();
            Colorizer_FactoryAdd<ZScrollBar>();
            Colorizer_FactoryAdd<ZComboBox>();
            Colorizer_FactoryAdd<ZEdit>();
            Colorizer_FactoryAdd<ZListBox>();
            Colorizer_FactoryAdd<ZHeaderCtrl>();
          //Colorizer_FactoryAdd<ZLinkCtrl>();
            Colorizer_FactoryAdd<ZListViewCtrl>();
            Colorizer_FactoryAdd<ZTreeViewCtrl>();
            Colorizer_FactoryAdd<ZComboBoxEx>();
            Colorizer_FactoryAdd<ZTabCtrl>();
            Colorizer_FactoryAdd<ZIPAddressCtrl>();
            Colorizer_FactoryAdd<ZPagerCtrl>();
            Colorizer_FactoryAdd<ZProgressBarCtrl>();
            Colorizer_FactoryAdd<ZTrackBarCtrl>();
            Colorizer_FactoryAdd<ZUpDownCtrl>();
            Colorizer_FactoryAdd<ZDateTimePickerCtrl>();
            Colorizer_FactoryAdd<ZMonthCalendarCtrl>();
            Colorizer_FactoryAdd<ZRichEditCtrl>();
        }
        if (!SpecGxf().m_hImageList) {
            SpecGxf().Create(IDB_SPECGFX, 9, 1, 0xff00ff);
        }
    }

#pragma endregion 
#pragma region Windows messages handler

    LRESULT Colorizer::OnDrawItem(UINT message, WPARAM wParam, LPARAM lParam)
    {
        const auto drawItemSt = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
        const auto hWndItem = ::GetDlgItem(this->m_hWnd, static_cast<int>(drawItemSt->CtlID));
        LRESULT rv = ::SendMessage(hWndItem, message, wParam, lParam);
        SetMsgHandled(TRUE);
        return rv;
    }
    
    LRESULT Colorizer::OnNotify(UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT  rv = 0;
        auto header = reinterpret_cast<LPNMHDR>(lParam);
        SetMsgHandled(FALSE);
        switch (header->code) {
        case NM_CUSTOMDRAW:
      //case TVN_GETDISPINFOW:
      //case TVN_GETDISPINFOA:
            //DBGTPrint(LTH_COLORIZER L" >> CUSTOMDRAW %d (%d)\n", header->idFrom, static_cast<int>(wParam));
            rv = ::SendMessageW(header->hwndFrom, message, wParam, lParam);
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
        SuperclassForEachChild(m_hWnd);
        return isDialog ? TRUE : 0;
    }

    void Colorizer::OnDestroy()
    {
#ifdef _DEBUG
        CStringW text;
        GetWindowTextW(text);
        DBGTPrint(LTH_COLORIZER L" - %p %s...\n", m_hWnd, text.GetString());
#endif
        SetMsgHandled(FALSE);
    }

    void Colorizer::SuperclassForEachChild(HWND hwnd)
    {
#ifdef _DEBUG
        CStringW text;
        GetWindowTextW(text);
        DBGTPrint(LTH_COLORIZER L" + %p %s...\n", hwnd, text.GetString());
#endif
        EnumChildWindows(hwnd, reinterpret_cast<WNDENUMPROC>(InitItem), reinterpret_cast<LPARAM>(this));
    }

    BOOL CALLBACK Colorizer::InitItem(HWND hwnd, Colorizer& self)
    {
        const auto ci = self.Controls.find(hwnd);
        bool  already = ci != self.Controls.end();

        CStringW _class;
        int classLen = GetClassNameW(hwnd, _class.GetBufferSetLength(4096), 4096);
        _class.ReleaseBufferSetLength(classLen);
        _class.MakeUpper();

        const auto fact = CtrlFactory().find(_class);
        bool      exist = fact != CtrlFactory().end();
        if (!exist) {
            DBGTPrint(LTH_COLORIZER L" >> %p '%s' not EXIST <<\n", hwnd, _class.GetString());
        }
        if (exist && !already) {
            ::ShowWindow(hwnd, SW_HIDE);
            ControlCreator create = fact->second;
            ControlPtr  citem = create(hwnd, self);
            self.Controls[hwnd]   = std::move(citem);
            ::ShowWindow(hwnd, SW_SHOW);
        }

        return TRUE;
    }

    bool Colorizer::ColorizerAdd(HWND hwnd)
    {
        return 0 != InitItem(hwnd, *this);
    }

#pragma endregion 
#pragma region Common stuff

    void Colorizer::SetTextColor(CDCHandle dc) const
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);
        dc.SetBkColor(MyBackColor);
    }

    LRESULT Colorizer::OnEraseBackground(CDCHandle dc, CRect const& rc)
    {
        dc.FillSolidRect(rc, MyBackColor);
        SetTextColor(dc);
        return 0;
        // MSDN: An application should return nonzero if it erases the background; otherwise, it should return zero.
    }

    LRESULT Colorizer::OnEraseMyBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, MyBackColor);
        SetTextColor(dc);
        return 0;
        // MSDN: An application should return nonzero if it erases the background; otherwise, it should return zero.
    }

    void Colorizer::OnNcPaint(CRgnHandle rgn)
    {
        if (Win567().IsCompositionEnabled()) {
            SetMsgHandled(FALSE);
            return ;
        }
        NcPainContext nc(m_hWnd, rgn);
        CRect rcClient;
        ::GetClientRect(m_hWnd, rcClient);

        WTL::CRgn clientRgn;
        clientRgn.CreateRectRgnIndirect(rcClient);

        nc.Dc.SelectClipRgn(clientRgn);
        nc.Dc.FillSolidRect(nc.Rect, 0xff00ff);

        LONG style = 0, estyle = 0;
        BorderFlags bt = GetBorderType(m_hWnd, style, estyle);
        DrawControlBorder(nc.Dc, nc.Rect, bt);
    }

    HBRUSH Colorizer::OnCtlColorStatic(CDCHandle dc, HWND)
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);
        dc.SetBkColor(MyBackColor);
        return MyBackBrush[0];
    }

    HBRUSH Colorizer::OnCtlColorBtn(CDCHandle dc, HWND)
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);
        dc.SetBkColor(MyBackColor);
        return MyBackBrush[1];
    }

#pragma endregion 
#pragma region Border drawing

    BorderFlags Colorizer::GetBorderType(HWND hwnd, LONG& style, LONG& estyle)
    {
        style = ::GetWindowLongW(hwnd, GWL_STYLE);
        estyle = ::GetWindowLongW(hwnd, GWL_EXSTYLE);
        BorderFlags result = BorderNone;
        if (style & WS_BORDER) {
            result = BorderFrame;
        }
        else if (estyle & WS_EX_CLIENTEDGE) {
            result = BorderClientEdge;
        }
        else if (estyle & WS_EX_STATICEDGE) {
            result = BorderStaticEdge;
        }                
        return result;
    }
    
    void Colorizer::DrawControlBorder(CDCHandle dc, CRect const& rcPaint, BorderFlags border) const
    {
        CRect rc = rcPaint;
        for (int i=0; i<2; i++) {
            HPEN lp = dc.SelectPen(MyBorderPen[0]);
            dc.MoveTo(rc.left, rc.bottom, nullptr);
            dc.LineTo(rc.left, rc.top);
            dc.LineTo(rc.right, rc.top);
            if (BorderFrame != border) {
                dc.SelectPen(MyBorderPen[1]);
            }
            dc.LineTo(rc.right, rc.bottom);
            dc.LineTo(rc.left, rc.bottom);
            dc.SelectPen(lp);
            if (BorderClientEdge != border) {
                break;
            }
            rc.DeflateRect(1, 1);
        }
    }

#pragma endregion
#pragma region Text drawing

    void Colorizer::PutText(CDCHandle             dc,
                            const CRect&          rc,
                            const CString&      text,
                            UINT             tformat,
                            bool             deflate,
                            int                   dx,
                            int                   dy)
    {
        if (text.IsEmpty()) {
            return ;
        }
        CRect rcText(rc);
        if (deflate) {
            rcText.DeflateRect(dx, dy);
        }
        if (DT_SINGLELINE & tformat) {
            dc.DrawText(text, text.GetLength(), rcText, tformat);
        }
        else {
            CRect rcCalc;
            dc.DrawText(text, text.GetLength(), rcCalc, DT_CALCRECT);
            Rc::PutInto(rcText, rcCalc, tformat & 0xf);
            dc.DrawText(text, text.GetLength(), rcCalc, tformat);
        }
    }

#pragma endregion
#pragma region GroupBox drawing

    void Colorizer::DrawGroupBox(CDCHandle dc, CRect const& rc, CString const& text, LONG style) const 
    {
        CSize szText;
        if (!text.IsEmpty()) {
            dc.GetTextExtent(text, text.GetLength(), &szText);
        }
        LONG c2y = szText.cy / 2;
        HPEN  lp = dc.SelectPen(MyBorderPen[0]);
        dc.SelectBrush(MyBackBrush[0]);

        CRect rcFrame = rc;
        rcFrame.top += c2y;
        dc.RoundRect(rcFrame, CPoint(6, 6));
        
        dc.SelectPen(MyBorderPen[1]);
        rcFrame.DeflateRect(1, 1);
        dc.RoundRect(rcFrame, CPoint(6, 6));

        if (!text.IsEmpty()) {
            CRect rcText = rc;
            if (BS_CENTER == style) {
                rcText.left = (rcText.Width() - szText.cx) / 2;
            }
            else if (BS_RIGHT == style) {
                rcText.left = rcText.right - szText.cx - 8;
            }
            else {
                rcText.left += 8;
            }
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

    void Colorizer::DrawButton(CDCHandle            dc,
                               const CRect&         rc,
                               const CString&     text,
                               UINT            tformat /* = Details<T>::GetDrawTextFormat(citem)*/,
                               UINT              state /* = citem.GetState() */,
                               bool               flat,
                               bool           _default) const
    {
        UNREFERENCED_ARG(_default);
        HPEN lp = dc.SelectPen(MyPen);
        if (!flat) {
            CF::GradRect(dc, rc, MyButtonBackColor);
        }
        else {
            dc.FillSolidRect(rc, MyBackColor);
        }
        PutText(dc, rc, text, tformat, true, 3, 3);
        CF::FrameRect(dc, rc);
        if (state & BST_PUSHED) {
            dc.InvertRect(rc);
            // TODO: Make some noise here...
        }
        if (state & BST_FOCUS) {
            // TODO: Implement focus marker
            CRect rcFocus(rc);
            rcFocus.DeflateRect(2, 2);
            dc.SelectPen(MyFocusPen);
            CF::FrameRect(dc, rcFocus);
        }
        dc.SelectPen(lp);
    }

#pragma endregion
#pragma region CheckBox

    void Colorizer::DrawCheckBox(CDCHandle        dc,
                                 CRect const&     rc,
                                 CString const& text,
                                 UINT        tformat,
                                 LONG          style /* = citem.GetWindowLong(GWL_STYLE) */,
                                 LONG         estyle /* = citem.GetWindowLong(GWL_EXSTYLE) */,
                                 UINT          state /* = citem.GetState() */) const 
    {
        dc.FillSolidRect(rc, MyBackColor);

        CRect rcBox(0, 0, rc.Height(), rc.Height());
        CRect rcText = rc;

        if ((estyle & WS_EX_RIGHT) || (style & BS_LEFTTEXT)) {
            rcBox.MoveToXY(rc.right - rcBox.Width(), rc.top);
            rcText.right -= rcBox.Width() + 2;
        }
        else {
            rcBox.MoveToXY(rc.left, rc.top);
            rcText.left += rcBox.Width() + 2;
        }

        HPEN lp = dc.SelectPen(MyBorderPen[0]);
        dc.SelectBrush(MyBackBrush[0]);
        dc.RoundRect(rcBox, CPoint(6, 6));
        dc.SelectPen(MyBorderPen[1]);
        rcBox.DeflateRect(1, 1);
        dc.RoundRect(rcBox, CPoint(6, 6));

        if (BST_CHECKED & state) {
            rcBox.DeflateRect(3, 3);

            dc.SelectPen(MyThickEdgePen);
            dc.MoveTo(rcBox.left, rcBox.bottom);
            dc.LineTo(rcBox.right, rcBox.top);
            dc.MoveTo(rcBox.right, rcBox.bottom);
            dc.LineTo(rcBox.left, rcBox.top);
        }
        else if (BST_INDETERMINATE & state) {
            rcBox.DeflateRect(2, 2);

            dc.SelectBrush(MyBackBrush[1]);
            dc.RoundRect(rcBox, CPoint(6, 6));
        }

        if (BST_FOCUS & state) {
            // TODO: Implement focus marker
            CRect rcFocus(rcText);
            rcFocus.DeflateRect(-2, 1, 1, 2);

            dc.SelectPen(MyFocusPen);
            CF::FrameRect(dc, rcFocus);
        }

        PutText(dc, rcText, text, tformat);
        dc.SelectPen(lp);
    }

#pragma endregion 
#pragma region RadioButton

    void Colorizer::DrawRadioButton(CDCHandle dc, CRect const& rc, CString const& text, UINT tformat, LONG style, LONG estyle, UINT state) const 
    {
        dc.FillSolidRect(rc, MyBackColor);

        CRect rcBox(0, 0, rc.Height(), rc.Height());
        CRect rcText = rc;
        if ((estyle & WS_EX_RIGHT) || (style & BS_LEFTTEXT)) {
            rcBox.MoveToXY(rc.right - rcBox.Width(), rc.top);
            rcText.right -= rcBox.Width() + 2;
        }
        else {
            rcBox.MoveToXY(rc.left, rc.top);
            rcText.left += rcBox.Width() + 2;
        }

        HPEN lp = dc.SelectPen(MyBorderPen[0]);
        dc.SelectBrush(MyBackBrush[0]);
        dc.Ellipse(rcBox);
        dc.SelectPen(MyBorderPen[1]);
        rcBox.DeflateRect(1, 1);
        dc.Ellipse(rcBox);

        if (BST_CHECKED & state) {
            rcBox.DeflateRect(2, 2);
            dc.SelectPen(MyBorderPen[0]);
            dc.SelectBrush(MyBackBrush[2]);
            dc.Ellipse(rcBox);
        }

        PutText(dc, rcText, text, tformat);

        if (BST_FOCUS & state) {
            // TODO: Implement focus marker
            CRect rcFocus(rcText);
            rcFocus.DeflateRect(-2, 1, 1, 2);

            dc.SelectPen(MyFocusPen);
            CF::FrameRect(dc, rcFocus);
        }

        dc.SelectPen(lp);
    }

#pragma endregion
#pragma region Combo & list boxes

    LRESULT Colorizer::DrawItem(LPDRAWITEMSTRUCT  di,
                                const CString&  text /* citem.GetItemText(di->itemID) */,
                                CImageList     ilist /* citem.GetImageList()*/,
                                int           iindex /* citem.GetImageIndex(di->itemID)  */,
                                bool       eraseback) const
    {
        CDCHandle dc(di->hDC);
        CRect rc(di->rcItem);
        if (eraseback) {
            dc.FillSolidRect(rc, MyBackColor);
        }
        dc.SetBkMode(TRANSPARENT);
        if (-1 == di->itemID) {
            return CDRF_DOERASE;
        }
        if (di->itemState & ODS_SELECTED) {
            dc.FillSolidRect(rc, MyHotBackColor);
            dc.SetTextColor(MyHotTextColor);
        }
        else {
            dc.SetTextColor(MyTextColor);
        }

        bool drawIcon = (-1 != iindex) && (nullptr != ilist.m_hImageList);
        int iconx = 0, icony = 0;
        if (drawIcon) {
            ImageList_GetIconSize(ilist.m_hImageList, &iconx, &icony);
        }
        if (!text.IsEmpty()) {
            CRect rcText(rc);
            rcText.DeflateRect(2, 2);
            if (drawIcon) {
                rcText.left += iconx + 3;
            }
            dc.DrawText(text, text.GetLength(), rcText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
        if (drawIcon) {
            ImageList_Draw(ilist.m_hImageList, iindex, dc, rc.left + 2, rc.top + 2, 0);
        }
        return CDRF_DODEFAULT;
    }

    void Colorizer::DrawComboFace(CDCHandle            dc,
                                  CRect         const& rc,
                                  int                 iid /* = citem.GetCurSel() */,
                                  CStringW const&    text /* = citem.GetItemText(iid) */,
                                  CImageList const& ilist /* = citem.GetImageList() */,
                                  int              iindex /*= citem.GetImageIndex(iid)*/) const 
    {
        CF::GradRect(dc, rc, MyButtonBackColor);
        CF::FrameRect(dc, rc);

        int by = rc.Height();
      //bool framed = Details<T>::DropdownList != Details<T>::GetAppearType(citem);

        CRect rcItem(rc);
        rcItem.DeflateRect(3, 3);
        rcItem.top -= 1;
        rcItem.bottom += 1;
        rcItem.right -= (by - 2);

        DRAWITEMSTRUCT di = {0};
        di.hDC = dc;
        di.rcItem = rcItem;
        di.itemID = iid;

        DrawItem(&di, text, ilist, iindex, /*framed*/ false);

      //if (framed)
      //{
      //    dc.SelectPen(MyPen);
      //    CF::FrameRect(dc, rcItem);
      //}

        CRect rcBox(0, 0, 9, 9);
        rcBox.MoveToXY(rc.right - by + 6, rc.top + 6);
        SpecGxf().DrawEx(SpecgDown, dc, rcBox, CLR_NONE, MyTextColor, ILD_TRANSPARENT);
    }

#pragma endregion
}
