#pragma once

#include "panel.view.scanner.h"
#include "panel.view.sort.h"
#include "panel.view.header.h"
#include "panel.view.columns.h"
#include "panel.view.edit.h"
#include "panel.view.quicksearch.h"
#include "panel.view.selecting.h"
#include "panel.view.status.h"
#include "panel.view.dnd.h"
#include <settings.h>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <atltypes.h>
#include <atlctrls.h>
#include <atlgdi.h>

namespace Twins
{
    typedef CWinTraits< WS_CHILD | WS_VISIBLE 
                      | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                      | WS_BORDER
                      , 0
                      | WS_EX_ACCEPTFILES
                      > PVTraits;

    class PanelView: boost::noncopyable
                   , ATL::CWindowImpl<PanelView, ATL::CWindow, PVTraits>
                   , WTL::CDialogResize<PanelView>
    {
        DECLARE_WND_CLASS_EX(_T("[Twins]PanelView"), CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, COLOR_WINDOW)

    private:
        typedef boost::function<bool(FItem const*, CString const&)> RenameCallback;
        typedef ATL::CWindowImpl<PanelView, ATL::CWindow, PVTraits> Super;
        typedef WTL::CDialogResize<PanelView> SuperResizer;

    public:
        enum { InvalidIndex = -1 };

        int iHot;
        FScanner Scanner;
        ViewStatus Status;
        CCursor Cursor;
        CSize IconSize;
        RenameCallback Renamer;

        PanelView(int id);
        ~PanelView();

        using Super::Create;
        using Super::SetFocus;
        using Super::SetFont;
        using Super::operator HWND;
        using Super::SetWindowPos;
        using Super::GetWindowRect;
        using Super::KillTimer;
        using Super::SetTimer;

        void OnLoadContentBegin();
        void OnLoadContentDone();
        void SetHot(std::wstring const& hotname, bool invalidate);
        int GetIndexInRange(int index) const;
        void EnsureVisible(int id, bool invalidate = true);
        void SetHot(int id, bool invalidate = true);
        FItem* GetHot() const;
        std::wstring GetHotFilename() const;
        void Clear();
        int FindByName(std::wstring const& name) const;
        CRect GetVisibleItemRect(int index) const;
        void StartLabelEdit();
        int QuickSearch(int from, std::wstring const& prefix, bool ignoreCase) const;
        bool GetSelection(Fl::List& files) const;
        int GetIndexBeforeSelection() const;
        void ClearSelection();
        bool NeedReload(DirectoryNotifyMap const& nmap) const;
        void UpdateView(PCWSTR dirpath, DirectoryNotifyMap const& nmap);
        bool IsIndexValid(int n) const;
        void OnEntryProperties() const;
        void XorSelection(bool invalidate = true);
        void AddSubSelection(bool mark, bool invalidate = true);
        void EnterItem();
        void ToogleSelectionAndMoveCursor(bool down);
        void MoveCursorUpDown(bool down);
        void MoveCursorHomeEnd(bool down);
        void MoveCursorPageUpDown(bool down);
        void Invalidate() const;

    private:
        friend class Super;
        friend class SuperResizer;
        friend class DragnDropHelper;
        friend struct AutoLocker;

        CFont MyFont;
        int iFirst;
        PanelHeader Header;
        LabelEdit Edit;
        mutable volatile long Locker;
        QuickSearchHelper Search;
        SelectHelper Selector;
        PanelColumns Columns;
        mutable CDC SnapshotDc;
        mutable HBITMAP SnapshotBm;
        DragnDropHelper DragnDrop;
        FSort Sorter;
        std::wstring LastPath;

        void ResetView(bool invalidate);
        int GetScrollerWidth() const;
        CRect GetListRect() const;
        void Lock(bool invalidate) const;
        void Unlock(bool invalidate) const;
        bool IsLocked() const;
        int OnCreate(LPCREATESTRUCT cs);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle) const;
        void OnPaint(CDCHandle senderDc) const;
        void OnKeyDown(UINT code, UINT rep, UINT flags);
        int OnMouseDown(UINT flags, CPoint point, bool right = false, bool up = false, bool invl = true);
        void OnLButtonDblClk(UINT flags, CPoint point);
        void ScrollList(int amount, bool down);
        BOOL OnMouseWheel(UINT flags, short delta, CPoint point);
        void OnSetFocus(HWND);
        int GetVisibleCount(CRect const& rc, int& itemHeight) const;
        int GetVisibleCount() const;
        void DrawList(WTL::CDC& dc, CRect const& rc) const;
        int GetItemHeight() const;
        bool EnsureVisible(int index, int vc, bool invalidate = true);
        int GetItemFrom(CPoint const& point, CRect const& rc, int itemHeight) const;
        int GetItemFrom(CPoint const& point, bool invalidate) const;
        void MoveCursor(int count, bool invalidate = true);
        void StartLabelEdit(CRect const& rc, int index, FItem* it);
        void CancelLabelEdit();
        void OnLabelEditDone(int index, CString const& newname);
        void OnVScroll(UINT code, UINT pos, CScrollBar bar);
        void OnDropFiles(HDROP dropInfo);
        void OnContextMenu(CWindow control, CPoint point);
        void OnRButtonUp(UINT flags, CPoint point);
        void SelectToggleHot();
        int OnSort(int, PCWSTR = NULL);
        int GetItemCount() const;
        int GetItemIndex(CPoint const& pt, CRect const& rc) const;
        int GetItemIndex(CPoint const& pt) const;

        enum
        {
            ID_HEADER = 1007,
            ID_SCROLLER,
            ID_LABELEDIT,
        };

        BEGIN_DLGRESIZE_MAP(PanelView)
            DLGRESIZE_CONTROL(ID_HEADER, DLSZ_SIZE_X)
        END_DLGRESIZE_MAP()

        BEGIN_MSG_MAP_EX(PanelView)
            CHAIN_MSG_MAP_MEMBER(Search)
            CHAIN_MSG_MAP_MEMBER(DragnDrop)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_LBUTTONDOWN(OnMouseDown)
            MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
            MSG_WM_RBUTTONUP(OnRButtonUp)
            MSG_WM_MOUSEWHEEL(OnMouseWheel)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_VSCROLL(OnVScroll)
            MSG_WM_DROPFILES(OnDropFiles)
            MSG_WM_CONTEXTMENU(OnContextMenu)
            CHAIN_MSG_MAP(SuperResizer)
        END_MSG_MAP()

    public:
        Conf::Section Options;
    };

    inline int PanelView::GetItemCount() const { return (int)Scanner.Items.size(); }
}

