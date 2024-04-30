#include "pch.hxx"
#include "dh.tracing.h"
#include "err.printer.h"
#include "debug.assistance.h"
#include <gtest/gtest.h>

using          Compositor = winrt::Windows::UI::Composition::Compositor;
using        SpriteVisual = winrt::Windows::UI::Composition::SpriteVisual;
using     ContainerVisual = winrt::Windows::UI::Composition::ContainerVisual;
using DesktopWindowTarget = winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget;

static void DrawSmthng(Compositor const& compositor, ContainerVisual const& root)
{
    // Create a new ShapeVisual that will contain our drawings.
    auto shape = compositor.CreateShapeVisual();
    shape.Size({ 400.0f,400.0f });

    // Create a circle geometry and set its radius.
    auto circleGeometry = compositor.CreateEllipseGeometry();
    circleGeometry.Radius({ 30.0f, 30.0f });

    // Create a shape object from the geometry and give it a color and offset.
    auto circleShape = compositor.CreateSpriteShape(circleGeometry);
    circleShape.FillBrush(compositor.CreateColorBrush(winrt::Windows::UI::ColorHelper::FromArgb(255, 0, 209, 193)));
    circleShape.Offset({ 200.0f, 200.0f });

    // Add the circle to our shape visual.
    shape.Shapes().Append(circleShape);

    // Add to the visual tree.
    root.Children().InsertAtTop(shape);
}

struct CTestWinUIDlg: CDialogImpl<CTestWinUIDlg, CWindow>,
                      WTL::CDialogResize<CTestWinUIDlg>
{
    using  Thiz = CTestWinUIDlg;
    using Super = CDialogImpl<CTestWinUIDlg, CWindow>;

    enum : LONG
    {
        IDD = IDD_EMPTY,
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

        m_normal.MakeItNorm(m_hWnd);

        MoveToMonitor{}.Move(m_hWnd, MoveToMonitor::FirstNotPrimary, PutAt::YCenter | PutAt::XCenter);
    }

    BEGIN_MSG_MAP(CAboutDlg)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    // Composition --------------------------------------------------
        MESSAGE_HANDLER(WM_DPICHANGED, OnDPIChanged)
    END_MSG_MAP()

    LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        PrepareWindow();
        ShowWindow(SW_SHOW);

        EnableNonClientDpiScaling(m_hWnd);
        m_currentDpi = GetDpiForWindow(m_hWnd);

        PrepareVisuals(m_compositor);
        OnNewScale(m_currentDpi, nullptr);

        auto container = m_root.as<ContainerVisual>();
        DrawSmthng(m_compositor, container);
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
