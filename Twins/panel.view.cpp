#include "stdafx.h"
#include "twins.funcs.h"
#include "panel.view.h"
#include "keyboard.h"
#include "fs.enum.h"
#include "panel.view.misc.h"
#include "shell.menus.h"
#include <dh.tracing.h>
#include <windows.wtl.buffered.paint.h>
#include <windows.gdi.rects.h>
#include <windows.gdi.rects.h>
#include <windows.wtl.message.h>
#include <string.utils.error.code.h>
#include <twins.langs/twins.lang.strings.h>
#include <atltime.h>

namespace Twins
{
    enum { PanelViewStatusHeight = 22 };

    PanelView::~PanelView()
    {
        Scanner.SelectedName = GetHotFilename();
        LastPath = Scanner.Path.FullPath().wstring();
    }

    PanelView::PanelView(int id)
        :       iHot{0}
        ,    Scanner{}
        ,     Status{}
        ,     Cursor{LoadCursor(nullptr, IDC_ARROW)}
        ,   IconSize{16, 16}
        ,    Renamer{}
        ,     MyFont{CreateFont(-13, 0, 0, 0, FW_MEDIUM, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Consolas"))}
        ,     iFirst{0}
        ,     Header{}
        ,       Edit{}
        ,     Locker{PaintAreUnlocked}
        ,     Search{*this}
        ,   Selector{}
        , SnapshotDc{}
        , SnapshotBm{nullptr}
        ,  DragnDrop{*this}
        ,     Sorter{}
        ,   LastPath{}
        ,    Options{::Settings(), std::wstring(L"Panel#") + std::to_wstring(id)}
    {
        FromSettings(Options, Columns.iHot);
        FromSettings(Options, Columns.Used);
        FromSettings(Options, Columns.Ascending);
        FromSettings(Options, Scanner.SelectedName);
        FromSettings(Options, LastPath);

        Scanner.UpdatePath(LastPath);
    }

    void PanelView::ResetView(bool invalidate)
    {
        iFirst = 0;
        iHot = 0;
        Selector.Reset(GetItemCount(), Status, m_hWnd);

        if (!Scanner.SelectedName.empty())
        {
            SetHot(Scanner.SelectedName, invalidate);
            Scanner.SelectedName.clear();
        }

        if (invalidate) {
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }
    }

    int PanelView::OnCreate(LPCREATESTRUCT cs)
    {
        CRect rc;
        ::GetClientRect(m_hWnd, rc);
        
        CRect rcHeader(0, 0, rc.right, HeaderHeight);
        if (Header.Create(m_hWnd, rcHeader, nullptr, 0, 0, ID_HEADER)) {
            Columns.OnCreate(Header);
            Header.OnClick() = std::bind(&PanelView::OnSort, this, std::placeholders::_1, std::placeholders::_2);
        }

        Edit.Create(m_hWnd, rcDefault, nullptr, WS_CHILD, WS_EX_CLIENTEDGE, ID_LABELEDIT);
        Edit.SetFont(MyFont);
        Edit.OnEditDone() = std::bind(&PanelView::OnLabelEditDone, this, std::placeholders::_1, std::placeholders::_2);

        SetCursor(Cursor);

        DlgResize_Init(false, false);
        return 0;
    }

    void PanelView::OnDestroy()
    {
        Columns.OnDestroy(Header);
        SetMsgHandled(FALSE);
    }

    int PanelView::GetScrollerWidth() const
    {
        return 0; //(vScroller.IsWindowVisible() ? ::GetSystemMetrics(SM_CXVSCROLL) : 0);
    }

    CRect PanelView::GetListRect() const
    {
        CRect rc;
        ::GetClientRect(m_hWnd, rc);
        rc.top += HeaderHeight;
        rc.right -= GetScrollerWidth();
        rc.bottom -= PanelViewStatusHeight;
        return rc;
    }

    void PanelView::Lock(bool invalidate) const
    {
        ::InterlockedExchange(&Locker, PaintAreLocked);

        if (invalidate) {
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }
    }

    void PanelView::Unlock(bool invalidate) const
    {
        ::InterlockedExchange(&Locker, PaintAreUnlocked);

        if (invalidate) {
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }
    }

    bool PanelView::IsLocked() const
    {
        long rv = ::InterlockedCompareExchange(&Locker, Locker, 1);
        return PaintAreLocked == rv;
    }

    BOOL PanelView::OnEraseBkgnd(CDCHandle dc) const
    {
        if (IsLocked())
        {
            CRect rcList = GetListRect();
            dc.FillSolidRect(rcList, 0x00050505);

            if (SnapshotDc.m_hDC) {
                dc.BitBlt(rcList.left, rcList.top, rcList.Width(), rcList.Height(), SnapshotDc, rcList.left, rcList.top, SRCCOPY);
            }
        }

        return 1;
    }

    void PanelView::OnPaint(CDCHandle /*senderDc*/) const
    {
        WTL::CPaintDC paintDc(m_hWnd);
        CRect rcList = GetListRect();

        if (IsLocked()) {
            OnEraseBkgnd(paintDc.m_hDC);
        }
        else {
            CRect rcAll;
            GetClientRect(rcAll);
            BufferedPaint paintBuffer(paintDc, rcAll, rcList, SnapshotDc, SnapshotBm);
            CDC& dc = SnapshotDc;

            HFONT lastFont = dc.SelectFont(MyFont);

            dc.FillSolidRect(rcAll, 0x00050505);
            dc.SetBkMode(TRANSPARENT);

            DrawList(dc, rcList);

            dc.MoveTo(rcList.left, rcList.top - 1);
            dc.LineTo(rcList.left, rcList.bottom - 1);
            dc.LineTo(rcList.right - 1, rcList.bottom - 1);
            dc.LineTo(rcList.right - 1, rcList.top - 1);

            CRect rcStatus(rcAll.left, rcAll.bottom - PanelViewStatusHeight, rcAll.right, rcAll.bottom);
            Status.OnPaint(dc, rcStatus);

            dc.SelectFont(lastFont);
        }
    }

    int PanelView::GetItemHeight() const
    {
        return IconSize.cy + 2;
    }

    int PanelView::GetVisibleCount(CRect const& rc, int& itemHeight) const
    {
        itemHeight = GetItemHeight();
        return (0 != itemHeight ? (rc.Height() / itemHeight) : 0);
    }

    int PanelView::GetVisibleCount() const
    {
        int h = 0;
        return GetVisibleCount(GetListRect(), h);
    }

    static bool IsItemIndexValid(FItemVector const& items, int n)
    {
        return (n >= 0) && (n < (int)items.size());
    }

    void PanelView::DrawList(WTL::CDC& dc, CRect const& rc) const
    {
        int ic = (int)Scanner.Items.size();
        int vc = GetVisibleCount() + 1;

        if ((vc < 1) || (iFirst > ic - 1))
            return ;

        CRect rcItem(rc.left + 1, rc.top, rc.left + 1 + rc.Width(), rc.top + GetItemHeight());

        int is = iFirst;
        CTime curtime = CTime::GetCurrentTime();
        for (int i = 0; i < vc; i++)
        {
            int ii = Sorter.GetIndex(is);

            if (!IsItemIndexValid(Scanner.Items, ii))
                break;

            if (iHot == is)
                dc.FillSolidRect(rcItem, IsPanelActive(*this) ? 0x00474747 : 0x00272727);
            else
            {
                COLORREF iback[2] = { 0x00202020, 0x00151515 };
                dc.FillSolidRect(rcItem, iback[(is % 2) & 1]);
            }


            bool isSelected = Selector.IsSelected(ii);
            Scanner.Items[ii]->Draw(dc.m_hDC, rcItem, isSelected, Columns.Widths, Columns.Used, curtime);

            ++is;
            rcItem.top = rcItem.bottom;
            rcItem.bottom = rcItem.top + GetItemHeight();
        }
    }

    void PanelView::OnKeyDown(UINT code, UINT rep, UINT flags)
    {
        if (!IsLocked())
            Message::RedirectTo(GetParent(), *this);
    }

    bool PanelView::EnsureVisible(int index, int vc, bool invalidate)
    {
        bool rv = false;

        int lv = iFirst + vc - 1;
        if (index > lv)
        {
            iFirst += index - lv;
            rv = true;

        }
        else if (index < iFirst)
        {
            iFirst -= iFirst - index;
            rv = true;
        }

        if (rv)
        {
            if (invalidate)
                ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }

        return rv;
    }

    void PanelView::MoveCursor(int count, bool invalidate)
    {
        int last = iHot;
        int next = last;
        next += count;

        int maxCount = GetItemCount() - 1;
        if (next > maxCount)
            next = maxCount;

        else if (next < 0)
            next = 0;

        if (next != last)
        {
            int vc = GetVisibleCount();
            EnsureVisible(next, vc, false);

        }

        iHot = next;

        if (invalidate)
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    int PanelView::GetItemFrom(CPoint const& point, CRect const& rc, int itemHeight) const
    {
        return (0 != itemHeight ? (iFirst + ((point.y - rc.top) / itemHeight)) : -1);
    }

    int PanelView::GetItemFrom(CPoint const& point, bool invalidate) const
    {
        CRect rc = GetListRect();
        int itemHeight = 0;
        int visibleCount = GetVisibleCount(rc, itemHeight);

        int last = iHot;
        int next = GetItemFrom(point, rc, itemHeight);

        if ((-1 != next) && invalidate)
            ::InvalidateRect(m_hWnd, nullptr, FALSE);

        return next;
    }

    bool PanelView::IsIndexValid(int n) const
    {
        return (n >= 0) && (n < (int)Scanner.Items.size());
    }

    int PanelView::GetItemIndex(CPoint const& pt, CRect const& rc) const
    {
        return (iFirst + (pt.y - rc.left) / GetItemHeight()) - 1;
    }

    int PanelView::GetItemIndex(CPoint const& pt) const
    {
        int next = GetItemIndex(pt, GetListRect());
        return next;
    }

    int PanelView::OnMouseDown(UINT flags, CPoint point, bool right /*= false*/, bool up /*= false*/, bool invl /*= true*/)
    {
        if (IsLocked())
            return -1;
        
        CancelLabelEdit();
        GetParent().SetFocus();

        int next = GetItemIndex(point);
        if (IsItemIndexValid(Scanner.Items, next))
        {
            iHot = next;

            //if (!Selector.IsSelected(GetHotIndex()))
            //    Selector.Clear(Entries, Status, m_hWnd);

            if (invl)
                ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }

        return next;
    }

    void PanelView::ScrollList(int amount, bool down)
    {
        int vc = GetVisibleCount();

        int next = iFirst;
        if (down)
        {
            int dl = GetItemCount() - vc;
            if (dl > 0)
            {
                next += amount;
                if (next > dl)
                    next = dl;
            }
        }
        else
        {
            next -= amount;
            if (next < 0)
                next = 0;
        }

        if (iFirst != next)
        {
            iFirst = next;
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }
    }

    BOOL PanelView::OnMouseWheel(UINT flags, short delta, CPoint point)
    {
        if (!IsLocked())
        {
            CancelLabelEdit();
            ScrollList(2, delta < 0);
        }

        return FALSE;
    }

    void PanelView::OnLButtonDblClk(UINT flags, CPoint point)
    {
        OnMouseDown(flags, point, false, false, false);
        EnterItem();
    }

    int PanelView::GetIndexInRange(int index) const
    {
        if (index < 0)
            index = 0;
        
        else 
        {
            int uplim = GetItemCount() - 1;
            uplim = (uplim < 0 ? 0 : uplim);
        
            if (index > uplim)
                index = uplim;
        }
        
        return index;
    }

    int PanelView::OnSort(int id, PCWSTR)
    {
        if (Columns.DoSort(id, Header, Sorter, Scanner.Items))
            ::InvalidateRect(m_hWnd, nullptr, FALSE);

        return 0;
    }

    void PanelView::OnSetFocus(HWND)
    {
        SetCommandLinePath(Scanner.Path.FullPath().wstring());
        ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    void PanelView::OnLoadContentBegin() 
    {
        Scanner.DisplayHidden = IsHiddenFilesVisible();
        Status.Reset(Scanner, m_hWnd, false);
        Status.Clear(m_hWnd, true);
        Lock(true); 
    }

    void PanelView::OnLoadContentDone()
    {
        Sorter.CreateIndex(Columns.iHot, Columns.Ascending[Columns.iHot], Scanner.Items);

        CRect rc = GetListRect();
        int itemHeight = 0;
        int visibleCount = GetVisibleCount(rc, itemHeight);

        ResetView(false);
        Unlock(true);

        //Status.Update(Entries, m_hWnd, false);
        //App().Commandline.SetPath(Scanner.Path.FullPath().wstring());
    }

    void PanelView::SetHot(std::wstring const& hotname, bool invalidate)
    {
        if (!hotname.empty())
        {
            int pi = FindByName(hotname);
            if (InvalidIndex != pi)
                SetHot(pi, invalidate);
        }
    }

    void PanelView::EnsureVisible(int id, bool invalidate)
    {
        CRect rc = GetListRect();
        int itemHeight = 0;
        int vc = GetVisibleCount(rc, itemHeight);
        EnsureVisible(id, vc);
    }

    void PanelView::SetHot(int id, bool invalidate) 
    { 
        if (IsIndexValid(id))
        {
            iHot = id; 
            EnsureVisible(id, invalidate);

            if (invalidate)
                ::InvalidateRect(m_hWnd, nullptr, FALSE);
        }
    }

    FItem* PanelView::GetHot() const
    {
        int i = Sorter.GetIndex(iHot);
        return IsItemIndexValid(Scanner.Items, i) ? Scanner.Items[i] : nullptr;
    }

    std::wstring PanelView::GetHotFilename() const
    {
        if (FItem const* it = GetHot())
            return std::wstring(it->FileName, it->FileNameLength);

        return L"";
    }

    void PanelView::Clear()
    {
        Scanner.Items.clear();
        Status.Clear(m_hWnd, false);
        //Status.Reset(Entries, m_hWnd, false);
        ResetView(true);
    }

    int PanelView::FindByName(std::wstring const& name) const
    {
        int n = Sorter.FindIndexByName(Scanner.Items, name);
        return IsItemIndexValid(Scanner.Items, n) ? n : -1;
    }

    CRect PanelView::GetVisibleItemRect(int index) const
    {
        CRect rc = GetListRect();

        int itemHeight = GetItemHeight();
        int visibleIndex = index - iFirst;
        int top = rc.top + visibleIndex * itemHeight;

        return CRect(rc.left, top, rc.right, top + itemHeight);
    }

    void PanelView::StartLabelEdit(CRect const& rc, int index, FItem* it) 
    { 
        Edit.Show(rc, index, it->FileName); 
    }

    void PanelView::CancelLabelEdit() 
    { 
        Edit.Hide(); 
    }

    void PanelView::StartLabelEdit()
    {
        if (FItem* it = GetHot())
        {
            if (!it->IsUpperDir())
            {
                CRect rc = GetVisibleItemRect(iHot);
                rc.InflateRect(-2, 2);
                rc.left += IconSize.cx;

                StartLabelEdit(rc, iHot, it);
            }
        }
    }

    void PanelView::OnLabelEditDone(int index, CString const& newname)
    {
        if (Renamer)
        {
            int n = Sorter.GetIndex(index);
            if (IsIndexValid(n))
                Renamer(Scanner.Items[n], newname);
        }
    }

    int PanelView::QuickSearch(int from, std::wstring const& prefix, bool ignoreCase) const
    {
        //if (!IsIndexValid(from))
        //    return InvalidIndex;
        //
        //Fl::List::const_iterator it = 
        //     std::find_if(Entries.Begin() + from
        //                , Entries.End()
        //                , std::bind(&Fl::Entry::IsNameStartsWith, _1, prefix, ignoreCase)
        //                );
        //
        //if (it == Entries.End())
        //    return InvalidIndex;
        //
        //return (int)std::distance(Entries.Begin(), it);
        
        return -1;
    }

    void PanelView::OnVScroll(UINT code, UINT pos, CScrollBar bar)
    {
        if (IsLocked()) {
            return ;
        }
        CancelLabelEdit();
        switch (code) {
        case SB_THUMBTRACK:
            iFirst = pos;
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
            break;

        case SB_LINEUP:
        case SB_LINEDOWN:
            ScrollList(1, SB_LINEDOWN == code);
            break;

        case SB_PAGEUP:
        case SB_PAGEDOWN:
        {
            ScrollList(GetVisibleCount() - 1, SB_PAGEDOWN == code);
            break;
        }
        }
    }

    bool PanelView::GetSelection(Fl::List& files) const
    {
        //return Selector.GetSelection(Entries, files, Entries[iHot]);
        return false;
    }

    int PanelView::GetIndexBeforeSelection() const
    {
        //int rv = Selector.FirstUnselected(Entries.Count());
        //return -1 == rv ? max(0, iHot - 1) : rv;
        return -1;
    }

    void PanelView::ClearSelection()
    {
        //Selector.Clear(Entries, Status, m_hWnd);
        ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    bool PanelView::NeedReload(DirectoryNotifyMap const& nmap) const
    {
        //AutoLocker lk(*this, false, false);
        //return (int)nmap.size() > Entries.Count() - 1;
        return false;
    }

    void PanelView::UpdateView(PCWSTR dirpath, DirectoryNotifyMap const& nmap)
    {
        //AutoLocker lk(*this, false, true);
        //
        //int prevCount = Entries.Count();
        //Entries.Update(dirpath, nmap, Columns.GetCurrentLessFunction(), Columns.GetCurrentAscending());
        //int newCount = Entries.Count();
        //
        //if (newCount != prevCount)
        //    Selector.Reset(Entries, Status, m_hWnd);
    }

    void PanelView::OnDropFiles(HDROP dropInfo)
    {
        std::shared_ptr<void> drop(dropInfo, DragFinish);
        ::SendMessage(GetParent(), WM_DROPFILES, (WPARAM)dropInfo, 0);
    }

    void PanelView::OnContextMenu(CWindow control, CPoint pn)
    {
        if (Header.PtInRect(pn))
        {
            ClientToScreen(&pn);

            if (Columns.OnContextMenu(Header, pn))
            {
                Sorter.CreateIndex(Columns.iHot, Columns.Ascending[Columns.iHot], Scanner.Items);
                ::InvalidateRect(m_hWnd, nullptr, FALSE);
            }

            return ;
        }

        Fl::List files;
        if (IsIndexValid(iHot) /*&& Selector.GetSelection(Entries, files, Entries[iHot])*/)
        {
            CRect rcEntry = GetVisibleItemRect(iHot);
            CPoint pn(rcEntry.left, rcEntry.bottom);
            ClientToScreen(&pn);
            TrackShellContextMenu(*this, pn, files);
        }
        else
            SetMsgHandled(FALSE);
    }

    void PanelView::OnRButtonUp(UINT flags, CPoint pn) 
    {
        //Fl::List files;
        int n = OnMouseDown(flags, pn, true, true);
        //if (IsIndexValid(n))
        //{
        //    if (Selector.GetSelection(Entries, files, Entries[n]))
        //    {
        //        ClientToScreen(&pn);
        //        TrackShellContextMenu(*this, pn, files);
        //    }
        //}
        //else
        {
            _ATL_MSG const* message = GetCurrentMessage();
            ::SendMessage(GetParent(), WM_CONTEXTMENU, (WPARAM)m_hWnd, message->lParam);
        }
    }

    void PanelView::SelectToggleHot()
    {
        //int sel = GetHotIndex();
        //Selector.Toggle(sel);
        //Status.Count(Entries, Entries[sel], Selector.IsSelected(sel));
        //Status.Update(Entries);
    }

    void PanelView::OnEntryProperties() const
    {
        //Fl::List files;
        //if (IsIndexValid(iHot))
        //{
        //    CString list = Entries[iHot].GetPath().c_str();
        //
        //    SHELLEXECUTEINFOW senfo = {};
        //    senfo.cbSize   = sizeof(senfo);
        //    senfo.fMask    = SEE_MASK_INVOKEIDLIST;
        //    senfo.lpVerb   = L"PROPERTIES";
        //    senfo.lpFile   = list.GetBuffer();
        //    senfo.hwnd     = m_hWnd;
        //    senfo.nShow    = SW_SHOW;
        //
        //    ::ShellExecuteExW(&senfo);
        //    HRESULT hr = ::GetLastError();
        //    DH::TPrintf(_T("ShowProp: `%s` %x %s\n"), senfo.lpFile
        //        , hr, Str::ErrorCode<>::SystemMessage(hr));
        //}
    }

    void PanelView::XorSelection(bool invalidate /*= true*/)
    {
        //Selector.Toggle(Entries[0].IsUpperDir() ? 1 : 0, Entries.Count());
        //Status.Count(Entries, Selector, m_hWnd, false);

        if (invalidate)
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    void PanelView::AddSubSelection(bool mark, bool invalidate /*= true*/)
    {
        //Search.Skip();
        //Selector.SelectByRegEx(Entries, mark, Status, State.GetMainframeHwnd());
        //Status.Update(Entries, m_hWnd, false);

        if (invalidate)
            ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    void PanelView::EnterItem()
    {
        FItem const* it = GetHot();
        if (nullptr != it)
        {
            std::filesystem::path p = Scanner.Path.FullPath();

            std::wstring filename(it->FileName, it->FileNameLength);
            p /= filename;

            if (it->IsDir())
            {
                if (it->IsUpperDir())
                    Scanner.Path.Go2UpperDir(Scanner.SelectedName);
                else
                    Scanner.Path.Cd(p.c_str(), false, Scanner.SelectedName);

                Scanner.Fetch(nullptr);
            }
        }
    }

    void PanelView::ToogleSelectionAndMoveCursor(bool down)
    {
        SelectToggleHot();
        MoveCursorUpDown(down);
        ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    void PanelView::MoveCursorUpDown(bool down)
    {
        MoveCursor(down ? 1 : -1);
    }

    void PanelView::MoveCursorHomeEnd(bool down)
    {
        int count = GetItemCount();
        MoveCursor(down ? count : -count);
    }

    void PanelView::MoveCursorPageUpDown(bool down)
    {
        int vc = GetVisibleCount() - 1;
        MoveCursor(down ? vc : -vc);
    }

    void PanelView::Invalidate() const
    {
        ::InvalidateRect(Header, nullptr, FALSE);
        ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }
}
