#include "stdafx.h"
#include "dh.trace.level.h"
#include "info/runtime.information.h"

namespace DH
{
    void StrTraceLevel(unsigned nLevel, std::string& szLevel)
    {
        switch (nLevel & TL_LevelMask) {
        case TL_Notify:  szLevel = "Notify"; break;
        case TL_Warning: szLevel = "Warning"; break;
        case TL_Error:   szLevel = "Error"; break;
        case TL_Fatal:   szLevel = "Alert"; break;
        case TL_Module:
            ATLTRACE(_T("No string for TL_Module\n"));
            break;
        default:
            break;
        }
    }

    void StrTraceLevel(unsigned nLevel, std::wstring& szLevel)
    {
        switch (nLevel & TL_LevelMask) {
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
