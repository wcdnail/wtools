#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include "icons.dll.h"
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
    IconFolderOpen,
    IconCount
};

void ReportError(ATL::CStringA&& caption, HRESULT code, bool showMBox = false, UINT mbType = MB_ICONERROR);
void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMBox = false, UINT mbType = MB_ICONERROR);

struct CLegacyUIConfigurator: CAppModule,
                              CMessageFilter
{
    using Super = CAppModule;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    CTheme& CurrentTheme() const;
    HICON GetIcon(int icon) const;
    CMenu const& GetTestMenu() const;
    CIconCollectionFile const& ShellIcons() const;
    CImageList const& GetImageList() const;

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

    static CLegacyUIConfigurator* App();

private:
    friend Super;

    CMainFrame           m_MainFrame;
    CImageList              m_ImList;
    HACCEL               m_wAccelTab;
    CMenu                 m_TestMenu;
    CIconCollectionFile m_ShellIcons;

    static CTheme g_ThemeNative;
    static CLegacyUIConfigurator* g_pApp;
    static std::recursive_mutex m_pAppMx;

    HRESULT ImListCreate();

    BOOL PreTranslateMessage(MSG* pMsg) override;
};

inline CIconCollectionFile const& CLegacyUIConfigurator::ShellIcons() const
{
    return m_ShellIcons;
}

inline CImageList const& CLegacyUIConfigurator::GetImageList() const
{
    return m_ImList;
}
