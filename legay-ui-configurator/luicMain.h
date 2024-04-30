#pragma once

#include "luicMainFrame.h"
#include <atlapp.h>

struct CLegacyUIConfigurator: CAppModule
{
    using Super = CAppModule;

    CMainFrame m_MainFrame;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    HRESULT Run(HINSTANCE instHnd, int showCmd);

private:
    friend Super;
};
