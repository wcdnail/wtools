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
    IconFloppy,
    IconEditField,
    IconHatchCross,
    IconCount
};

void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMessageBox = false, UINT mbType = MB_ICONERROR);

struct CLegacyUIConfigurator: CAppModule,
                              CMessageFilter
{
    using Super = CAppModule;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    CTheme& CurrentTheme() const;
    HICON GetIcon(int icon) const;
    CMenu const& GetMenu() const;

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

    static CLegacyUIConfigurator* App();

private:
    friend Super;

    CMainFrame m_MainFrame;
    CImageList    m_ImList;
    HACCEL     m_wAccelTab;
    CMenu       m_TestMenu;

    static CTheme g_ThemeNative;
    static CLegacyUIConfigurator* g_pApp;
    static std::recursive_mutex m_pAppMx;

    HRESULT ImListCreate();

    BOOL PreTranslateMessage(MSG* pMsg) override;
};
