#include "pch.hxx"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "clr.dracula.h"
#include "err.printer.h"
#include "debug.assistance.h"
#include <gtest/gtest.h>

using          Compositor = winrt::Windows::UI::Composition::Compositor;
using        SpriteVisual = winrt::Windows::UI::Composition::SpriteVisual;
using     ContainerVisual = winrt::Windows::UI::Composition::ContainerVisual;
using DesktopWindowTarget = winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget;

static void DrawSmthng(Compositor const& compositor, ContainerVisual const& root)
{
    auto shape = compositor.CreateShapeVisual();
    shape.Size({ 400.0f,400.0f });
    
    auto rcGeometry = compositor.CreateRectangleGeometry();
    rcGeometry.Size({ 500.0f, 500.0f });

    auto rcShape = compositor.CreateSpriteShape(rcGeometry);
    rcShape.FillBrush(compositor.CreateColorBrush(UI_CLR_Background));
    rcShape.Offset({ 100.0f, 100.0f });

    shape.Shapes().Append(rcShape);
    root.Children().InsertAtTop(shape);
}

struct CTestWinUIDlg: CDialogImpl<CTestWinUIDlg, CWindow>,
                      WTL::CDialogResize<CTestWinUIDlg>
{
    using  Thiz = CTestWinUIDlg;
    using Super = CDialogImpl<CTestWinUIDlg, CWindow>;

    enum : LONG
    {
        IDD = IDD_STD_CONTROLS,
        DLG_WIDHT = 1200,
        DLG_HEIGHT = 650,
    };

    NormWindow m_normal {};

    void PrepareWindow()
    {
        CRect rcWin;
        GetWindowRect(rcWin);
        rcWin.right = rcWin.left + DLG_WIDHT;
        rcWin.bottom = rcWin.top + DLG_HEIGHT;
        MoveWindow(rcWin, TRUE);

        m_normal.dwExStyle |= WS_EX_COMPOSITED | WS_EX_LAYERED;
        m_normal.MakeItNorm(m_hWnd);

        MoveToMonitor{}.Move(m_hWnd, 3 /*MoveToMonitor::FirstNotPrimary*/, PutAt::YCenter | PutAt::XCenter);
    }

    BEGIN_MSG_MAP(CAboutDlg)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    // Composition --------------------------------------------------
        MESSAGE_HANDLER(WM_DPICHANGED, OnDPIChanged)
    END_MSG_MAP()

    LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        WORD notifyCode = HIWORD(wParam);
        WORD      cmdId = LOWORD(wParam);
        HWND    hCtlWnd = reinterpret_cast<HWND>(lParam);
        switch (cmdId) {
        case IDCANCEL:
            OnCloseCmd(notifyCode, cmdId, hCtlWnd, bHandled);
            break;
        default:
            DebugThreadPrintf(LTH_WM_COMMAND L" Unknown: n:%04d c:%04d w:%08x\n", notifyCode, cmdId, hCtlWnd);
        }
        return 0;
    }    

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        PrepareWindow();
        ShowWindow(SW_SHOW);

        if (1) {
            EnableNonClientDpiScaling(m_hWnd);
            m_currentDpi = GetDpiForWindow(m_hWnd);

            PrepareVisuals(m_compositor);
            OnNewScale(m_currentDpi, nullptr);

            auto container = m_root.as<ContainerVisual>();
            DrawSmthng(m_compositor, container);
        }
        return TRUE;
    }

    // Composition --------------------------------------------------
    struct StaticInit
    {
        using DispatcherQueueController = winrt::Windows::System::DispatcherQueueController;
        DispatcherQueueController m_controller;

        static StaticInit& init()
        {
            static StaticInit inst;
            return inst;
        }

    private:
        ~StaticInit() = default;
        StaticInit()
            : m_controller{ CreateDispatcher() }
        {}

        static DispatcherQueueController CreateDispatcher()
        {
            namespace abi = ABI::Windows::System;
            DispatcherQueueOptions options
            {
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_CURRENT,
                DQTAT_COM_STA
            };
            DispatcherQueueController controller{ nullptr };
            winrt::check_hresult(CreateDispatcherQueueController(options,
                    reinterpret_cast<abi::IDispatcherQueueController**>(winrt::put_abi(controller))
                ));
            return controller;
        }
    };

    StaticInit&         m_statiInit = StaticInit::init();
    Compositor         m_compositor;
    SpriteVisual             m_root { nullptr };
    DesktopWindowTarget    m_target { nullptr };
    inline static UINT m_currentDpi = 0;

    DesktopWindowTarget CreateDesktopWindowTarget(Compositor const& compositor)
    {
        namespace abi = ABI::Windows::UI::Composition::Desktop;
        auto interop = compositor.as<abi::ICompositorDesktopInterop>();
        DesktopWindowTarget target{ nullptr };
        winrt::check_hresult(interop->CreateDesktopWindowTarget(m_hWnd, true,
            reinterpret_cast<abi::IDesktopWindowTarget**>(winrt::put_abi(target))
        ));
        return target;
    }

    void PrepareVisuals(Compositor const& compositor)
    {
        m_target = CreateDesktopWindowTarget(compositor);
        m_root = compositor.CreateSpriteVisual();
        m_root.RelativeSizeAdjustment({ 1.05f, 1.05f });
        m_root.Brush(compositor.CreateColorBrush({ 0xFF, 0xFF, 0xFF , 0xFF }));
        m_target.Root(m_root);
    }

    LRESULT OnNewScale(UINT uDpi, PRECT prcNewScale)
    {
        auto scaleFactor = static_cast<float>(uDpi) / 100.f;
        if (nullptr != m_root && scaleFactor > 0) {
            m_root.Scale({ scaleFactor, scaleFactor, 1.0 });
        }
        return 0;
    }

    LRESULT OnDPIChanged(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
    {
        UINT uDpi = HIWORD(wParam);

        // Resize the window
        PRECT prcNewScale = reinterpret_cast<PRECT>(lParam);
        SetWindowPos(nullptr, prcNewScale->left, prcNewScale->top,
            prcNewScale->right - prcNewScale->left,
            prcNewScale->bottom - prcNewScale->top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        return OnNewScale(uDpi, prcNewScale);
    }
};

struct TestWinUI : ::testing::Test
{
};

TEST_F(TestWinUI, Simple)
{
    CTestWinUIDlg dlg;
    auto rv = dlg.DoModal();
    if (rv <= 0) {
        PrintLastError("Error in 'CTestWinUIDlg::DoModal'");
    }
}
