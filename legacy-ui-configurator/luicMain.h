#pragma once

#include "luicMainFrame.h"
#include "luicTheme.h"
#include "luicFontDef.h"
#include "settings.h"
#include <atlcomcli.h>
#include <atlapp.h>
#include <mutex>

#define SHELL32_PATHNAME L"%SYSTEMROOT%\\System32\\shell32.dll"

using IDesktopWallpaperPtr = ATL::CComPtr<IDesktopWallpaper>;

enum ImageListIndex : int
{
    IL_Own = 0,
    IL_OwnBig,
    IL_SHELL_32x32,
    IL_SHELL_16x16,
    IL_Count
};

enum IconIndex : int
{
    IconMain = 0,
    IconMyComp,
    IconAppearance,
    IconStartmenu,
    IconFloppy,
    IconEditField,
    IconHatchCross,
    IconFolderOpen,
    IconFontBold,
    IconMatreshka,
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

class CLUIApp: public WTL::CAppModule
{
public:
    using Super = CAppModule;

    static CLUIApp* App();

    ~CLUIApp() override;
    CLUIApp();

    Conf::Section& GetSettings();
    HICON GetIcon(int icon) const;
    WTL::CMenuHandle GetTestMenu() const;
    WTL::CImageListManaged const& GetImageList(int index) const;
    IDesktopWallpaper* GetWallpaperMan() const;
    bool IsThemePreviewDrawWallpaper() const;
    FontMap const& GetFontMap() const;
    CTheme& GetTheme(int nThemeIndex) const;

    HRESULT Initialize(ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = nullptr);
    HRESULT Run(HINSTANCE instHnd, int showCmd);

private:
    friend Super;
    friend void SetMFStatus(int status, PCWSTR format, ...);

    Conf::Section                  m_Settings;
    CMainFrame                    m_MainFrame;
    WTL::CMenu                     m_TestMenu;
    FontMap                         m_FontMap;
    IDesktopWallpaperPtr         m_pWallpaper;
    bool              m_bShowDesktopWallpaper;
    WTL::CImageListManaged m_ImList[IL_Count];

    static CTheme               g_CurrentTheme;
    static CLUIApp*                    g_pApp;
    static std::recursive_mutex      g_pAppMx;

    void SetMainFrameStatus(int status, ATL::CStringW&& message);
    HRESULT ImListCreate(int index, int cx, int cy);
};

inline IDesktopWallpaper* CLUIApp::GetWallpaperMan() const { return m_pWallpaper; }
inline bool          CLUIApp::IsThemePreviewDrawWallpaper() const { return m_bShowDesktopWallpaper;  } // ##TODO: m_bShowDesktopWallpaper to settings
inline FontMap const&          CLUIApp::GetFontMap() const { return m_FontMap; }
