#include "stdafx.h"
#include "sqlbro.main.h"
#include "sqlbro.appwindow.h"
#include <dh.tracing.h>
#include <exceptions.any.h>
#include <messagebox/ppmessagebox.h>
#include <windows.uses.ole.h>
#include <sqlite/sqlite3.h>
#include <boost/noncopyable.hpp>

namespace Sqlbro
{
    struct EntryContext: boost::noncopyable
    {
    private:
        Initialize::OLE Ole;
        CAppModule Module;
        CMessageLoop MessageLoop;
        AppWindow MainFrame;

    public:
        EntryContext(HINSTANCE instance)
            : Ole()
            , Module()
            , MessageLoop()
            , MainFrame(MessageLoop)
        {
            HRESULT hr = S_FALSE;

            ::DefWindowProc(NULL, 0, 0, 0L);
            SetLastError(0);

            INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_BAR_CLASSES };
            if (!::InitCommonControlsEx(&iccx))
            {
                hr = ::GetLastError();
                throw CrashException("::InitCommonControlsEx failed!", 0 == hr ? ERROR_BAD_ARGUMENTS : hr);
            }

            hr = Module.Init(NULL, instance, NULL);
            if (FAILED(hr))
                throw CrashException("CAppModule::Init() failed!", hr);

            if (!AtlAxWinInit())
                throw CrashException("AtlAxWinInit failed!", ::GetLastError());

            Module.AddMessageLoop(&MessageLoop);
            Dh::ThreadPrintfc(Dh::Category::Module, "ribbon %s\n", RunTimeHelper::IsRibbonUIAvailable() ? "present" : "unavailable");
        }

        ~EntryContext()
        {
            Module.RemoveMessageLoop();
            Module.Term();
        }

        static int Run(EntryContext& ctx)
        {
            ctx.MainFrame.OnRun();
            return ctx.MessageLoop.Run();
        }
    };
}

void _ShowException(char const* caption, char const* what, char const* codetext, char const* callstack)
{
    std::string message = what;

    message += std::string("\r\n") + codetext;
    message += std::string("\r\n\r\nBacktrace:") + callstack;
    message += "\r\n\r\nStopped.";

    CString text = CA2T(message.c_str(), CP_UTF8);
    CString title = CA2T(caption, CP_UTF8);

    ::PPMessageBox(NULL, text, title, MB_OK | MB_ICONEXCLAMATION, NULL);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    Dh::ThreadPrintfc(Dh::Category::Module, "sqlite v.%s\n", ::sqlite3_libversion());

    try
    {
        Sqlbro::EntryContext ctx(instance);
        return Sqlbro::EntryContext::Run(ctx);
    }
    catch (...)
    {
        Cf::HandleAnyException(_ShowException);
    }

    return -1;
}
