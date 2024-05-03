#include "stdafx.h"
#include "wtl.frame.wnd.h"

CAppModule _Module;

int Run(LPTSTR cmdline = NULL, int cmdshow = SW_SHOWDEFAULT)
{
	CMessageLoop loop;
	_Module.AddMessageLoop(&loop);

   int rv = ERROR_CANCELLED;
   PText::FrameWnd mainframe;
   if (NULL != mainframe.Create(NULL))
   {
      mainframe.ShowWindow(cmdshow);
      rv = loop.Run();
   }

	_Module.RemoveMessageLoop();
	return rv;
}

int WINAPI _tWinMain(HINSTANCE inst, HINSTANCE, LPTSTR cmdline, int cmdshow)
{
   HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hr));
	::DefWindowProc(NULL, 0, 0, 0L);
	AtlInitCommonControls(ICC_BAR_CLASSES);

	hr = _Module.Init(NULL, inst);
	ATLASSERT(SUCCEEDED(hr));

	int rv = ::Run(cmdline, cmdshow);

	_Module.Term();
	::CoUninitialize();
	return rv;
}
