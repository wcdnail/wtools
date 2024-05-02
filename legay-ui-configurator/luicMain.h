#pragma once

#include "luicMainFrame.h"
#include <atlapp.h>

void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox = false, UINT mbType = MB_ICONERROR);

struct CLegacyUIConfigurator: CAppModule,
                              CMessageFilter
{
    using Super = CAppModule;

    static CBrush g_brBackBrush;

    CMainFrame m_MainFrame;
    HACCEL     m_wAccelTab;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    HRESULT Run(HINSTANCE instHnd, int showCmd);

private:
    friend Super;

    BOOL PreTranslateMessage(MSG* pMsg) override;
};
