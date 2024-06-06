#include "stdafx.h"
#include "dh.trace.level.h"
#include "info/runtime.information.h"

/*
#define LTH_WM_NOTIFY       L"WM_NOTIFY"
#define LTH_WM_DRAWITEM     L"WM_DRAWITEM"
#define LTH_WM_COMMAND      L"WM_COMMAND"
#define LTH_DBG_ASSIST      L"DBG_ASSIST"
#define LTH_COLORIZER       L"Colorize"
#define LTH_COLORIZED_CTRL  L"ColorizedCtrl"
#define LTH_CONTROL         L"Control"
#define LTH_CANCEL          L"** CANCEL **"
#define LTH_MAINFRAME       L"MainFrame"
#define LTH_STATUS          L"Status"
#define LTH_SHELL_ICON      L"ShellIcon"
#define LTH_DESK_WALLPPR    L"DesktopWallpaper"
#define LTH_APPEARANCE      L"Appearance"
#define LTH_GLOBALS         L"Globals"
#define LTH_COLORPICKER     L"ColorPicker"
*/

namespace DH
{
    void StrTraceLevel(int nLevel, std::string& szLevel)
    {
        switch (nLevel) {
        case TL_Notify:  szLevel = "Notify"; break;
        case TL_Warning: szLevel = "Warning"; break;
        case TL_Error:   szLevel = "Error"; break;
        case TL_Fatal:   szLevel = "Alert"; break;
        default:
            break;
        }
    }

    void StrTraceLevel(int nLevel, std::wstring& szLevel)
    {
        switch (nLevel) {
        case TL_Notify:  szLevel = L"Notify"; break;
        case TL_Warning: szLevel = L"Warning"; break;
        case TL_Error:   szLevel = L"Error"; break;
        case TL_Fatal:   szLevel = L"Alert"; break;
        case TL_Module:  szLevel = ModuleName(); break;
        default:
            break;
        }
    }

#pragma region Level

    WString const& ModuleName()
    {
        static WString sModuleName;
        if (sModuleName.empty()) {
            auto const* pName{&Runtime::Info().Executable.Version.ProductName};
            if (pName->empty()) {
                pName = &Runtime::Info().Executable.Filename;
            }
            std::filesystem::path const tempath{*pName};
            sModuleName = tempath.filename().replace_extension().wstring();
        }
        return sModuleName;
    }

    WString const& ModuleDir()
    {
        static WString sModuleDir;
        if (sModuleDir.empty()) {
            std::filesystem::path const modPath{Runtime::Info().Executable.Directory};
            sModuleDir = modPath.native();
        }
        return sModuleDir;
    }
#pragma endregion
}
