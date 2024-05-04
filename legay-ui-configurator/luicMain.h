#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include "settings.h"
#include <atlapp.h>
#include <mutex>

#define SHELL32_PATHNAME L"%SYSTEMROOT%\\System32\\shell32.dll"

enum ImageListIndex : int
{
    IL_Own = 0,
    IL_SHELL_32x32,
    IL_SHELL_16x16,
    IL_Count
};

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
    WTL::CImageListManaged const& GetImageList(int index) const;
    void SetMainFrameStatus(int status, ATL::CStringW&& message);

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = NULL);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

    static CLegacyUIConfigurator* App();

private:
    friend Super;

    Conf::Section                  m_Settings;
    CMainFrame                    m_MainFrame;
    CMenu                          m_TestMenu;
    WTL::CImageListManaged m_ImList[IL_Count];

    static CTheme g_ThemeNative;
    static CLegacyUIConfigurator* g_pApp;
    static std::recursive_mutex m_pAppMx;

    HRESULT ImListCreate();
};
