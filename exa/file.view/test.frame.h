#pragma once

#include "file.viewer.h"
#include <atlapp.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlframe.h>
#include <atlcrack.h>

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

        BEGIN_MSG_MAP_EX(Frame)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_ACTIVATE(OnActivate)
            CHAIN_MSG_MAP(Resizer)
        END_MSG_MAP()

    private:
        enum { ID_VIEW = 1000 };

        Fv::Viewer View;

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle dc);
        void OnActivate(UINT state, BOOL minimized, CWindow other);

        BEGIN_DLGRESIZE_MAP(Frame)
            DLGRESIZE_CONTROL(ID_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()


    private:
        Frame(Frame const&);
        Frame& operator = (Frame const&);
    };

    inline Frame::Frame()
        : Super()
        , View()
    {}

    inline Frame::~Frame()
    {}

    inline void Frame::OnActivate(UINT state, BOOL minimized, CWindow other)
    {
        if (NULL != (HWND)View)
            View.SetFocus();
    }

    inline int Frame::OnCreate(LPCREATESTRUCT)
    {
        Fv::ErrorCode err = View.Create(*this, ID_VIEW);
        if (!err)
        {
            //View.Open(L"types.h");
            //View.Open(L"file.viewer.cpp", 65536);
            //View.Open(L"test/README_GRUB4DOS.txt");
            View.Open(L"test/Библия Fallout (перевод).txt", 65536);
            //View.Open(L"C:/Windows/explorer.exe");
            //View.Open(L"d:/Загружено/Готовые/Фумлу/Djentlmeny.udachi.1971.mkv");
        }

        DlgResize_Init(false, false);
        return 0;
    }

    inline void Frame::OnDestroy()
    {
        ::PostQuitMessage(0);
        SetMsgHandled(FALSE);
    }

    inline BOOL Frame::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, ::GetSysColor(COLOR_DESKTOP));
        return 1;
    }
}
