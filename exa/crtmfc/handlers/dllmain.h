// dllmain.h: объ€вление класса модул€.

class CcrtmfcHandlersModule : public ATL::CAtlDllModuleT< CcrtmfcHandlersModule >
{
public :
	DECLARE_LIBID(LIBID_crtmfcHandlersLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CRTMFCHANDLERS, "{0CD978D7-2F99-4605-97BA-E6D839AAFA3C}")
};

extern class CcrtmfcHandlersModule _AtlModule;
