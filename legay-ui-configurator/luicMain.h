#pragma once

#include "luicMainFrame.h"
#include <atlapp.h>

void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox = false, UINT mbType = MB_ICONERROR);

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
