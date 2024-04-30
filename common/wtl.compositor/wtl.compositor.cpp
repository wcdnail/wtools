#include "stdafx.h"
#include "wtl.compositor.h"
#include "dh.tracing.defs.h"
#include "dev.assistance/dev.assist.h"
#include <windows.ui.composition.interop.h>
#include <winrt/windows.ui.composition.h>
#include <winrt/windows.ui.composition.desktop.h>
#include <DispatcherQueue.h>

#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Metadata.h>

//
// https://github.com/dracula/dracula-theme
//
using     UIColor = winrt::Windows::UI::Color;
using UIColorHlpr = winrt::Windows::UI::ColorHelper;

static const UIColor UI_CLR_Background  = UIColorHlpr::FromArgb(255,  40,  42,  54); // #282a36
static const UIColor UI_CLR_CurrentLine = UIColorHlpr::FromArgb(255,  68,  71,  90); // #44475a
static const UIColor UI_CLR_Selection   = UIColorHlpr::FromArgb(255,  68,  71,  90); // #44475a
static const UIColor UI_CLR_Foreground  = UIColorHlpr::FromArgb(255, 248, 248, 242); // #f8f8f2
static const UIColor UI_CLR_Comment     = UIColorHlpr::FromArgb(255,  98, 114, 164); // #6272a4
static const UIColor UI_CLR_Cyan        = UIColorHlpr::FromArgb(255, 139, 233, 253); // #8be9fd
static const UIColor UI_CLR_Green       = UIColorHlpr::FromArgb(255,  80, 250, 123); // #50fa7b
static const UIColor UI_CLR_Orange      = UIColorHlpr::FromArgb(255, 255, 184, 108); // #ffb86c
static const UIColor UI_CLR_Pink        = UIColorHlpr::FromArgb(255, 255, 121, 198); // #ff79c6
static const UIColor UI_CLR_Purple      = UIColorHlpr::FromArgb(255, 189, 147, 249); // #bd93f9
static const UIColor UI_CLR_Red         = UIColorHlpr::FromArgb(255, 255,  85,  85); // #ff5555
static const UIColor UI_CLR_Yellow      = UIColorHlpr::FromArgb(255, 241, 250, 140); // #f1fa8c

namespace CF::UI
{
    using                wuiCompositor = winrt::Windows::UI::Composition::Compositor;
    using              wuiSpriteVisual = winrt::Windows::UI::Composition::SpriteVisual;
    using           wuiContainerVisual = winrt::Windows::UI::Composition::ContainerVisual;
    using       wuiDesktopWindowTarget = winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget;
    using wuiDispatcherQueueController = winrt::Windows::System::DispatcherQueueController;

    static void DrawSmthng(wuiCompositor const& compositor, wuiContainerVisual const& root)
    {
        // Create a new ShapeVisual that will contain our drawings.
        winrt::Windows::UI::Composition::ShapeVisual
        shape = compositor.CreateShapeVisual();
        shape.Size({ 400.0f,400.0f });

        // Create a circle geometry and set its radius.
        winrt::Windows::UI::Composition::CompositionEllipseGeometry
        circleGeometry = compositor.CreateEllipseGeometry();
        circleGeometry.Radius({ 100.0f, 80.0f });

        // Create a shape object from the geometry and give it a color and offset.
        winrt::Windows::UI::Composition::CompositionSpriteShape
        circleShape = compositor.CreateSpriteShape(circleGeometry);
        circleShape.FillBrush(compositor.CreateColorBrush(UI_CLR_Selection));
        circleShape.Offset({ 100.0f, 100.0f });

        // Add the circle to our shape visual.
        shape.Shapes().Append(circleShape);

        // Add to the visual tree.
        root.Children().InsertAtTop(shape);
    }

    struct CGlobalInit
    {
        wuiDispatcherQueueController m_controller;

        static CGlobalInit& instance();
        void free();

    private:
        ~CGlobalInit();
        CGlobalInit();

        static wuiDispatcherQueueController CreateDispatcher();
    };

    CGlobalInit& CGlobalInit::instance()
    {
        static CGlobalInit inst;
        return inst;
    }

    CGlobalInit::~CGlobalInit()
    {
    }

    CGlobalInit::CGlobalInit()
        : m_controller{ CreateDispatcher() }
    {
    }

    wuiDispatcherQueueController CGlobalInit::CreateDispatcher()
    {
        namespace abi = ABI::Windows::System;
        DispatcherQueueOptions options
        {
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,
            DQTAT_COM_STA
        };
        wuiDispatcherQueueController controller{ nullptr };
        winrt::check_hresult(CreateDispatcherQueueController(options,
            reinterpret_cast<abi::IDispatcherQueueController**>(winrt::put_abi(controller))
        ));
        return controller;
    }

    void CGlobalInit::free()
    {
        //m_controller = wuiDispatcherQueueController { nullptr };
    }

    void Compositor::GlobalInit()
    {
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            UNREFERENCED_ARG(CGlobalInit::instance());
        }
        catch (winrt::hresult_error const& ex) {
            winrt::hstring msg = ex.message();
            DH::ThreadPrintfc(DH::Category::Module(), L"winrt::init_apartment FAILED 0x%08X '%s'\n", ex.code().value, msg.c_str());
        }
    }


    void Compositor::GlobalFree()
    {
        CGlobalInit::instance().free();
        winrt::uninit_apartment();
    }

    struct Compositor::Impl
    {
        // ----------------------------------------------------------------------------------
        wuiCompositor          m_compositor;
        wuiSpriteVisual              m_root;
        wuiDesktopWindowTarget     m_target;
        ATL::CWindow*              m_pSlave;

        inline static UINT     m_currentDpi = 0;

        ~Impl();
        Impl(CGlobalInit& init);

        wuiDesktopWindowTarget CreateDesktopWindowTarget(HWND hWnd) const;
        void PrepareVisuals(HWND hWnd);

        // MSG map --------------------------------------------------------------------------
        LRESULT OnNewScale(UINT uDpi, PRECT prcNewScale) const;
        LRESULT OnDPIChanged(HWND hWnd, UINT, WPARAM wParam, LPARAM lParam, BOOL&) const;
        void Run(CWindow& slave);
    };

    Compositor::Impl::~Impl()
    {
    }

    Compositor::Impl::Impl(CGlobalInit& init)
        : m_compositor{}
        ,       m_root{ nullptr }
        ,     m_target{ nullptr }
        ,     m_pSlave{ nullptr }
    {
        UNREFERENCED_ARG(init);
    }

    wuiDesktopWindowTarget Compositor::Impl::CreateDesktopWindowTarget(HWND hWnd) const
    {
        namespace abi = ABI::Windows::UI::Composition::Desktop;
        auto interop = m_compositor.as<abi::ICompositorDesktopInterop>();
        wuiDesktopWindowTarget target{ nullptr };
        winrt::check_hresult(interop->CreateDesktopWindowTarget(hWnd, true,
            reinterpret_cast<abi::IDesktopWindowTarget**>(winrt::put_abi(target))
        ));
        return target;
    }

    void Compositor::Impl::PrepareVisuals(HWND hWnd)
    {
        m_target = CreateDesktopWindowTarget(hWnd);
        m_root = m_compositor.CreateSpriteVisual();
        m_root.RelativeSizeAdjustment({ 1.05f, 1.05f });
        m_root.Brush(m_compositor.CreateColorBrush({ 0xFF, 0xFF, 0xFF , 0xFF }));
        m_target.Root(m_root);
    }

    LRESULT Compositor::Impl::OnNewScale(UINT uDpi, PRECT prcNewScale) const
    {
        auto scaleFactor = static_cast<float>(uDpi) / 100.f;
        if (nullptr != m_root && scaleFactor > 0) {
            m_root.Scale({ scaleFactor, scaleFactor, 1.0 });
        }
        return 0;
    }

    LRESULT Compositor::Impl::OnDPIChanged(HWND hWnd, UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled) const
    {
        UINT uDpi = HIWORD(wParam);
        PRECT prcNewScale = reinterpret_cast<PRECT>(lParam);
        SetWindowPos(hWnd, nullptr, 
                     prcNewScale->left, prcNewScale->top,
                     prcNewScale->right - prcNewScale->left,
                     prcNewScale->bottom - prcNewScale->top,
                     SWP_NOZORDER | SWP_NOACTIVATE
                    );
        bHandled = TRUE;
        return OnNewScale(uDpi, prcNewScale);
    }

    void Compositor::Impl::Run(CWindow& slave)
    {
        ATLASSUME(m_pSlave == nullptr);
        m_pSlave = &slave;

        EnableNonClientDpiScaling(m_pSlave->m_hWnd);
        m_currentDpi = GetDpiForWindow(m_pSlave->m_hWnd);
    
        PrepareVisuals(m_pSlave->m_hWnd);
        OnNewScale(m_currentDpi, nullptr);
    
        auto container = m_root.as<wuiContainerVisual>();
        DrawSmthng(m_compositor, container);
    }

    Compositor::~Compositor()
    {
    }

    Compositor::Compositor()
        :   m_iPtr(std::make_unique<Impl>(CGlobalInit::instance()))
    {
    }

    void Compositor::EnableComposition(ATL::CWindow& slave)
    {
        ATLASSERT(m_iPtr.get() != nullptr);
        m_iPtr->Run(slave);
    }

    BOOL Compositor::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
    {
        ATLASSERT(m_iPtr.get() != nullptr);

        UNREFERENCED_ARG(hWnd);
        UNREFERENCED_ARG(uMsg);
        UNREFERENCED_ARG(wParam);
        UNREFERENCED_ARG(lParam);
        UNREFERENCED_ARG(lResult);

        BOOL bHandled = FALSE;
        switch (uMsg) {
        case WM_DPICHANGED: {
            lResult = m_iPtr->OnDPIChanged(hWnd, uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_SIZE: {
            // ##TODO: add impl
            break;

        // ##FIXME: die for WM_PAIN/ERASEBK/CTLCOLOR... etc.. messages
        case WM_SYNCPAINT:
        case WM_NCPAINT:
        case WM_PAINT:
        case WM_ERASEBKGND:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
            bHandled = TRUE;
            break;
        }
        }
        if (bHandled) {
            return TRUE;
        }
#ifdef _DEBUG
        if (1) {
            MSG msg;
            msg.hwnd = hWnd;
            msg.message = uMsg;
            msg.wParam = wParam;
            msg.lParam = lParam;
            auto msgStr = DH::MessageToStrignW(&msg);
            DebugThreadPrintf(LTH_CONTROL L" -WM- [[ %s ]]\n", msgStr.c_str());
        }
#endif
        return FALSE;
    }
}
