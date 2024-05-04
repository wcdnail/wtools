#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include "settings.h"
#include <atlcomcli.h>
#include <atlapp.h>
#include <mutex>

#define SHELL32_PATHNAME L"%SYSTEMROOT%\\System32\\shell32.dll"

using IDesktopWallpaperPtr = ATL::CComPtr<IDesktopWallpaper>;

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

class CLUIApp: public CAppModule
{
public:
    using Super = CAppModule;

    static CLUIApp* App();

    ~CLUIApp() override;
    CLUIApp();

    Conf::Section& GetSettings();
    CTheme& CurrentTheme() const;
    HICON GetIcon(int icon) const;
    CMenu const& GetTestMenu() const;
    WTL::CImageListManaged const& GetImageList(int index) const;
    IDesktopWallpaper* GetWallpaperMan();
    bool ShowDesktopWallpaper() const;

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = nullptr);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

private:
    friend Super;
    friend void SetMFStatus(int status, PCWSTR format, ...);

    Conf::Section                  m_Settings;
    CMainFrame                    m_MainFrame;
    CMenu                          m_TestMenu;
    IDesktopWallpaperPtr        m_pWallpaper;
    WTL::CImageListManaged m_ImList[IL_Count];

    static CTheme g_ThemeNative;
    static CLUIApp* g_pApp;
    static std::recursive_mutex m_pAppMx;

    HRESULT ImListCreate();
    void SetMainFrameStatus(int status, ATL::CStringW&& message);
};

inline IDesktopWallpaper* CLUIApp::GetWallpaperMan()
{
    return m_pWallpaper;
}

inline bool CLUIApp::ShowDesktopWallpaper() const
{
    return false; // ##TODO: m_bShowDesktopWallpaper to settings
}
