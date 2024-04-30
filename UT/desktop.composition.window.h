//  ---------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All rights reserved.
// 
//  The MIT License (MIT)
// 
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
// 
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//  ---------------------------------------------------------------------------------

#pragma once
#include "pch.h"
#include <functional>
extern "C" IMAGE_DOS_HEADER __ImageBase;

using namespace winrt;
using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Composition::Desktop;

auto CreateDispatcherQueueController()
{
    namespace abi = ABI::Windows::System;
    DispatcherQueueOptions options
    {
        sizeof(DispatcherQueueOptions),
        DQTYPE_THREAD_CURRENT,
        DQTAT_COM_STA
    };
    Windows::System::DispatcherQueueController controller{nullptr};
    check_hresult(CreateDispatcherQueueController(options, reinterpret_cast<abi::IDispatcherQueueController**>(put_abi(controller))));
    return controller;
}

template <typename T>
struct DesktopWindow
{
    DesktopWindow()
    {
    }

    static T* GetThisFromHandle(const HWND window) noexcept
    {
        return reinterpret_cast<T*>(GetWindowLongPtr(window, GWLP_USERDATA));
    }

    static LRESULT __stdcall WndProc(const HWND window, const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
    {
        WINRT_ASSERT(window);
        if (WM_NCCREATE == message) {
            auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
            T* that = static_cast<T*>(cs->lpCreateParams);
            WINRT_ASSERT(that);
            WINRT_ASSERT(!that->m_window);
            that->m_window = window;
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));
            EnableNonClientDpiScaling(window);
            m_currentDpi = GetDpiForWindow(window);
        }
        else if (T* that = GetThisFromHandle(window)) {
            return that->MessageHandler(message, wparam, lparam);
        }
        return DefWindowProc(window, message, wparam, lparam);
    }

    // DPI Change handler. On WM_DPICHANGE, resize the window.
    static LRESULT HandleDpiChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
        //HWND hWndStatic = GetWindow(hWnd, GW_CHILD);
        if (hWnd != nullptr) {
            UINT uDpi = HIWORD(wParam);
            // Resize the window.
            auto lprcNewScale = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(hWnd, nullptr, lprcNewScale->left, lprcNewScale->top,
                         lprcNewScale->right - lprcNewScale->left,
                         lprcNewScale->bottom - lprcNewScale->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            if (T* that = GetThisFromHandle(hWnd)) {
                that->NewScale(uDpi);
            }
        }
        return 0;
    }

    LRESULT MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
    {
        switch (message) {
        case WM_DPICHANGED: {
            return HandleDpiChange(m_window, wparam, lparam);
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            UINT width = LOWORD(lparam);
            UINT height = HIWORD(lparam);
            mCurrentWidth = width;
            mCurrentHeight = height;
            if (T* that = GetThisFromHandle(m_window)) {
                that->DoResize(width, height);
            }
        }
        }
        return DefWindowProc(m_window, message, wparam, lparam);
    }

    void NewScale(UINT dpi)
    {
    }

    void DoResize(UINT width, UINT height)
    {
    }

protected:
    using base_type = DesktopWindow<T>;
    HWND m_window = nullptr;
    inline static UINT m_currentDpi = 0;
    int mCurrentWidth = 0;
    int mCurrentHeight = 0;
};

// Specialization of DesktopWindow that binds a composition tree to the HWND.
struct CompositionWindow : DesktopWindow<CompositionWindow>
{
    CompositionWindow(std::function<void(const Windows::UI::Composition::Compositor&, const Windows::UI::Composition::Visual&)> func) noexcept
        : CompositionWindow()
    {
        PrepareVisuals(m_compositor);
        func(m_compositor, m_root);
        NewScale(m_currentDpi);
    }

    CompositionWindow() noexcept
    {
        WNDCLASS wc{};
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        wc.lpszClassName = L"XAML island in Win32";
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        RegisterClass(&wc);
        WINRT_ASSERT(!m_window);
        WINRT_VERIFY(CreateWindow(wc.lpszClassName,
            L"Vectors in Win32",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, 850, 874,
            nullptr, nullptr, wc.hInstance, this));
        WINRT_ASSERT(m_window);
    }

    ~CompositionWindow()
    {
    }

    LRESULT MessageHandler(const UINT message, const WPARAM wparam, const LPARAM lparam) noexcept
    {
        // Handle messages here...
        return base_type::MessageHandler(message, wparam, lparam);
    }

    void NewScale(UINT dpi)
    {
        auto scaleFactor = (float)dpi / 100;
        if (m_root != nullptr && scaleFactor > 0) {
            m_root.Scale({scaleFactor, scaleFactor, 1.0});
        }
    }

    void DoResize(UINT width, UINT height)
    {
        m_currentWidth = width;
        m_currentHeight = height;
    }

    DesktopWindowTarget CreateDesktopWindowTarget(const Compositor& compositor, HWND window)
    {
        namespace abi = ABI::Windows::UI::Composition::Desktop;
        auto interop = compositor.as<abi::ICompositorDesktopInterop>();
        DesktopWindowTarget target{nullptr};
        check_hresult(interop->CreateDesktopWindowTarget(window, true,
                                                         reinterpret_cast<abi::IDesktopWindowTarget**>(winrt::put_abi(target))
                ));
        return target;
    }

    void PrepareVisuals(const Compositor& compositor)
    {
        m_target = CreateDesktopWindowTarget(compositor, m_window);
        m_root = compositor.CreateSpriteVisual();
        m_root.RelativeSizeAdjustment({1.05f, 1.05f});
        m_root.Brush(compositor.CreateColorBrush({0xFF, 0xFF, 0xFF, 0xFF}));
        m_target.Root(m_root);
    }

private:
    UINT m_currentWidth = 600;
    UINT m_currentHeight = 600;
    HWND m_interopWindowHandle = nullptr;
    Compositor m_compositor;
    DesktopWindowTarget m_target{nullptr};
    SpriteVisual m_root{nullptr};
};

using namespace winrt;
using namespace Windows::Foundation::Numerics;

///////////////////////////////////////////////////////////////////////////////////////
// Scenario 1: Construct a simple shape using ShapeVisual.
///////////////////////////////////////////////////////////////////////////////////////

static void Scenario1SimpleShape(const Compositor & compositor, const ContainerVisual & root)
{
    // Create a new ShapeVisual that will contain our drawings.
    ShapeVisual shape = compositor.CreateShapeVisual();
    shape.Size({ 400.0f,400.0f });

    // Create a circle geometry and set its radius.
    auto circleGeometry = compositor.CreateEllipseGeometry();
    circleGeometry.Radius({ 30.0f, 30.0f });

    // Create a shape object from the geometry and give it a color and offset.
    auto circleShape = compositor.CreateSpriteShape(circleGeometry);
    circleShape.FillBrush(compositor.CreateColorBrush(ColorHelper::FromArgb(255, 0, 209, 193)));
    circleShape.Offset({ 200.0f, 200.0f });

    // Add the circle to our shape visual.
    shape.Shapes().Append(circleShape);

    // Add to the visual tree.
    root.Children().InsertAtTop(shape);
}

// end Scenario 1

///////////////////////////////////////////////////////////////////////////////////////
// Scenario 2: Construct a simple path using ShapeVisual, CompositionPath, and Direct2D.
///////////////////////////////////////////////////////////////////////////////////////

// Helper function to create a GradientBrush.
Windows::UI::Composition::CompositionLinearGradientBrush CreateGradientBrush(const Compositor & compositor)
{
    auto gradBrush = compositor.CreateLinearGradientBrush();
    gradBrush.ColorStops().InsertAt(0, compositor.CreateColorGradientStop(0.0f, ColorHelper::FromArgb(255, 0, 120, 134)));
    gradBrush.ColorStops().InsertAt(1, compositor.CreateColorGradientStop(0.5f, ColorHelper::FromArgb(255, 245, 245, 245)));
    gradBrush.ColorStops().InsertAt(2, compositor.CreateColorGradientStop(1.0f, ColorHelper::FromArgb(255, 0, 209, 193)));
    return gradBrush;
}

// Helper class for converting geometry to a composition compatible geometry source.
struct GeoSource : implements<GeoSource,
    Windows::Graphics::IGeometrySource2D,
    ABI::Windows::Graphics::IGeometrySource2DInterop>
{
public:
    GeoSource(com_ptr<ID2D1Geometry> const & pGeometry) :
        _cpGeometry(pGeometry)
    { }

    IFACEMETHODIMP GetGeometry(ID2D1Geometry** value) override
    {
        _cpGeometry.copy_to(value);
        return S_OK;
    }

    IFACEMETHODIMP TryGetGeometryUsingFactory(ID2D1Factory*, ID2D1Geometry** result) override
    {
        *result = nullptr;
        return E_NOTIMPL;
    }

private:
    com_ptr<ID2D1Geometry> _cpGeometry;
};

void Scenario2SimplePath(const Compositor & compositor, const ContainerVisual & root) 
{
    // Create, size, and host a ShapeVisual.
    ShapeVisual shape = compositor.CreateShapeVisual();
    shape.Size({ 500.0f, 500.0f });
    shape.Offset({ 400.0f, 25.0f, 1.0f });

    // Create a D2D Factory
    com_ptr<ID2D1Factory> d2dFactory;
    check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.put()));

    com_ptr<ID2D1PathGeometry> path;

    // Use D2D factory to create a path geometry.
    check_hresult(d2dFactory->CreatePathGeometry(path.put()));

    // For the path created above, create a geometry sink used to add points to the path.
    com_ptr<ID2D1GeometrySink> sink;
    check_hresult(path->Open(sink.put()));

    // Add points to the path.
    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure({ 1, 1 }, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine({ 300, 300 });
    sink->AddLine({ 1, 300 });
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    
    // Close geometry sink.
    check_hresult(sink->Close());

    // Create a GeoSource helper object to wrap the path.
    CompositionPath trianglePath = CompositionPath(make<GeoSource>(path));

    // Create a CompositionPathGeometry from the composition path.
    CompositionPathGeometry compositionPathGeometry = compositor.CreatePathGeometry(trianglePath);

    // Create a SpriteShape from the CompositionPathGeometry, give it a gradient fill, and add to our ShapeVisual.
    CompositionSpriteShape spriteShape = compositor.CreateSpriteShape(compositionPathGeometry);
    spriteShape.FillBrush(CreateGradientBrush(compositor));

    // Add the SpriteShape to our shape visual.
    shape.Shapes().Append(spriteShape);

    // Add to the visual tree.
    root.Children().InsertAtTop(shape);
}

// end Scenario 2

///////////////////////////////////////////////////////////////////////////////////////
// Scenario 3: Build a morph animation using ShapeVisual, two CompositionPaths, 
// and an animated CompositionPathGeometry
///////////////////////////////////////////////////////////////////////////////////////

// Helper to build a CompositionPath using a provided callback function.
CompositionPath BuildPath(com_ptr<ID2D1Factory> const & d2dFactory, std::function<void(com_ptr<ID2D1GeometrySink> const & sink)> builder)
{
    // See scenario 2 for a more detailed explanation of the items here.
    com_ptr<ID2D1PathGeometry> path;
    check_hresult(d2dFactory->CreatePathGeometry(path.put()));
    com_ptr<ID2D1GeometrySink> sink;

    check_hresult(path->Open(sink.put()));

    builder(sink);

    check_hresult(sink->Close());

    CompositionPath trianglePath = CompositionPath(make<GeoSource>(path));
    return trianglePath;
}

// Helper to build a square CompositionPath.
CompositionPath BuildSquarePath(com_ptr<ID2D1Factory> const & d2dFactory)
{
    auto squareBuilder = [](com_ptr<ID2D1GeometrySink> const & sink) {
        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
        sink->BeginFigure({ -90, -146 }, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddBezier({ { -90.0F, -146.0F }, { 176.0F, -148.555F }, { 176.0F, -148.555F } });
        sink->AddBezier({ { 176.0F, -148.555F }, { 174.445F, 121.445F }, { 174.445F, 121.445F } });
        sink->AddBezier({ { 174.445F, 121.445F }, { -91.555F, 120.0F }, { -91.555F, 120.0F } });
        sink->AddBezier({ { -91.555F, 120.0F }, { -90.0F, -146.0F }, { -90.0F, -146.0F } });
        sink->AddLine({ 1, 300 });
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    };

    return BuildPath(d2dFactory, squareBuilder);
}

// Helper to build a circular CompositionPath.
CompositionPath BuildCirclePath(com_ptr<ID2D1Factory> const & d2dFactory)
{
    auto circleBuilder = [](com_ptr<ID2D1GeometrySink> const & sink) {
        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
        sink->BeginFigure({ 42.223F, -146 }, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddBezier({ { 115.248F, -146 }, { 174.445F, -86.13F }, { 174.445F, -12.277F } });
        sink->AddBezier({ { 174.445F, 61.576F }, { 115.248F, 121.445F }, { 42.223F, 121.445F } });
        sink->AddBezier({ { -30.802F, 121.445F }, { -90, 61.576F }, { -90, -12.277F } });
        sink->AddBezier({ { -90, -86.13F }, { -30.802F, -146 }, { 42.223F, -146 } });
        sink->AddLine({ 1, 300 });
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    };

    return BuildPath(d2dFactory, circleBuilder);
}

// Scenario 3: create morph animation
void Scenario3PathMorphImperative(const Compositor & compositor, const ContainerVisual & root) {
    // Create, size, and host a ShapeVisual.
    ShapeVisual shape = compositor.CreateShapeVisual();
    shape.Size({ 500.0f, 500.0f });
    shape.Offset({ 0.0f, 350.0f, 1.0f });

    com_ptr<ID2D1Factory> d2dFactory;
    check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.put()));

    // Call helper functions that use Win2D to build square and circle 
    // path geometries and create CompositionPaths for them.
    auto squarePath = BuildSquarePath(d2dFactory);

    auto circlePath = BuildCirclePath(d2dFactory);

    // Create a CompositionPathGeometry & CompositionSpriteShape, and set offset and fill.
    CompositionPathGeometry compositionPathGeometry = compositor.CreatePathGeometry(squarePath);
    CompositionSpriteShape spriteShape = compositor.CreateSpriteShape(compositionPathGeometry);
    spriteShape.Offset({ 150.0f, 200.0f });
    spriteShape.FillBrush(CreateGradientBrush(compositor));

    // Create a PathKeyFrameAnimation to set up the path morph, passing in the circle and square paths.
    auto playAnimation = compositor.CreatePathKeyFrameAnimation();
    playAnimation.Duration(std::chrono::seconds(4));
    playAnimation.InsertKeyFrame(0, squarePath);
    playAnimation.InsertKeyFrame(0.3F, circlePath);
    playAnimation.InsertKeyFrame(0.6F, circlePath);
    playAnimation.InsertKeyFrame(1.0F, squarePath);

    // Make animation repeat forever and start it.
    playAnimation.IterationBehavior(AnimationIterationBehavior::Forever);
    playAnimation.Direction(AnimationDirection::Alternate);
    compositionPathGeometry.StartAnimation(L"Path", playAnimation);

    // Add the SpriteShape to our shape visual.
    shape.Shapes().Append(spriteShape);

    // Add to the visual tree.
    root.Children().InsertAtTop(shape);
}

// end Scenario 3

///////////////////////////////////////////////////////////////////////////////////////
// Scenario 4: Use output from LottieViewer (https://www.microsoft.com/store/productId/9P7X9K692TMW) 
// to play back animation generated in Adobe After Effects and converted using Bodymovin.
///////////////////////////////////////////////////////////////////////////////////////

// Helper function for playing back Lottie generated animations.
ScalarKeyFrameAnimation Play(const Compositor & compositor, Visual const & visual) 
{
    auto progressAnimation = compositor.CreateScalarKeyFrameAnimation();
    progressAnimation.Duration(std::chrono::seconds(5));
    progressAnimation.IterationBehavior(AnimationIterationBehavior::Forever);
    progressAnimation.Direction(AnimationDirection::Alternate);
    auto linearEasing = compositor.CreateLinearEasingFunction();
    progressAnimation.InsertKeyFrame(0, 0, linearEasing);
    progressAnimation.InsertKeyFrame(1, 1, linearEasing);
    
    visual.Properties().StartAnimation(L"Progress", progressAnimation);
    return progressAnimation;
}

// Scenario 4.
void Scenario4PlayLottieOutput(const Compositor & compositor, const ContainerVisual & root) 
{
    // Configure a container visual.
    float width = 400.0f, height = 400.0f;
    SpriteVisual container = compositor.CreateSpriteVisual();
    container.Size({ width, height });
    container.Offset({ 400.0f, 400.0f, 1.0f });
    root.Children().InsertAtTop(container);

    // NOTE: To make this scenario compile with prerelease Microsoft.UI.Xaml package 190131001 you need to edit:
    // HelloVectors\packages\Microsoft.UI.Xaml.2.1.190131001-prerelease\build\native\Microsoft.UI.Xaml.targets
    // and change <ItemGroup Condition="'$(TargetPlatformIdentifier)' == 'UAP'"> to <ItemGroup>.

    winrt::Windows::Foundation::IInspectable diags;
    auto bmv = make<AnimatedVisuals::LottieLogo1>();
    auto avptr = bmv.TryCreateAnimatedVisual(compositor, diags);

    auto visual = avptr.RootVisual();
    container.Children().InsertAtTop(visual);

    // Calculate a scale to make the animation fit into the specified visual size.
    container.Scale({ width / avptr.Size().x, height / avptr.Size().y, 1.0f });

    auto playanimation = Play(compositor, visual);
}

// end scenario 4


static void testThisStuff()
{
    init_apartment(apartment_type::single_threaded);

    // Create a dispatcher controller required when using Windows::UI::Composition in Win32.
    auto controller = CreateDispatcherQueueController();

    // Callback that will build a visual tree containing each sample.
    auto buildvisualtree =
        [](const Compositor & compositor, const Visual & root) {

        Scenario1SimpleShape(compositor, root.as<ContainerVisual>());
        Scenario2SimplePath(compositor, root.as<ContainerVisual>());
        Scenario3PathMorphImperative(compositor, root.as<ContainerVisual>());
        Scenario4PlayLottieOutput(compositor, root.as<ContainerVisual>());
    };

    // Composition Window.
    CompositionWindow window(buildvisualtree);

    // Win32 MessageLoop
    MSG message;

    while (GetMessage(&message, nullptr, 0, 0))
    {
        DispatchMessage(&message);
    }
}
