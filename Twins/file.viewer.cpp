#include "stdafx.h"
#include "file.viewer.h"
#include "file.list.h"
#include "keyboard.h"
#include <generic.increment.h>
#include <dh.tracing.h>
#include <twins.langs/twins.lang.strings.h>
#include <win-5-6-7.features.h>
#include <memory>

namespace Twins
{
    FileViewer::~FileViewer()
    {}

    FileViewer::FileViewer(CRect& rc, bool standAlone /*= StandAlone::No*/)
        : Super()
        , Views()
        , BlockSize(1024 * 1024)
        , MyRect(rc)
        , LoopPtr(NULL)
        , IsStandAlone(standAlone)
    {}

    BOOL FileViewer::PreTranslateMessage(MSG* message)
    {
        return Super::PreTranslateMessage(message);
    }

    BOOL FileViewer::OnForwardMsg(LPMSG message, DWORD data)
    {
        if (PreTranslateMessage(message))
            return TRUE;

        return FALSE;
    }

    void FileViewer::Show(Fl::List const& files, CMessageLoop& loop, HWND parent /*= NULL*/)
    {
        LoopPtr = &loop;

        if (!m_hWnd)
        {
            Super::SetReflectNotifications(true);
            Super::SetTabStyles(CTCS_TOOLTIPS | CTCS_SCROLL | CTCS_CLOSEBUTTON | CTCS_HOTTRACK | CTCS_FLATEDGE | CTCS_BOLDSELECTEDTAB);

            Super::Create(parent, MyRect, NULL
                , WS_OVERLAPPEDWINDOW 
                | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
                , WS_EX_APPWINDOW
                , 0U, (LPVOID)&files);
        }

        if (m_hWnd)
            ShowWindow(SW_SHOW);
    }

    FileViewer::ErrorCode FileViewer::Add(Fl::Entry const& entry)
    {
        Fv::Viewer* view = NULL;
        ErrorCode error;

        try
        {
            std::unique_ptr<Fv::Viewer> temp = std::make_unique<Fv::Viewer>();
            CRect rcTemp(rcDefault);
            error = temp->Create(m_hWnd, rcTemp);
            if (error)
            {
                DH::ThreadPrintf("FileView: create view failed - %s (%d)\n"
                    , error.value(), error.message().c_str());

                return error;
            }

            view = temp.release();
            error = view->Open(entry, BlockSize);
            if (error)
            {
                view->DestroyWindow();
                return error;
            }
            
            Views.push_back(view);

            int n = AddTabWithIcon(*view, entry.GetFilename().c_str(), view->GetSmallIcon());
            ItemType* item = GetTabCtrl().GetItem(n);
            if (NULL != item) {
                item->SetToolTip(entry.GetPath().c_str());
            }
        }
        catch (...) {
            if (NULL != view)
                view->DestroyWindow();

            if (!error)
                error.assign(EINVAL, std::generic_category());
        }

        return error;
    }

    bool FileViewer::InitTabs(Fl::List const* files)
    {
        if (!files || files->Count() < 1)
            return false;

        int counter = 0;
        for (Fl::List::const_iterator it = files->Begin(); it != files->End(); ++it)
        {
            if (!it->IsDir())
            {
                ErrorCode error = Add(*it);

                if (!error)
                    ++counter;
            }
        }

        SelectTab(--counter);
        return true;
    }

    void FileViewer::OnSetFocus(HWND last)
    {
        ::SetFocus(m_hWndActive);
    }

    int FileViewer::OnCreate(LPCREATESTRUCT cs)
    {
        BOOL handled = FALSE;
        LRESULT lr = Super::OnCreate(WM_CREATE, 0, (LPARAM)cs, handled);
        if (0 != lr)
            return (int)lr;

        BOOST_ASSERT(0 == Views.size());

        if (!InitTabs((Fl::List const*)cs->lpCreateParams))
            return -1;

        if (GetTabCtrl().GetItemCount() < 1)
            return -1;

        //OnCompositionChanged();

        if (NULL != LoopPtr)
            LoopPtr->AddMessageFilter(this);
        
        int index = GetTabCtrl().GetCurSel();
        if (IsTabIndexOk(index))
            UpdateInfos(Views[index]);

        return 0;
    }

    LRESULT FileViewer::OnCompositionChanged(UINT, WPARAM, LPARAM)
    {
#if 0
        if (Win567().IsCompositionEnabled())
        {
            CRect rc;
            GetClientRect(rc);

            MARGINS margins = { 0, 0, 0, rc.Height() - GetTabAreaHeight() };
            Win567().ExtendFrameIntoClientArea(m_hWnd, &margins);
        }
#endif

        return 0;
    }

    BOOL FileViewer::OnEraseBkgnd(CDCHandle dc) 
    { 
        if (Win567().IsCompositionEnabled())
        {
            CRect rc;
            GetClientRect(rc);
            dc.FillSolidRect(rc, 0);
        }

        ::InvalidateRect(m_hWndActive, NULL, FALSE);
        return TRUE; 
    }

    void FileViewer::OnSize(UINT, CSize sz)
    {
        if (m_hWnd)
        {
            if (sz.cy < GetTabAreaHeight())
            {
                SetMsgHandled(TRUE);
                return ;
            }

            OnCompositionChanged();
            ::InvalidateRect(m_hWndActive, NULL, FALSE);
        }

        SetMsgHandled(FALSE);
    }

    bool FileViewer::IsTabIndexOk(int index) const
    {
        return (index >= 0) && (index < (int)Views.size());
    }

    void FileViewer::SelectTab(int index)
    {
        if (IsTabIndexOk(index))
        {
            int last = GetTabCtrl().GetCurSel();
            if (last != index)
                GetTabCtrl().SetCurSel(index);
        }
    }

    void FileViewer::UpdateInfos(Fv::Viewer const* view)
    {
        CString caption; 
// ##TODO: Caption"))
        caption.Format(L"%s: %s", _LS(StrId_View), view->GetPath().c_str()); 
        SetWindowText(caption);

        SetIcon(view->GetBigIcon(), TRUE);
        SetIcon(view->GetSmallIcon(), FALSE);
    }

    LRESULT FileViewer::OnSelChange(LPNMHDR pnmh)
    {
        BOOL handled = TRUE;
        Super::OnSelChange(0, pnmh, handled);
        
        if (pnmh && (pnmh->hwndFrom == GetTabCtrl()))
        {
            int sel = GetTabCtrl().GetCurSel();
            if (IsTabIndexOk(sel))
                UpdateInfos(Views[sel]);
        }

        return 0;
    }

    LRESULT FileViewer::OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        Super::OnSettingChange(uMsg, wParam, lParam, bHandled);

        ::InvalidateRect(GetTabCtrl(), NULL, FALSE);
        ::InvalidateRect(m_hWndActive, NULL, FALSE);

        SetMsgHandled(TRUE);
        bHandled = m_bMsgHandled;
        return 0;
    }

    void FileViewer::OnKeyDown(UINT code, UINT, UINT flags)
    {
        Keyboard::KeyState state(code, flags);

        switch (code)
        {
        case VK_ESCAPE: 
            DestroyWindow(); 
            break;

        case VK_TAB:
            if (state.Ctrl)
            {
                int sel = GetTabCtrl().GetCurSel();
                Generic::Increment(sel, state.Shift ? -1 : 1, 0, GetTabCtrl().GetItemCount()-1, true);
                SelectTab(sel);
            }
            break;

        case (UINT)'W':
            if (state.Ctrl)
                CloseCurrentTabAndExitIfEmpty();
            break;
        }
    }

    bool FileViewer::DeleteView(int index)
    {
        if (IsTabIndexOk(index))
        {
            ViewerList::iterator it = Views.begin() + index;
            (*it)->DestroyWindow();
            Views.erase(it);
            return true;
        }

        return false;
    }

    void FileViewer::CloseTab(int index)
    {
        if (DeleteView(index))
        {
            GetTabCtrl().DeleteItem(index);
        }
    }

    LRESULT FileViewer::OnDestroyIfEmpty(UINT, WPARAM, LPARAM)
    {
        if (GetTabCtrl().GetItemCount() < 1)
            ::DestroyWindow(m_hWnd);

        return 0;
    }

    bool FileViewer::ExitIfEmpty()
    {
        ::PostMessage(m_hWnd, WM_DESTROYIFEMPTY, 0, 0);
        return false;
    }

    void FileViewer::CloseCurrentTabAndExitIfEmpty()
    {
        CloseTab(GetTabCtrl().GetCurSel());
        ExitIfEmpty();
    }

    void FileViewer::OnDestroy()
    {
        GetWindowRect(MyRect);

        for (ViewerList::iterator it = Views.begin(); it != Views.end(); ++it)
            (*it)->DestroyWindow();

        Views.clear();
    
        if (LoopPtr)
            LoopPtr->RemoveMessageFilter(this);

        LoopPtr = NULL;

        SetMsgHandled(FALSE);

        if (IsStandAlone)
            ::PostQuitMessage(0);
    }

    LRESULT FileViewer::OnClosePage(LPNMHDR pnmh)
    {
        if (pnmh && (pnmh->hwndFrom == GetTabCtrl()))
        {
            NMCTCITEM* nmh = (NMCTCITEM*)pnmh;
            DH::ThreadPrintf(L"FileView: OnClosePage %d\n", nmh->iItem);

            CloseTab(nmh->iItem);
            ExitIfEmpty();
        }

        return 0;
    }
}
