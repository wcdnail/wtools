#pragma once

#include "ClAppMainFrame.h"
#include <atlapp.h>

struct CClassicAppearance: CAppModule
{
    using Super = CAppModule;

    CMainFrame m_MainFrame;

    ~CClassicAppearance() override;
    CClassicAppearance();

    HRESULT Run(HINSTANCE instHnd, int showCmd);

private:
    friend Super;
};
