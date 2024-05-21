#pragma once

#include <wcdafx.api.h>
#include <atlapp.h>

WCDAFX_API WTL::CAppModule* AppModulePtr();

WCDAFX_API void RegisterAppModule(WTL::CAppModule& wtlAppModule);
WCDAFX_API void UnregisterAppModule();

struct CScopedAppRegistrator
{
    ~CScopedAppRegistrator();
    CScopedAppRegistrator(WTL::CAppModule& wtlAppModule);
};

inline CScopedAppRegistrator::~CScopedAppRegistrator()
{
    UnregisterAppModule();
}

inline CScopedAppRegistrator::CScopedAppRegistrator(WTL::CAppModule& wtlAppModule)
{
    RegisterAppModule(wtlAppModule);
}
