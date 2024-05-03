// dllmain.cpp: реализация DllMain.

#include "stdafx.h"
#include "resource.h"
#include "crtmfcHandlers_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CcrtmfcHandlersModule _AtlModule;

class CcrtmfcHandlersApp : public CWinApp
{
public:

// Переопределение
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CcrtmfcHandlersApp, CWinApp)
END_MESSAGE_MAP()

CcrtmfcHandlersApp _App;

BOOL CcrtmfcHandlersApp::InitInstance()
{
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
	return CWinApp::InitInstance();
}

int CcrtmfcHandlersApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
