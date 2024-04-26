#include "pch.h"
#include "ClAppearance.h"
#include "CRectPutInto.h"

CClAppearanceModule _AtlModule;

extern "C"
int WINAPI _tWinMain(HINSTANCE instHnd, HINSTANCE, LPTSTR lpCmdLine, int showCmd)
{
	return _AtlModule.WinMain(showCmd);
}

CClAppearanceModule::~CClAppearanceModule()
{
}

CClAppearanceModule::CClAppearanceModule()
    : m_MainFrame(modInst)
{
}

HRESULT CClAppearanceModule::Run(int showCmd)
{
	CMessageLoop loop;
    //Super::AddMessageLoop(&loop);

    HRESULT rv = S_OK;

    CRect rc(0, 0, (Rc::Screen.Width() / 2.2), Rc::Screen.Height());
    Rc::PutInto(Rc::Screen, rc, Rc::Right);

	return rv;
}
