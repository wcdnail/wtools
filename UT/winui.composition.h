#pragma once

#include <winrt/Windows.UI.Composition.Desktop.h>
#include <windows.ui.composition.interop.h>
#include <DispatcherQueue.h>

class CompositionHost
{
    winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget        m_target;
    winrt::Windows::UI::Composition::Compositor                      m_compositor;
    winrt::Windows::System::DispatcherQueueController m_dispatcherQueueController;

public:
    static CompositionHost& GetInstance();
    ~CompositionHost();

    void Initialize(HWND hwnd);
    void AddElement(float size, float x, float y);

private:
    CompositionHost();

    void EnsureDispatcherQueue();
    void CreateDesktopWindowTarget(HWND hwnd);
    void CreateCompositionRoot();
};
