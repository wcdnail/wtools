#pragma once

#include <windows.wtl.buffered.paint.h>
#include <atlctrls.h>
#include <atltypes.h>
#include <boost/function.hpp>

namespace Twins
{
    struct TabBarItem;

    typedef CWinTraits< WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0> PanelTabTraits;

    class TabBar: ATL::CWindowImpl<TabBar, ATL::CWindow, PanelTabTraits>
                , Cf::DoubleBuffered
    {
    public:
        typedef ATL::CWindowImpl<TabBar, ATL::CWindow, PanelTabTraits> Super;
        typedef std::vector<TabBarItem> ItemVec;
        typedef boost::function<int(int, PCTSTR)> ClickCallback;

        enum
        {
            Normal              = 0x0000,
            AutoWidth           = 0x0001,
            DontHighlight       = 0x0002,
            ReleaseSelection    = 0x0004,
            ButtonMode          = DontHighlight | ReleaseSelection,
        };

        TabBar(int flags = Normal);
        ~TabBar();

        using Super::Create;
        using Super::Invalidate;
        using Super::LockWindowUpdate;
        using Super::GetWindowRect;
        using Super::SetWindowPos;
        using Super::SetFocus;
        using Super::EnableWindow;
        using Super::DestroyWindow;
        using Super::operator HWND;

        int Count() const;
        void Clear();
        void Add(int id, PCWSTR name, int width = -1, HICON icon = NULL, UINT dflags = DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        void Add(PCWSTR name, UINT dflags = DT_LEFT | DT_VCENTER | DT_SINGLELINE, HICON icon = NULL, int id = -1);
        bool SetWidth(int n, int width);
        void SetHot(int n);
        void SetSelectedById(int id);
        void SetIconById(int id, HICON icon);
        void ClearHot();
        void ClearSelected();
        void SetIconSize(int cx, int cy);
        
        ClickCallback& OnClick();

        bool AutoWidthOn() const;
        bool DontHighlightOn() const;
        bool ButtonModeOn() const;
        bool ReleaseSelectionOn() const;

        TabBarItem const& GetItem(int i) const;
        TabBarItem& GetItem(int i);
        TabBarItem const& GetItem(CPoint const& point) const;
        TabBarItem& GetItemById(int id);

        static bool IsValid(TabBarItem const& it);

    protected:
        ItemVec Items;
        ClickCallback ClickFunctor;
        CSize IconSize;
        int Flags;
        TabBarItem const* HotItem; 
        TabBarItem const* SelItem; 
        int TabPos;

        // Appearance
        CFont MyFont;
        COLORREF MyTextColor;
        COLORREF MyHotTextColor;
        COLORREF MySelTextColor;
        COLORREF MyBackColor[2];
        COLORREF MyBackHotColor[2];
        COLORREF MyBackSelColor[2];

        using Super::m_hWnd;
        using Super::GetParent;
        using Super::GetClientRect;

        int GetIndexById(int id) const;

    private:
        friend Super;

        int CalcStartPos() const;
        bool IsHot(TabBarItem const& it) const;
        void SetHotByPtr(TabBarItem const* itPtr);
        bool IsSelected(TabBarItem const& it) const;
        void SetSelected(TabBarItem const* itPtr);
        int OnCreate(LPCREATESTRUCT);
        void OnPaint(CDCHandle senderDc);
        void OnLButtonDown(UINT flags, CPoint point);
        void InvokeOnClick(TabBarItem const& it) const;
        void OnLButtonUp(UINT flags, CPoint point);
        void OnMouseMove(UINT flags, CPoint point);
        void OnMouseLeave();
        template <class Iter>
        Iter GetById(int id);
        TabBarItem& GetItem(CPoint const& point);
        LRESULT WM_CONTEXTMENU_ToParent(UINT message, WPARAM wParam, LPARAM lParam) const;
        UINT OnGetDlgCode(LPMSG message);
        void HandleDlgKeyDown(UINT code, UINT flags);
        bool SetTabPos(int x, bool sethot = true);
        TabBarItem& Access(int i);
        void OnSetFocus(HWND prev);
        void OnKillFocus(HWND next);
        void MoveTab(int x);
        
    protected:
        BEGIN_MSG_MAP_EX(PanelView)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MESSAGE_HANDLER_EX(WM_RBUTTONUP, WM_CONTEXTMENU_ToParent)
            MSG_WM_GETDLGCODE(OnGetDlgCode)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_KILLFOCUS(OnKillFocus)
            CHAIN_MSG_MAP(Cf::DoubleBuffered)
        END_MSG_MAP()
    };
}
