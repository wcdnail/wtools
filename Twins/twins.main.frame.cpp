#include "stdafx.h"
#include "twins.h"
#include "twins.funcs.h"
#include "twins.main.frame.h"
#include "status.ui.h"
#include "status.bar.ex.h"
#include "commands.h"
#include <twins.langs/twins.lang.strings.h>
#include <info/runtime.information.h>
#include <dh.tracing.h>

namespace Twins
{
    MainFrame::MainFrame()
        :     Super()
        , vSplitter()
        ,   ToolBar(TabBar::ReleaseSelection)
        , ButtonBar(TabBar::AutoWidth)
    {
    }

    MainFrame::SplitterCtrl::SplitterCtrl() 
        : Super()
    {
        m_bFullDrag = FALSE;
    }

    MainFrame::SplitterCtrl::~SplitterCtrl()
    {
    }

    MainFrame::~MainFrame()
    {
    }

    void MainFrame::SetupCaption()
    {
        Runtime::InfoStore const& info = Runtime::Info();
        CStringW text;
        text.Format(L"%s %s",
            info.Executable.Version.ProductName.c_str(),
            // ##TODO: remove TESTING... mark in production
            _LS(StrId_OnlyforTESTing)
            // ##TODO: is need registration info etc...
            // ##TODO: application running instance counter...
            );
        SetWindowTextW(text);
    }

    BOOL MainFrame::PreTranslateMessage(MSG* message)
    {
        if (NULL != m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, message)) {
            SendMessageW(m_hWnd, message->message, message->wParam, message->lParam);
            // ##TODO: check SendMessageW return code
            return TRUE;
        }
        return FALSE;
    }

    BOOL MainFrame::OnIdle()
    {
        return FALSE;
    }

    BOOL MainFrame::OnEraseBkgnd(CDCHandle dc) 
    {
        return TRUE; 
    }

    BOOL MainFrame::SplitterCtrl::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, 0x202020);
        return TRUE; 
    }

    void MainFrame::OnActivate(UINT state, BOOL minimized, HWND) 
    {
        if (TRUE != minimized)
            ActivePanel().SetFocus();
    }

    void MainFrame::OnKeyDown(UINT code, UINT rep, UINT flags) 
    {
        //State->OnKeyDown(code, flags); 
        Commands().OnKeyDown(code, flags);
    }

    void MainFrame::OnDestroy()
    {
        App().IsMainframeMaximized = (TRUE == IsZoomed());
        if (!App().IsMainframeMaximized)
            GetWindowRect(App().AppRect);

        vSplitter.DestroyWindow();
        App().MessageLoop.RemoveMessageFilter(this);

        SetMsgHandled(FALSE);
        ::PostQuitMessage(0);
    }

    void MainFrame::ShowWindow(int show)
    {
        if (show != SW_HIDE)
        {
            Super::ShowWindow(App().IsMainframeMaximized ? SW_SHOWMAXIMIZED : show);
            Super::UpdateWindow();
        }
        else
            Super::ShowWindow(show);
    }

    int MainFrame::OnCreate(LPCREATESTRUCT cs)
    {
        App().MessageLoop.AddMessageFilter(this);

        MoveWindow(App().AppRect);

        App().Icon.LoadIcon(IDR_MAINFRAME);
        if (!App().Icon.IsNull())
        {
            SetIcon(App().Icon, TRUE);
            SetIcon(App().Icon, FALSE);
        }

        SetupCaption();

        CRect rc;
        GetClientRect(rc);

        CRect rcToolbar(rc);
        static const int ToolbarHeight = 24; 
// ##TODO: Configure main toolbar height"))

        rcToolbar.bottom = rcToolbar.top + ToolbarHeight;
        rc.top = rcToolbar.bottom;

        HWND toolBar = ToolBar.Create(m_hWnd, rcToolbar, NULL, 0, 0, ID_TOOLBAR);

        SetupMenu();

        CreateSimpleStatusBar(_T(""));
        UiStatus().SubclassWindow(m_hWndStatusBar);

        UIAddToolBar(toolBar);
        UISetCheck(ID_TOOLBAR, 1);
        UISetCheck(ID_STATUSBAR, 1);

        UpdateBarsPosition(rc, TRUE); 

        static const int ButtonsHeight = 20; 
// ##TODO: Configure main bottombar buttons height"))

        CRect rcButtons(rc.left, rc.bottom - ButtonsHeight - 1, rc.right, rc.bottom);
        rc.bottom -= ButtonsHeight + 1;

        ButtonBar.Create(m_hWnd, rcButtons, NULL, 0, 0, ID_BUTTONS);

        static const int CommandLineHeight = 24; 
// ##TODO: Configure commandline height"))

        CRect rcCLine(rc.left, rcButtons.top - CommandLineHeight - 1, rc.right, rcButtons.top);
        rc.bottom -= CommandLineHeight;

        App().Commandline.Create(m_hWnd, rcCLine, ID_COMMANDLINE);

        m_hWndClient = vSplitter.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, ID_VSPLITTER);

        if (NULL == vSplitter.m_hWnd)
            throw std::runtime_error("Can't create CLIENT pane");

        vSplitter.ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

// ##TODO: Configure splitter"))
        vSplitter.m_cxySplitBar = 3; // Thickness
        vSplitter.m_bFullDrag = false;
        //vSplitter.m_bEraseBk = FALSE;

        vSplitter.SetSplitterPos();

        CRect rcLeft, rcRight;
        vSplitter.GetSplitterPaneRect(SPLIT_PANE_LEFT, rcLeft);
        vSplitter.GetSplitterPaneRect(SPLIT_PANE_RIGHT, rcRight);

        App().Panels[0].Create(vSplitter, rcLeft);
        App().Panels[1].Create(vSplitter, rcRight);

        Twins::Drive::Enum().OnEnumDone() = std::bind(DriveBar::InitializeDrives,
            std::ref(App().Panels[0].Drives), std::ref(App().Panels[1].Drives),
            std::placeholders::_1);

        Commands().Initialize();
        App().Commandline.OnCancel = CancelEditCommandLine;

        UINT dflags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
        for (size_t i=0; i<_countof(App().ButtonBarId); i++) {
            Command::Definition const& def = Commands().GetCommand(App().ButtonBarId[i]);
            WString name = def.GetKeyIdName() + L" " + def.Name.c_str();
            ButtonBar.Add(name.c_str(), dflags, nullptr, App().ButtonBarId[i]);
        }

        ButtonBar.OnClick() = std::bind(&MainFrame::OnButtonCommand, this, 
            std::ref(ButtonBar), std::placeholders::_1, std::placeholders::_2
        );

        vSplitter.SetSplitterPanes(App().Panels[0], App().Panels[1]);

        DlgResize_Init(false, false);
        Twins::Drive::Enum().Acquire();
        return 1;
    }

    int MainFrame::OnButtonCommand(TabBar& buttons, int id, PCTSTR text)
    {
        DH::TPrintf(L"PnButton: %2d `%s`\n", id, text);
        buttons.ClearHot();
        Commands().GetCommand(id).Callback();
        return id;
    }

    void MainFrame::OnCommand(UINT code, int id, CWindow)
    {
        if (0 == code)
        {
            DH::TPrintf(L"Mainframe: OnCommand %d %d\n", code, id);

            Command::Definition const& def = Command::Manager::Instance().GetCommand(id);
            def.Callback();
        }
    }

    void MainFrame::SplitterCtrl::DrawSplitterBar(CDCHandle dc)
    {
        CRect rc;
        if (GetSplitterBarRect(&rc))
            dc.FillSolidRect(rc, 0x404040);
    }
}
