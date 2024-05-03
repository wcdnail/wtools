#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include "icons.dll.h"
#include "settings.h"
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

enum MFStatus: int
{
    STA_Info = 0,
    STA_Warning,
    STA_Error,
};

void SetMFStatus(int status, PCWSTR format, ...);

void ReportError(ATL::CStringA&& caption, HRESULT code, bool showMBox = false, UINT mbType = MB_ICONERROR);
void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMBox = false, UINT mbType = MB_ICONERROR);

class CLegacyUIConfigurator: public CAppModule
{
public:
    using Super = CAppModule;

    ~CLegacyUIConfigurator() override;
    CLegacyUIConfigurator();

    Conf::Section& GetSettings();
    CTheme& CurrentTheme() const;
    HICON GetIcon(int icon) const;
    CMenu const& GetTestMenu() const;
    CIconCollectionFile const& ShellIcons() const;
    CImageList const& GetImageList() const;
    void SetMainFrameStatus(int status, ATL::CStringW&& message);

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

    static CLegacyUIConfigurator* App();

private:
    friend Super;

    Conf::Section         m_Settings;
    CMainFrame           m_MainFrame;
    CImageList              m_ImList;
    CMenu                 m_TestMenu;
    CIconCollectionFile m_ShellIcons;

    static CTheme g_ThemeNative;
    static CLegacyUIConfigurator* g_pApp;
    static std::recursive_mutex m_pAppMx;

    HRESULT ImListCreate();
};

inline CIconCollectionFile const& CLegacyUIConfigurator::ShellIcons() const
{
    return m_ShellIcons;
}

inline CImageList const& CLegacyUIConfigurator::GetImageList() const
{
    return m_ImList;
}
