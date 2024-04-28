#pragma once

#include "drive.enum.h"
#include "tab.bar.h"
#include <boost/noncopyable.hpp>
#include <atlapp.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <atluser.h>
#include "res/resource.h"

namespace Twins
{
    struct Application;

    typedef CWinTraits< WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                      , WS_EX_APPWINDOW
                      > MainFrameTraits;

    class MainFrame: CFrameWindowImpl<MainFrame, CWindow, MainFrameTraits>
                   , CUpdateUI<MainFrame>
                   , CDialogResize<MainFrame>
                   , CMessageFilter
                   , CIdleHandler
    {
    private:
        friend struct Application;

        DECLARE_FRAME_WND_CLASS_EX(_T("Twins:MAINFRAME"), IDR_MAINFRAME, CS_VREDRAW | CS_HREDRAW, COLOR_WINDOW);

        typedef CFrameWindowImpl<MainFrame, CWindow, MainFrameTraits> Super;
        typedef CDialogResize<MainFrame> SuperResizer;
        typedef CUpdateUI<MainFrame> SuperUiUpdater;

        MainFrame();
        virtual ~MainFrame();

        using Super::CreateEx;
        using Super::UpdateWindow;
        using Super::GetWindowText;
        using Super::operator HWND;

        void ShowWindow(int cmdShow);

        class SplitterCtrl : public CSplitterWindowImpl<SplitterCtrl>
        {
        public:
            typedef CSplitterWindowImpl<SplitterCtrl> Super;
            
            SplitterCtrl();
            ~SplitterCtrl();

            BOOL OnEraseBkgnd(CDCHandle);
            void DrawSplitterBar(CDCHandle dc);

            BEGIN_MSG_MAP_EX(SplitterCtrl)
                MSG_WM_ERASEBKGND(OnEraseBkgnd)
                CHAIN_MSG_MAP(Super)
            END_MSG_MAP()
        };

    private:
        friend class Super;
        friend class SuperResizer;
        friend class SuperTheme;
        friend class SuperUiUpdater;

        SplitterCtrl vSplitter;
        TabBar ToolBar;
        TabBar ButtonBar;

        virtual BOOL PreTranslateMessage(MSG*);
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle dc);
        void OnActivate(UINT state, BOOL minimized, HWND);
        void SetupCaption();
        void OnKeyDown(UINT code, UINT rep, UINT flags);
        void OnCommand(UINT code, int id, CWindow);
        void SetupMenu();
        virtual BOOL OnIdle();
        int OnButtonCommand(TabBar& buttons, int id, PCTSTR text);

        BEGIN_MSG_MAP_EX(MainFrame)
            MSG_WM_ACTIVATE(OnActivate)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_SYSKEYDOWN(OnKeyDown)
            MSG_WM_COMMAND(OnCommand)
            CHAIN_MSG_MAP_MEMBER(Drive::Enum());
            CHAIN_MSG_MAP(SuperUiUpdater)
            CHAIN_MSG_MAP(SuperResizer)
            CHAIN_MSG_MAP(Super)
        END_MSG_MAP()

        enum ControlIds
        {
            ID_TOOLBAR = ID_VIEW_TOOLBAR,
            ID_STATUSBAR = ID_VIEW_STATUS_BAR,
            ID_VSPLITTER,
            ID_BUTTONS,
            ID_COMMANDLINE,
        };

        BEGIN_UPDATE_UI_MAP(MainFrame)
            UPDATE_ELEMENT(ID_TOOLBAR, UPDUI_MENUPOPUP)
            UPDATE_ELEMENT(ID_STATUSBAR, UPDUI_MENUPOPUP)
        END_UPDATE_UI_MAP()

        BEGIN_DLGRESIZE_MAP(MainFrame)
            DLGRESIZE_CONTROL(ID_TOOLBAR, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_VSPLITTER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
            DLGRESIZE_CONTROL(ID_BUTTONS, DLSZ_SIZE_X | DLSZ_MOVE_Y)
            DLGRESIZE_CONTROL(ID_COMMANDLINE, DLSZ_SIZE_X | DLSZ_MOVE_Y)
            DLGRESIZE_CONTROL(ID_STATUSBAR, DLSZ_SIZE_X | DLSZ_MOVE_Y)
        END_DLGRESIZE_MAP()
    };
}
