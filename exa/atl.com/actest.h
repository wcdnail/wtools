#pragma once

#include "resource.h"
#include "atlcom_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ATL_NO_VTABLE CATest: public CComObjectRootEx<CComSingleThreadModel>
                          , public CComCoClass<CATest, &CLSID_ATest>
                          , public IDispatchImpl<IATest, &IID_IATest, &LIBID_atlcomLib, 1, 0>
{
public:
	CATest()
	{}

    DECLARE_REGISTRY_RESOURCEID(IDR_ACTEST)

    BEGIN_COM_MAP(CATest)
	    COM_INTERFACE_ENTRY(IATest)
	    COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{}
};

OBJECT_ENTRY_AUTO(__uuidof(ATest), CATest)
