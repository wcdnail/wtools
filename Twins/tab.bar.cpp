#include "stdafx.h"
#include "tab.bar.h"
#include "tab.bar.item.h"
#include "keyboard.h"
#include <dh.tracing.h>
#include <atlstr.h>
#include <windows.gdi.rects.h>
#include <algorithm>
#include <functional>

namespace Twins
{
    TabBar::TabBar(int flags /*= Normal*/)
        : Cf::DoubleBuffered(0xf8e0ca, true) // CONF: panel tabs double buffering
        , Items()
        , ClickFunctor()
        , IconSize(16, 16)
        , Flags(flags)
        , HotItem(NULL)
        , SelItem(NULL)
        , TabPos(-1)
        , MyFont(CreateFont(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Tahoma")))
        , MyTextColor(0xdfdfdf)
        , MyHotTextColor(0xffffff)
        , MySelTextColor(0xffffff)
    {
        MyBackColor[0] = 0x7f7f7f;
        MyBackColor[1] = 0x000000;

        MyBackHotColor[0] = 0x100000;
        MyBackHotColor[1] = 0x4f0f0f;

        MyBackSelColor[0] = 0x000000;
        MyBackSelColor[1] = 0x7f7f7f;
    }

    TabBar::~TabBar()
    {}

    bool TabBar::AutoWidthOn() const
    {
        return 0 != (AutoWidth & Flags);
    }

    bool TabBar::DontHighlightOn() const
    {
        return 0 != (DontHighlight & Flags);
    }

    bool TabBar::ReleaseSelectionOn() const
    {
        return 0 != (ReleaseSelection & Flags);
    }

    bool TabBar::ButtonModeOn() const
    {
        return 0 != (ButtonMode & Flags);
    }

    int TabBar::OnCreate(LPCREATESTRUCT)
    {
        return 0;
    }

    void TabBar::Clear()
    {
        Items.clear();
        HotItem = NULL;
        ::InvalidateRect(m_hWnd, NULL, FALSE);
    }

    void TabBar::Add(int id, PCWSTR name, int width /*= -1*/, HICON icon /*= NULL*/, UINT dflags /*= DT_LEFT | DT_VCENTER | DT_SINGLELINE*/)
    {
        TabBarItem newItem;

        newItem.id = id;
        newItem.icon = icon;
        newItem.name = name;
        newItem.pos.x = 0;
        newItem.pos.y = width;
        newItem.dflags = dflags;

        Items.push_back(newItem);
    }

    void TabBar::Add(PCWSTR name, UINT dflags/* = DT_LEFT | DT_VCENTER | DT_SINGLELINE*/, HICON icon/* = NULL*/, int id/* = -1*/)
    {
        Add((-1 == id ? (int)Items.size() : id), name, -1, icon, dflags);
    }

    bool TabBar::SetWidth(int id, int width)
    {
        if ((id >= 0) && (id < (int)Items.size()))
        {
            Items[id].pos.y = width;
            ::InvalidateRect(m_hWnd, NULL, FALSE);
            return true;
        }

        return false;
    }

    bool TabBar::IsHot(TabBarItem const& it) const
    {
        return &it == HotItem;
    }

    bool TabBar::IsSelected(TabBarItem const& it) const
    {
        return &it == SelItem;
    }

    void TabBar::OnPaint(CDCHandle senderDc)
    {
        WTL::CPaintDC paintDc(m_hWnd);
        Cf::BufferedPaint bufferedPaint(paintDc, GetSecondDc(), IsDoubleBuffered(), m_hWnd);
        CDC& curDc = bufferedPaint.GetCurrentDc();
        if (!curDc.m_hDC)
        {
            ::InvalidateRect(m_hWnd, NULL, FALSE);
            return ;
        }

        CRect rc;
        GetClientRect(rc);
        HFONT lastFont = curDc.SelectFont(MyFont);
        Cf::GradRect(curDc, rc, MyBackColor);

        curDc.SetBkMode(TRANSPARENT);
        
        int bottom = rc.bottom - 1;
        CRect rcItem = rc;

        int count = (int)Items.size();
        int cellWidth = rc.Width() / (0 == count ? 1 : count);

        int counter = 0;
        for (ItemVec::const_iterator it = Items.begin(); it != Items.end(); ++it)
        {
            TabBarItem const& cur = *it;

            if (AutoWidthOn())
                cur.pos.y = cellWidth;

            cur.pos.x = rcItem.left;
            rcItem.right = cur.Right();

            COLORREF textColor = MyTextColor;

            if (IsHot(cur))
            {
                textColor = MyHotTextColor;
                Cf::GradRect(curDc, rcItem, MyBackHotColor);
            }
            else if (IsSelected(cur))
            {
                textColor = MySelTextColor;
                Cf::GradRect(curDc, rcItem, MyBackSelColor);
            }

#if _DEBUG_TAB_BAR
            if (counter == Cursor)
            {
                CRect rcFocus = rcItem;
                rcFocus.DeflateRect(2, 2);
                curDc.DrawFocusRect(rcFocus);
            }
#endif
            
            curDc.SetTextColor(textColor);

            cur.Draw(curDc, rcItem, IconSize);

            if (rcItem.right < (rc.right - 8))
            {
                curDc.MoveTo(rcItem.right-1, 0);
                curDc.LineTo(rcItem.right-1, bottom);
            }

            rcItem.left = rcItem.right;
            ++counter;
        }

        CRect rcEdge = rc;
        --rcEdge.right;
        --rcEdge.bottom;
        Cf::FrameRect(curDc, rcEdge);

        curDc.SelectFont(lastFont);
    }

    static TabBarItem ptDummyItem;

    bool TabBar::IsValid(TabBarItem const& it)
    {
        return &it != &ptDummyItem;
    }

    TabBarItem& TabBar::Access(int i)
    {
        if (i < 0 || i > (Count() - 1))
            return ptDummyItem;

        return Items[i];
    }

    TabBarItem& TabBar::GetItem(int i)
    {
        return Access(i);
    }

    TabBarItem const& TabBar::GetItem(int i) const
    {
        return const_cast<TabBar*>(this)->Access(i);
    }

    TabBarItem const& TabBar::GetItem(CPoint const& point) const
    {
        ItemVec::const_iterator it = std::find_if(Items.begin(), Items.end(), std::bind(&TabBarItem::IsIn, std::placeholders::_1, point));
        return it == Items.end() ? ptDummyItem : *it;
    }

    TabBarItem& TabBar::GetItem(CPoint const& point)
    {
        ItemVec::iterator it = std::find_if(Items.begin(), Items.end(), std::bind(&TabBarItem::IsIn, std::placeholders::_1, point));
        return it == Items.end() ? ptDummyItem : *it;
    }

    template <class Iter>
    Iter TabBar::GetById(int id)
    {
        for (Iter it = Items.begin(); it != Items.end(); ++it)
        {
            if (it->IsIdEq(id))
                return it;
        }

        return Items.end();
    }

    int TabBar::GetIndexById(int id) const
    {
        ItemVec::const_iterator it = const_cast<TabBar*>(this)->GetById<ItemVec::const_iterator>(id);
        return (it == Items.end() ? -1 : (int)std::distance(Items.begin(), it));
    }

    void TabBar::SetSelectedById(int id)
    {
        ItemVec::const_iterator it = const_cast<TabBar*>(this)->GetById<ItemVec::const_iterator>(id);
        if (it != Items.end())
            SetSelected(&(*it));
    }

    void TabBar::SetIconById(int id, HICON icon)
    {
        ItemVec::iterator it = GetById<ItemVec::iterator>(id);
        if (it != Items.end())
        {
            it->icon = icon;
            ::InvalidateRect(m_hWnd, NULL, FALSE);
        }
    }

    void TabBar::ClearSelected()
    {
        SetSelected(NULL);
    }

    void TabBar::SetHotByPtr(TabBarItem const* itPtr)
    {
        HotItem = itPtr;
        ::InvalidateRect(m_hWnd, NULL, FALSE);
    }

    void TabBar::SetSelected(TabBarItem const* itPtr)
    {
        SelItem = itPtr;
        ::InvalidateRect(m_hWnd, NULL, FALSE);
    }

    void TabBar::OnLButtonDown(UINT flags, CPoint point)
    {
        TabBarItem const& it = GetItem(point);
        if (IsValid(it))
            SetSelected(&it);
    }

    void TabBar::InvokeOnClick(TabBarItem const& it) const
    {
        if (ClickFunctor)
            ClickFunctor(it.id, it.name);
    }

    void TabBar::OnLButtonUp(UINT flags, CPoint point)
    {
        TabBarItem const& it = GetItem(point);
        if (IsValid(it))
            InvokeOnClick(it);

        if (ReleaseSelectionOn())
            ClearHot();
    }

    void TabBar::OnMouseMove(UINT flags, CPoint point)
    {
        if (!DontHighlightOn())
        {
            TabBarItem const& it = GetItem(point);
            if (IsValid(it))
            {
                SetHotByPtr(&it);

                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
                ::TrackMouseEvent(&tme);
            }
        }
    }

    void TabBar::OnMouseLeave()
    {
        SetHotByPtr(NULL);
    }

    void TabBar::SetHot(int id)
    {
        if (id >= 0 && id < (int)Items.size())
            SetSelected(&Items[id]);
    }

    void TabBar::ClearHot()
    {
        SetSelected(NULL);
    }

    TabBar::ClickCallback& TabBar::OnClick()
    {
        return ClickFunctor;
    }

    void TabBar::SetIconSize(int cx, int cy)
    {
        IconSize.SetSize(cx, cy);
        ::InvalidateRect(m_hWnd, NULL, FALSE);
    }

    LRESULT TabBar::WM_CONTEXTMENU_ToParent(UINT message, WPARAM wParam, LPARAM lParam) const
    {
        return ::SendMessage(GetParent(), WM_CONTEXTMENU, (WPARAM)m_hWnd, lParam);
    }

    int TabBar::Count() const 
    {
        return (int)Items.size(); 
    }

    TabBarItem& TabBar::GetItemById(int id)
    {
        ItemVec::iterator it = GetById<ItemVec::iterator>(id);
        if (it == Items.end())
            return ptDummyItem;

        return *it;
    }

    UINT TabBar::OnGetDlgCode(LPMSG message)
    {
        //Dh::ThreadPrintf(L"DialogCd: %s\n", message ? Dh::MessageToStrignW(message) : L"--None");

        if (NULL != message)
        {
            switch (message->message)
            {
            case WM_KEYDOWN: 
                HandleDlgKeyDown((UINT)message->wParam, (UINT)((message->lParam & 0xffff0000) >> 16)); 
                break;
            }
        }

        return DLGC_BUTTON | DLGC_WANTARROWS | DLGC_WANTTAB;
    }

    void TabBar::MoveTab(int x)
    {
        if (SetTabPos(TabPos + x, false))
        {
            SetHotByPtr(NULL);
            ::SendMessage(GetParent(), WM_NEXTDLGCTL, (0 == TabPos ? 0 : 1), 0);
        }

        if (GetFocus() == m_hWnd)
            SetHotByPtr(&GetItem(TabPos));
    }

    void TabBar::HandleDlgKeyDown(UINT code, UINT flags)
    {
        Keyboard::KeyState ks(code, flags);

        switch (code)
        {
        case VK_TAB:    
            MoveTab(ks.Shift ? -1 : 1); 
            break;

        case VK_RIGHT:
        case VK_LEFT:   
            MoveTab(VK_LEFT == code ? -1 : 1); 
            break;

        case VK_UP:
        case VK_DOWN:   
            MoveTab(VK_UP == code ? -Count() : Count()); 
            break;

        case VK_SPACE:
        {
            TabBarItem const& it = Access(TabPos);
            if (IsValid(it))
                InvokeOnClick(it);

            break;
        }
        }
    }

    bool TabBar::SetTabPos(int x, bool sethot /*= true*/)
    {
        TabPos = x;
        bool overrun = false;

        if (TabPos > (Count()-1))
        {
            TabPos = 0;
            overrun = true;
        }
        else if (TabPos < 0)
        {
            TabPos = Count() - 1;
            overrun = true;
        }

        if (sethot)
            SetHotByPtr(&GetItem(TabPos));

        return overrun;
    }

    void TabBar::OnSetFocus(HWND prev)
    {
        Keyboard::KeyState ks;
        SetTabPos(ks.Shift ? Count() - 1 : 0);
    }

    void TabBar::OnKillFocus(HWND next)
    {
        SetHotByPtr(NULL);
    }
}
