#include "stdafx.h"
#include "CAppModuleRef.h"

static WTL::CAppModule*& AppModulePtrHolder()
{
    static WTL::CAppModule* gps_wtlAppModule{nullptr};
    return gps_wtlAppModule;
}

WTL::CAppModule* AppModulePtr()
{
    return AppModulePtrHolder();
}

void RegisterAppModule(WTL::CAppModule& wtlAppModule)
{
    AppModulePtrHolder() = &wtlAppModule;
}

void UnregisterAppModule()
{
    AppModulePtrHolder() = nullptr;
}
