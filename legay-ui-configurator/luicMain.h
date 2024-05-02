#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include <atlapp.h>
#include <mutex>

void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox = false, UINT mbType = MB_ICONERROR);

struct CLegacyUIConfigurator: CAppModule,
                              CMessageFilter
{
    using Super = CAppModule;

    static CTheme g_ThemeNative;

    CMainFrame m_MainFrame;
    HACCEL     m_wAccelTab;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    CTheme& CurrentTheme() const;

    HRESULT Run(HINSTANCE instHnd, int showCmd);

    static CLegacyUIConfigurator* App();

private:
    friend Super;

    static CLegacyUIConfigurator* g_pApp;
    static std::recursive_mutex m_pAppMx;

    BOOL PreTranslateMessage(MSG* pMsg) override;
};
