#include "stdafx.h"
#include <string.utils.error.code.h>
#include <info/runtime.information.h>
#include <windows.uses.com.h>
#include <windows.uses.gdi+.h>
#include <windows.uses.commoncontrols.h>
#include <dh.tracing.h>
#include <win-5-6-7.features.h>
#include <cstdarg>
#include <clocale>
#include <atlstr.h>

namespace Ut
{
    void Main(CAppModule&, CMessageLoop&);

    void DropFailBox(PCTSTR caption, PCTSTR format, ...)
    {
        va_list ap;
        va_start(ap, format);
        CString message;
        message.FormatV(format, ap);
        va_end(ap);

        ::MessageBox(NULL, message, caption, MB_ICONSTOP);
    }

    static void WtlMain(HINSTANCE instance)
    {
        try
        {
            ::setlocale(LC_ALL, "");
            ::SetErrorMode(::SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
            ::DefWindowProc(NULL, 0, 0, 0L);

            Initialize::COM initCom;
            Initialize::GdiPlus initGdiplus;
            Initialize::CommonControls initCommonControls(ICC_BAR_CLASSES);

            CAppModule module;
            CMessageLoop loop;
            HRESULT hr = module.Init(NULL, instance);
            module.AddMessageLoop(&loop);

            Main(module, loop);

            module.RemoveMessageLoop();
            module.Term();
        }
        catch (std::exception const& ex)
        {
            DropFailBox(_T("Ошибка:"), CA2T(ex.what()));
        }
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    //std::string infoReport = Runtime::Info().SimpleReport();
    //::OutputDebugStringA(infoReport.c_str());
    Dh::ThreadPrintf(_T("AppEntry: Under Wow64? - %s\n"), Win567().UnderWow64() ? _T("Yes!") : _T("No, no no no"));
    Ut::WtlMain(instance);
    return 0;
}

