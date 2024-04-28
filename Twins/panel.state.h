#pragma once

#if 0
#include <gdi+.animation.h>
#include <windows.wtl.buffered.paint.h>
#include <atlctrls.h>
#include <atltypes.h>
#include <atlstr.h>
#include <boost/function.hpp>

namespace Twins
{
    typedef CWinTraits< WS_CHILD | WS_VISIBLE 
                      | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                      , 0
                      > PanelStateTraits;

    class PanelState: ATL::CWindowImpl<PanelState, ATL::CWindow, PanelStateTraits>
                    , CF::DoubleBuffered
    {
    private:
        typedef ATL::CWindowImpl<PanelState, ATL::CWindow, PanelStateTraits> Super;

    public:
        typedef boost::function<void(void)> ActionCallback;

        PanelState();
        ~PanelState();

        void SetAnimation(Ui::Animation& anim);
        void RemoveAnimation();
        void SetSourceView(HWND sourceHandle);

        CStringW& CaptionText();
        CStringW& ActionText();
        ActionCallback& Action();

        using Super::Create;

        void Show();
        void Hide();

    private:
        friend Super;

        Ui::Animation* animPtr_;
        CStringW caption_;
        CStringW action_;
        CRect rcAction_;
        Gdiplus::Font capFont_;
        Gdiplus::Font actFont_;
        Gdiplus::StringFormat textFormat_;
        Gdiplus::SolidBrush textBrush_;
        Gdiplus::SolidBrush actBrush_;
        HCURSOR cursor_;
        HCURSOR actionCursor_;
        ActionCallback actionCallback_;
        CDC backgroundImg_;

        int OnCreate(LPCREATESTRUCT);
        void OnPaint(CDCHandle);
        void OnPaint(CDC& dc, CRect const& rc);
        void OnMouseMove(UINT, CPoint point);
        void OnLButtonDown(UINT, CPoint point);

        BEGIN_MSG_MAP_EX(PanelState)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            CHAIN_MSG_MAP(CF::DoubleBuffered)
        END_MSG_MAP()

        static void DummyAction();
    };
}
#endif
