#include "stdafx.h"
#include "shelltastix.mainframe.h"
#include <string.locale.h>
#include <crash.report/crash.report.h>
#include <windows.uses.com.h>
#include <windows.uses.commoncontrols.h>
#include <windows.uses.gdi+.h>
#include <windows.gui.leaks.h>
#include <locale>
#include <sstream>

void AtlThrow(HRESULT hr)
{
    std::string message;

    try
    {
        ATLTRACE(atlTraceException, 0, _T("AtlThrow: hr = 0x%x\n"), hr);

        std::stringstream temp;
        temp << std::hex << "0x" << hr << " " 
             << Str::ErrorCode<char>().SystemMessage(hr);

        message = temp.str();
    }
    catch (...)
    {
    }

    throw std::runtime_error(message);
}

static int SEHFilter(PEXCEPTION_POINTERS exinfo)
{
    if (STATUS_BREAKPOINT != exinfo->ExceptionRecord->ExceptionCode)
        return EXCEPTION_CONTINUE_EXECUTION;

    RUN_CONTEXT context = { GetCurrentProcess(), GetCurrentThread(), exinfo->ContextRecord };
    ::CRASH_REPORT_INT("Unhanded exception...", exinfo->ExceptionRecord->ExceptionCode, &context, 0);

#ifdef _DEBUG
    ::DebugBreak();
#endif 

    return EXCEPTION_EXECUTE_HANDLER;
}

static HRESULT RunApp(int showCmd)
{
    HRESULT rv = S_OK;

    try
    {
        Locale.String.Init();
        Locale.String.Select("en_US.UTF8");

        Initialize::COM initCom;
        Initialize::CommonControls initCommonControls(ICC_BAR_CLASSES);
        Initialize::GdiPlus initGdiPlus;

        CAppModule appModule;
        rv = appModule.Init(NULL, ModuleHelper::GetModuleInstance());
        if (FAILED(rv))
            throw std::runtime_error("CAppModule::Init failed!");

        CMessageLoop loop;
        appModule.AddMessageLoop(&loop);

        {
            Shelltastix::MainFrame mainFrame(loop);
            if (NULL == mainFrame.CreateEx())
            {
                rv = GetLastError();
                throw std::runtime_error("Main window creation failed!");
            }

            mainFrame.ShowWindow(showCmd);
            mainFrame.UpdateWindow();

            // FIXME: skip logo
            mainFrame.ShowLogo();

#ifdef _DEBUG_LINES
            Shelltastix::Console::View& console = mainFrame.GetConsole();
            for (int i=0; i<256; i++)
                console.WriteLine(L"Line #%d", i + 1);
#endif

            rv = loop.Run();
        }

        appModule.RemoveMessageLoop();
        appModule.Term();
    }
    catch(std::exception const& ex)
    {
        ::CRASH_REPORT(ex.what(), rv);
    }

    return rv;
}

int APIENTRY _tWinMain(HINSTANCE baseAddress, HINSTANCE, LPTSTR, int showCmd)
{
#ifndef _DEBUG
    ::SetErrorMode(SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif

    ::setlocale(LC_ALL, "");
    ::DefWindowProc(NULL, 0, 0, 0L);

    HRESULT rv = S_OK;
    __try
    {
        //Cf::GUILeaks::Instance().StartCount();
        rv = RunApp(showCmd);
    }
    __except(SEHFilter(GetExceptionInformation()))
    {
        rv = GetExceptionCode();
    }

    return (int)rv;
}
