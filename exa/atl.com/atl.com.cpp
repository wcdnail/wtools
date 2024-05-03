#include "stdafx.h"
#include "resource.h"
#include "atlcom_i.h"

class CAtlComModule: public ATL::CAtlExeModuleT<CAtlComModule>
{
public:
    DECLARE_LIBID(LIBID_atlcomLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLCOM, "{5EA23A65-FCE4-4B2F-8077-9DC5CB002C14}")
};

CAtlComModule _AtlModule;

extern "C" int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int showCmd)
{
	return _AtlModule.WinMain(showCmd);
}
