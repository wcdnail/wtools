#pragma once

#include "dnd.target.h"
#include <atlapp.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <memory>

void ShowErrorBox(HRESULT hr, PCWSTR caption, HWND parent = NULL);

namespace Test
{
    class Frame: WTL::CFrameWindowImpl<Frame, ATL::CWindow>
               , WTL::CDialogResize<Frame>
    {
    private:
        typedef WTL::CFrameWindowImpl<Frame, ATL::CWindow> Super;
        typedef WTL::CDialogResize<Frame> Resizer;

    public:
        Frame();
        ~Frame();

        using Super::Create;
        using Super::ShowWindow;

    private:
        friend class Super;
        friend class Resizer;

        Dnd::Target DropTarget;

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle dc);
        void OnActivate(UINT state, BOOL minimized, CWindow other);

        BEGIN_DLGRESIZE_MAP(Frame)
            //DLGRESIZE_CONTROL(ID_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()

        BEGIN_MSG_MAP_EX(Frame)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_ACTIVATE(OnActivate)
            CHAIN_MSG_MAP(Resizer)
        END_MSG_MAP()

    private:
        Frame(Frame const&);
        Frame& operator = (Frame const&);
    };
}
