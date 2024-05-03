#pragma once

#include <windows.wtl.buffered.paint.h>
#include <atlctrls.h>
#include <atltypes.h>
#include <boost/function.hpp>

namespace Ut
{
    typedef CWinTraits< WS_CHILD | WS_VISIBLE 
                      | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                      , 0
                      | WS_EX_CLIENTEDGE
                      > HLControlTraits;

    class HLControl: ATL::CWindowImpl<HLControl, ATL::CWindow, HLControlTraits>
                   , Cf::DoubleBuffered
    {
    private:
        typedef ATL::CWindowImpl<HLControl, ATL::CWindow, HLControlTraits> Super;
        typedef boost::function<void(HWND, CDC&, CRect const&)> OnPaintFunction;
        typedef boost::function<void(void)> Function;

    public:
        HLControl();
        ~HLControl();

        using Super::m_hWnd;
        using Super::Create;

        template <class T>
        void SetOnPaint(T const& pred);

    private:
        friend Super;

        OnPaintFunction onPaint_;

        int OnCreate(LPCREATESTRUCT);
        void OnPaint(CDCHandle senderDc);

        BEGIN_MSG_MAP_EX(PanelView0)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_PAINT(OnPaint)
            CHAIN_MSG_MAP(Cf::DoubleBuffered)
        END_MSG_MAP()
    };

    template <class T>
    inline void HLControl::SetOnPaint(T const& pred)
    {
        onPaint_ = (OnPaintFunction const&)pred;
        ::InvalidateRect(m_hWnd, NULL, TRUE);
    }
}
