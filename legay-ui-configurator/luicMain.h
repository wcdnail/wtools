#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include <atlapp.h>
#include <mutex>

enum IconIndex : int
{
    IconMain = 0,
    IconAppearance,
    IconStartmenu,
    IconCount
};


void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox = false, UINT mbType = MB_ICONERROR);

struct CLegacyUIConfigurator: CAppModule,
                              CMessageFilter
{
    using Super = CAppModule;

    static CTheme g_ThemeNative;

    CMainFrame m_MainFrame;
    CImageList    m_ImList;
    HACCEL     m_wAccelTab;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    CTheme& CurrentTheme() const;
    HICON GetIcon(int icon) const;

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

    static CLegacyUIConfigurator* App();

private:
    friend Super;

    static CLegacyUIConfigurator* g_pApp;
    static std::recursive_mutex m_pAppMx;

    HRESULT ImListCreate();

    BOOL PreTranslateMessage(MSG* pMsg) override;
};
