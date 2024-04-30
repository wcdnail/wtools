#pragma once

#include "wcdafx.api.h"
#include <memory>

struct LoadDLLResources;
namespace ATL { class CWindow; }

namespace CF::UI
{
    struct Compositor
    {
        WCDAFX_API virtual ~Compositor();
        WCDAFX_API Compositor();

        WCDAFX_API void EnableComposition(ATL::CWindow& slave);

        WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
    private:
        struct Impl;
        std::unique_ptr<Impl> m_iPtr;

        friend struct LoadDLLResources;

        static void GlobalInit();
        static void GlobalFree();
    };
}
