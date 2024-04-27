#include "stdafx.h"
#include "twins.h"
#include "twins.funcs.h"
#include "command.ids.h"
#include "extern.run.h"
#include "iconoz.h"
#include <twins.lang.strings.h>
#include <dh.tracing.h>
#include <string.utils.env.vars.h>
#include <rect.putinto.h>
#include <rect.screen.h>
#include <windows.uses.ole.h>
#include <windows.uses.commoncontrols.h>
#include <windows.uses.richedit.h>
#include <dialogz.messagebox.h>
#include <boost/bind.hpp>

namespace Twins
{
    Application& App()
    {
        static Application inst;
        return inst;
    }

    Application::Application()
        : AtlModule()
        , LanguageId(MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT))
        , Icon()
        , IsMainframeMaximized(false)
        , AppRect(0, 0, 1000, 640)
        , ViewerRect(0, 0, Rc::Screen.Width() - Rc::Screen.Width()/4, Rc::Screen.Height() - Rc::Screen.Height()/4)
        , ShowHidden(false)
        , ActivePanelIndex(0)
        , ExtrnTerminal(L"Terminal", L"%COMSPEC%")
        , ExtrnAkelpad(L"AkelPad", L"%AKEL_ROOT%\\AkelPad.exe")
        , ExtrnNotepad(L"Notepad", L"notepad")
        , ShellIcons(false)
        , ShellSmallIcons(true)
        , MessageLoop()
        , AppFrame()
        , Commandline(boost::bind(&Application::RunSome, this, _1))
        , ActivePanelId(0)
        , Options(::Settings(), L"Options")
    {
        Rc::PutInto(Rc::Screen, AppRect, Rc::Center);
        Rc::PutInto(Rc::Screen, ViewerRect, Rc::Right | Rc::YCenter);

        ButtonBarId[0] = Command::Ids::Rename;
        ButtonBarId[1] = Command::Ids::ViewEntries;
        ButtonBarId[2] = Command::Ids::EditEntry;
        ButtonBarId[3] = Command::Ids::Copy;
        ButtonBarId[4] = Command::Ids::Move;
        ButtonBarId[5] = Command::Ids::CreateDirectory;
        ButtonBarId[6] = Command::Ids::DeleteToTrash;
        ButtonBarId[7] = Command::Ids::RunTerminal;

        FromSettings(Options, LanguageId);
        FromSettings(Options, IsMainframeMaximized);
        FromSettings(Options, AppRect);
        FromSettings(Options, ViewerRect);
        FromSettings(Options, ShowHidden);
        FromSettings(Options, ActivePanelIndex);
        FromSettings(Options, ButtonBarId);
        FromSettings(Options, ActivePanelId);

        Strings::Instance().Select(LanguageId);
    }

    Application::~Application()
    {}

    HRESULT Application::Run(HINSTANCE baseAddress, int showCmd)
    {
        HRESULT rv = 0;

        Initialize::OLE cole;
        Initialize::CommonControls ccontrols(ICC_COOL_CLASSES | ICC_BAR_CLASSES);
        Initialize::RichEdit richedit;

        rv = AtlModule.Init(NULL, baseAddress);
        if (FAILED(rv))
            throw std::runtime_error("CAppModule::Init failed!");

        AtlModule.AddMessageLoop(&MessageLoop);

        if (NULL == AppFrame.CreateEx())
        {
            rv = GetLastError();
            throw std::runtime_error("Main window creation failed!");
        }

        AppFrame.ShowWindow(showCmd);

        rv = MessageLoop.Run();

        AtlModule.RemoveMessageLoop();
        AtlModule.Term();

        return rv;
    }

    void Application::Exit()
    {
        AppFrame.DestroyWindow();
    }

    bool Application::RunSome(CString const& rawLine)
    {
        std::wstring commandLine = CT2W(rawLine).m_psz;
        Extern::Item it(L"FromCommandLine", commandLine);

        PCWSTR pwd = Twins::ActivePanel().GetCurrentPath();

        HRESULT hr = Twins::Extern::Run(it, pwd, L"");

#pragma message(_TODO("Configure `Run in terminal` if `ERROR_FILE_NOT_FOUND == hr`"))
        if (ERROR_FILE_NOT_FOUND == hr)
        {
            Extern::Item cmd(L"CMD", L"%COMSPEC%");

#pragma message(_TODO("Configure `/C` instead `/K`"))
            std::wstring args = L"/D /T:F8 /C \"" + commandLine + L"\"";
            hr = Twins::Extern::Run(cmd, pwd, args);
        }

        return S_OK == hr;
    }

    void Application::SetLocale(unsigned id)
    {
        LanguageId = id;
        Strings::Instance().Select(LanguageId);

        unsigned localeId = (LCID)LanguageId;

        wchar_t localeName[256] = {0};
        if (::GetLocaleInfoW(localeId, LOCALE_SLANGUAGE, localeName, _countof(localeName)-1))
        {
            ::SetThreadLocale(localeId);
            ::_wsetlocale(LC_ALL, localeName);
        }
    }
}
