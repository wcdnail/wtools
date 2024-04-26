#pragma once

#include "ClAppMainFrame.h"
#include "ClAppearance_i.h"
#include <atlbase.h>

struct CClAppearanceModule;

extern CClAppearanceModule _AtlModule;

struct CClAppearanceModule: ATL::CAtlExeModuleT<CClAppearanceModule>
{
    using    Super = ATL::CAtlExeModuleT<CClAppearanceModule>;
    using SuperMod = ATL::CAtlModuleT<CClAppearanceModule>;

    CMainFrame m_MainFrame;

    ~CClAppearanceModule() override;
    CClAppearanceModule();

private:
    friend Super;
    friend SuperMod;

    DECLARE_LIBID(LIBID_ClAppearanceLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CLAPPEARANCE, "{a189a989-5210-498e-9326-eadb11ad6d14}")

    HRESULT Run(int showCmd);
};
