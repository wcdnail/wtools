#pragma once

#if 0
#include <windows.wtl.buffered.paint.h>
#include <atlctrls.h>
#include <atltypes.h>

namespace Twins
{
    typedef CWinTraits< WS_CHILD | WS_VISIBLE 
                      | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                      | SS_OWNERDRAW
                      , 0
                      > PanelScrollerTraits;

    class PanelScroller: ATL::CWindowImpl<PanelScroller, WTL::CStatic, PanelScrollerTraits>
                       , CF::DoubleBuffered
    {
    public:
        typedef ATL::CWindowImpl<PanelScroller, WTL::CStatic, PanelScrollerTraits> Super;

        PanelScroller();
        ~PanelScroller();

        using Super::Create;

        void SetPos(int pos, int limit);

    private:
        friend Super;

        int pos_;
        int limit_;
        
        void OnPaint(CDCHandle senderDc) const;
        
        BEGIN_MSG_MAP_EX(PanelScroller)
            MSG_WM_PAINT(OnPaint)
            CHAIN_MSG_MAP(CF::DoubleBuffered)
        END_MSG_MAP()
    };
}
#endif
