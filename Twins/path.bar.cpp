#include "stdafx.h"
#include "twins.funcs.h"
#include "path.bar.h"
#include "panel.h"
#include <windows.wtl.popup.menus.h>
#include <windows.gdi.rects.h>

namespace Twins
{
    PathBar::PathBar(Panel& owner)
        :               Owner(owner)
        ,         BookmarkBar()
        ,                Path()
        ,            Favorite()
        ,             History()
        ,            PathEdit()
        ,              MyFont(CreateFont(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Tahoma")))
        ,         MyTextColor(0x7f7f7f)
        ,         MyBackColor(0x202020)
        ,   MyActiveTextColor(0xffffff)
        ,   MyActiveBackColor(0x5c5c5c)
        ,   MyActiveBackBrush(::CreateSolidBrush(MyActiveBackColor))
        , MyInactiveBackBrush(::CreateSolidBrush(0x121212))
        ,         MyBackBrush(::CreateSolidBrush(MyBackColor))
        ,               MyPen(::CreatePen(PS_SOLID, 1, 0x7f7f7f))
    {
        MyButtonBackColor[0] = 0x7f7f7f;
        MyButtonBackColor[1] = 0x000000;
    }

    PathBar::~PathBar()
    {
    }

    BOOL PathBar::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, MyBackColor);
        return 1;
    }

    int PathBar::OnCreate(LPCREATESTRUCT)
    {
        BookmarkBar.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE, 0, ID_PAGER);
        Path.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | SS_NOPREFIX, 0, ID_PATH);
        History.Create(m_hWnd, rcDefault, _T("^"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, ID_HISTORY);
        Favorite.Create(m_hWnd, rcDefault, _T("*"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, ID_FAVORITES);
        PathEdit.Create(m_hWnd, rcDefault, NULL, WS_CHILD, WS_EX_CLIENTEDGE, ID_PATHEDIT);
        
        BookmarkBar.SetFont(MyFont);
        Path.SetFont(MyFont);
        PathEdit.SetFont(MyFont);
        History.SetFont(MyFont);
        Favorite.SetFont(MyFont);

        PathEdit.OnEditDone() = std::bind(&PathBar::OnPathEditDone, this, std::placeholders::_1, std::placeholders::_2);

        //BookmarkBar.InsertItem(0, _T("Trololo..."));
        AdjustPositions();
        DlgResize_Init(false, false);
        return 0;
    }

    void PathBar::AdjustPositions()
    {
        static const int InitialButtonWidth = 18;

        CRect rc;
        GetClientRect(rc);

        CRect rcPath = rc;

        CRect rcHist = rcPath;
        rcHist.left = rcHist.right - InitialButtonWidth;

        CRect rcFav = rcHist;
        rcFav.right = rcHist.left - 1;
        rcFav.left = rcFav.right - InitialButtonWidth;

        rcPath.right = rcFav.left - 1;
        rcPath.DeflateRect(2, 1);

        Path.MoveWindow(rcPath);
        History.MoveWindow(rcHist);
        Favorite.MoveWindow(rcFav);
        //PathEdit.MoveWindow(rcPath);

        // int count = BookmarkBar.GetItemCount();
        //CRect rcPager = rcDefault;
        //if (count >= 1)
        //{
        //    rcPager = rc;
        //    rcPager.top += 2;
        //    rcPager.bottom = rcPager.top + rc.Height() / 2 - 2;
        //}
        //else
        //{
        //    rc.bottom = rc.top + rc.Height() / 2 + 1;
        //}

        //CRect rcPath = rc;
        //rcPath.top = rcPager.bottom + 2;
        //CRect rcHist = rcPath;
        //rcHist.left = rcHist.right - InitialButtonWidth;
        //CRect rcFav = rcHist;
        //rcFav.right = rcHist.left - 1;
        //rcFav.left = rcFav.right - InitialButtonWidth;
        //rcPath.right = rcFav.left - 1;

        //BookmarkBar.MoveWindow(rcPager);
        //Path.MoveWindow(rcPath);
        //History.MoveWindow(rcHist);
        //Favorite.MoveWindow(rcFav);

        //SetWindowPos(NULL, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
        //::SendMessage(GetParent(), MYWM_HEIGHT_CHANGED, (WPARAM)rcPager.Height(), 0);
    }

    HBRUSH PathBar::OnCtlColorStatic(CDCHandle dc, HWND)
    {
        bool active = IsPanelActive(Owner);

        dc.SetTextColor(active ? MyActiveTextColor : MyTextColor);
        dc.SetBkMode(TRANSPARENT);

        return active ? MyActiveBackBrush : MyInactiveBackBrush;
    }

    HBRUSH PathBar::OnCtlColor(CDCHandle dc, HWND)
    {
        dc.SetBkMode(TRANSPARENT);
        return MyBackBrush;
    }

    void PathBar::Invalidate() const
    {
        ::InvalidateRect(Path, NULL, TRUE);
    }

    void PathBar::SetPathHint(PCTSTR displayPath)
    {
        Path.SetWindowText(displayPath);
    }

    void PathBar::ShowHistoryPopup(UINT, int, HWND)
    {
        TrackHistoryPopup();
    }

    void PathBar::ShowFavoritesPopup(UINT, int, HWND)
    {
        Owner.SetFocus();
    }

    void PathBar::TrackHistoryPopup(bool byKey /*= false*/)
    {
        struct Helper
        {
            static CString _Concat(HistoryDeque::value_type const& p) { return p.c_str(); }
        };

        CF::PopupMenu menu;
        int count = menu.Load(1, Owner.GetDirManager().GetHistory(), Helper::_Concat);
        int n = menu.Show(byKey ? Path.m_hWnd : History.m_hWnd, *this, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD);
        if ((n >= 1) && (n <= count))
        {
            CString temp = menu.GetString(n, MF_BYCOMMAND);
            if (!temp.IsEmpty())
            {
                Fl::Entry info((PCWSTR)temp, true);
                //Owner.ChangeDir(info);
            }
        }

        Owner.SetFocus();
    }

    void PathBar::OnSetFocus(CWindow)
    {
        Owner.SetFocus();
    }

    void PathBar::EditPath()
    {
        CString text;
        Path.GetWindowText(text);

        CRect rc;
        Path.GetWindowRect(rc);
        ScreenToClient(rc);
        PathEdit.Show(rc, 0, text, true);
    }

    void PathBar::OnPathEditDone(int index, CString const& newname)
    {
        Owner.View.SetHot(CT2W(newname).m_psz, true);
    }

//////////////////////////////////////////////////////////////////////////
// BADCODE: Copy-paste from DriveBar::OnDrawItem
    void PathBar::OnDrawItem(int id, PDRAWITEMSTRUCT di) 
    {
        CDCHandle dc(di->hDC);
        CRect rc(di->rcItem);

        HPEN lpen = dc.SelectPen(MyPen);

        OnCtlColor(dc, di->hwndItem);

        CF::GradRect(dc, rc, MyButtonBackColor);
        CF::FrameRect(dc, rc);

        CWindow temp(di->hwndItem);
        int textLen = temp.GetWindowTextLength();
        if (textLen > 0)
        {
            CRect rcText(rc);
            rcText.DeflateRect(2, 2);

            CString text(_T('\0'), textLen);
            temp.GetWindowText(text);

            UINT tf = DT_LEFT;

            if (ODT_BUTTON == di->CtlType)
                tf = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

            dc.DrawText(text, textLen, rcText, tf);
        }

        dc.SelectPen(lpen);
    }
}
