#include "stdafx.h"
#include "sqlbro.appwindow.h"
#include "sqlbro.main.h"
#include <exceptions.any.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlribbon.h>
#include <atlsplit.h>

namespace Sqlbro
{
    class AppWindow::Implementation: public CRibbonFrameWindowImpl<Implementation>
                                   , public CMessageFilter
                                   , public CIdleHandler
                                   , public CDialogResize<Implementation>
    {
    public:
        DECLARE_FRAME_WND_CLASS_EX(_T("SQLBRO::APPFRAME"), 0, CS_VREDRAW | CS_HREDRAW, COLOR_WINDOW)
    
        typedef CRibbonFrameWindowImpl<Implementation> Super;
        typedef CDialogResize<Implementation> SuperResizer;

        enum ControlIds
        {
            CtlId_Toolbar = ATL_IDW_TOOLBAR,
            CtlId_Statusbar = ATL_IDW_STATUS_BAR,
            CtlId_Vsplitter = CtlId_Statusbar + 2000,
            CtlId_Hsplitter,
        };

        CMessageLoop& LoopRef;
        CCommandBarCtrl CmdBar;
        CSplitterWindow Vsplitter;
        CHorSplitterWindow Hsplitter;
        CMultiPaneStatusBarCtrl Statusbar;
        CIcon MainIcon;

        Implementation(CMessageLoop& loop);
        ~Implementation();

        virtual BOOL PreTranslateMessage(MSG*);
        virtual BOOL OnIdle();
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle dc);

        BEGIN_DLGRESIZE_MAP(Implementation)
            DLGRESIZE_CONTROL(CtlId_Statusbar, DLSZ_SIZE_X | DLSZ_MOVE_Y)
            DLGRESIZE_CONTROL(CtlId_Vsplitter, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()

        BEGIN_MSG_MAP_EX(Implementation)
            try
            {
                MSG_WM_CREATE(OnCreate);
                MSG_WM_DESTROY(OnDestroy);
                MSG_WM_ERASEBKGND(OnEraseBkgnd);
                CHAIN_MSG_MAP(SuperResizer);
                CHAIN_MSG_MAP(Super);
            }
            catch(...)
            {
                Cf::HandleAnyException(_ShowException);
            }
        END_MSG_MAP()

        BEGIN_RIBBON_CONTROL_MAP(Implementation)
        END_RIBBON_CONTROL_MAP()

    private:
        Implementation(Implementation const&);
        Implementation& operator = (Implementation const&);
    };

    AppWindow::AppWindow(CMessageLoop& loop)
        : Impl(new Implementation(loop))
    {}

    AppWindow::~AppWindow()
    {}

    AppWindow::Implementation::Implementation(CMessageLoop& loop)
        : LoopRef(loop)
        , Vsplitter()
        , Hsplitter()
        , Statusbar()
        , MainIcon()
    {}

    AppWindow::Implementation::~Implementation()
    {}

    BOOL AppWindow::Implementation::PreTranslateMessage(MSG* messageLoop)
    { 
        return Super::PreTranslateMessage(messageLoop); 
    }

    BOOL AppWindow::Implementation::OnIdle()
    {
        return FALSE;
    }

    BOOL AppWindow::Implementation::OnEraseBkgnd(CDCHandle dc)
    {
        return TRUE;
    }

    void AppWindow::Implementation::OnDestroy()
    {
        Statusbar.DestroyWindow();
        Statusbar.m_hWnd = NULL; // FIXME: WTL bug

        Hsplitter.DestroyWindow();
        Vsplitter.DestroyWindow();

        LoopRef.RemoveMessageFilter(this);
        LoopRef.RemoveIdleHandler(this);

        ::PostQuitMessage(0);
        SetMsgHandled(FALSE);
    }

    int AppWindow::Implementation::OnCreate(LPCREATESTRUCT cs)
    {
        //UIAddMenu(GetMenu(), true);
        //CmdBar.Create(m_hWnd, rcDefault, NULL, WS_CHILD);
        //CmdBar.AttachMenu(GetMenu());
        //CmdBar.LoadImages(IDR_MAINFRAME);

        //MainIcon.LoadIcon(IDR_MAINFRAME);
        //if (NULL != MainIcon.m_hIcon)
        //{
        //    SetIcon(MainIcon, TRUE);
        //    SetIcon(MainIcon, FALSE);
        //}

        HWND toolBarHandle = CreateSimpleToolBarCtrl(m_hWnd, 0/*IDR_MAINFRAME*/, TRUE
            , WS_CHILD | WS_VISIBLE 
            | WS_CLIPCHILDREN | WS_CLIPSIBLINGS 
            | RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_AUTOSIZE
            | TBSTYLE_FLAT);

        if (NULL != toolBarHandle)
        {
            CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
            AddSimpleReBarBand(toolBarHandle);
            UIAddToolBar(toolBarHandle);
        }

        ShowRibbonUI(true);

        CreateSimpleStatusBar(_T(""));
        if (m_hWndStatusBar)
        {
            UIAddStatusBar(m_hWndStatusBar);
            Statusbar.Attach(m_hWndStatusBar);
            int statucCols[] = { ID_DEFAULT_PANE, 0, 0 };
            Statusbar.SetPanes(statucCols, _countof(statucCols), false);
        }

        CRect rc;
        GetClientRect(rc);
        UpdateBarsPosition(rc, FALSE);

        Vsplitter.Create(m_hWnd, rc, NULL
            , WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
            , 0, CtlId_Vsplitter);

        if (NULL != Vsplitter.m_hWnd)
        {
            m_hWndClient = Vsplitter.m_hWnd;
            Vsplitter.SetSplitterPos(rc.Width() / 5);

            UIAddChildWindowContainer(m_hWndClient);

            Hsplitter.Create(Vsplitter, rcDefault, NULL
                , WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
                , 0, CtlId_Hsplitter);

            if (NULL != Hsplitter.m_hWnd)
            {
                Vsplitter.SetSplitterPane(SPLIT_PANE_RIGHT, Hsplitter);
                Hsplitter.SetSplitterPos(rc.Height() - rc.Height() / 4);
            }
        }

        LoopRef.AddMessageFilter(this);
        LoopRef.AddIdleHandler(this);
        
        DlgResize_Init(false, false);
        return 0;
    }

    void AppWindow::OnRun()
    {
        if (!Impl->Create())
        {
            HRESULT hr = ::GetLastError();
            throw CrashException("MainFrame.Create failed!", hr);
        }

        Impl->ShowWindow(SW_SHOW);
        Impl->UpdateWindow();
    }
}
