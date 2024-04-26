#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "ClAppearance_i.h"

class CClAppearanceModule : public ATL::CAtlExeModuleT< CClAppearanceModule >
{
public :
	DECLARE_LIBID(LIBID_ClAppearanceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CLAPPEARANCE, "{a189a989-5210-498e-9326-eadb11ad6d14}")
};

CClAppearanceModule _AtlModule;

extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}
