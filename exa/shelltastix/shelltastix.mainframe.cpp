#include "stdafx.h"
#include "shelltastix.mainframe.h"
#include "cp.internal.commands.h"
#include "execution.state.h"
#include <crash.report/crash.report.h>
#include <debug.console.2/debug.console.2.h>
#include <rect.screen.h>
#include <rect.putinto.h>
#include <info/runtime.information.h>
#include <locale.helper.h>
#include <keyboard.extra.h>
#include <boost/bind.hpp>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi")

namespace Shelltastix
{
    enum MainFrame_Hardcoded
    {
        ID_TOOLBAR = ATL_IDW_TOOLBAR,
        ID_STATUSBAR = ATL_IDW_STATUS_BAR,
        ID_CONSOLE_VIEW = 1,

        // Timers...
        Tid_UpdateUptimeText = 1,
        Tid_UpdateUptimeTextInterval = 1000,
    };

    bool IsCompositionEnabled()
    {
        BOOL bEnabled = FALSE;
        HRESULT hr = ::DwmIsCompositionEnabled(&bEnabled);
        return SUCCEEDED(hr) && bEnabled;
    }

    CMessageLoop& MainFrame::GetMessageLoop() { return MessageLoop; }
    Console::View& MainFrame::GetConsole() { return Console; }
    Filesystem& MainFrame::GetFilesystem() { return Fs; }

    MainFrame::MainFrame(CMessageLoop& loop)
        : Posix(_USE_POSIX_BY_DEFAULT)
        , UpTime()
        , MessageLoop(loop)
        , Logo()
        , FullLogo()
        , Console(Posix, Fs)
        , Fs(Console, Posix)
    {
        SetThemeClassList(L"globals");
    }

    MainFrame::~MainFrame()
    {}

    void MainFrame::OnException(char const* exMessage /* = NULL */)
    {
        HRESULT code = GetLastError();
        ::CRASH_REPORT(exMessage, code);
        ::PostQuitMessage(code);
    }

    BOOL MainFrame::PreTranslateMessage(MSG* messageLoop)
    {
        return Super::PreTranslateMessage(messageLoop);
    }

    BOOL MainFrame::OnIdle()
    {
        return FALSE;
    }

    MainFrame::SuperResizer::_AtlDlgResizeMap const* MainFrame::GetDlgResizeMap()
    { 
        static const _AtlDlgResizeMap theMap[] = 
        {
            DLGRESIZE_CONTROL(ID_CONSOLE_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
            { -1, 0 }
        };

        return theMap;
    }

    void MainFrame::OnLButtonDown(UINT, CPoint point)
    {
        if (Keyboard::IsAltPressed())
            ::PostMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
    }

    void MainFrame::OnRButtonDown(UINT, CPoint point)
    {
        if (Keyboard::IsAltPressed())
            ::PostMessage(m_hWnd, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, MAKELPARAM(point.x, point.y));
    }

    LRESULT MainFrame::OnCompositionChanged(UINT, WPARAM wParam, LPARAM lParam)
    {
        if (IsCompositionEnabled())
        {
            MARGINS mar = {-1};
            ::DwmExtendFrameIntoClientArea(m_hWnd, &mar);
        }

        return 0;
    }

    BOOL MainFrame::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);

        dc.FillSolidRect(rc, IsCompositionEnabled() ? 0 : ::GetSysColor(COLOR_WINDOW-1));
        return TRUE;
    }

    void MainFrame::OnThemeChanged()
    {
        ::InvalidateRect(m_hWnd, NULL, TRUE);
    }

    void MainFrame::OnSetFocus(HWND)
    {
        Console.SetFocus();
    }

    void MainFrame::OnKeyDown(UINT code, UINT, UINT)
    {
        switch (code)
        {
        case VK_ESCAPE: DestroyWindow(); break;

        case (UINT)'C':
            if (Keyboard::IsCtrlPressed())
            {
                Execution().Cancel();
                Console.WriteLine(L"^C");
            }
            break;
        }
    }

    void MainFrame::OnDestroy()
    {
        ::PostQuitMessage(0); // FIXME: exiting...

        MessageLoop.RemoveMessageFilter(this);
        MessageLoop.RemoveIdleHandler(this);

        SetMsgHandled(FALSE);
    }

    void MainFrame::OnExit()
    {
        DestroyWindow();
    }

    void MainFrame::OnQuit()
    {
        DestroyWindow();
    }

    static void _AbortExecution(std::wstring const&)
    {
        ::ExitProcess(ERROR_PROCESS_ABORTED);
    }

    int MainFrame::OnCreate(LPCREATESTRUCT cs)
    {
        Command::Internals().Add(Command::Def(L"help",  L"Вывод справки: help [имя]", boost::bind(&MainFrame::ShowHelp, this, _1)));
        Command::Internals().Add(Command::Def(L"quit",  L"Выход с сохранением состояния.", boost::bind(&MainFrame::OnQuit, this)));
        Command::Internals().Add(Command::Def(L"exit",  L"Выход без всякого сохранения.", boost::bind(&MainFrame::OnExit, this)));
        Command::Internals().Add(Command::Def(L"abort", L"Незамедлительное прерывание работы.", _AbortExecution));
        Command::Internals().Add(Command::Def(L"iinfo", L"Вывод информации: info [имя]", boost::bind(&MainFrame::ShowInfo, this, _1)));
        Command::Internals().Add(Command::Def(L"posix", L"Переключатель POSIX: posix [off]", boost::bind(&MainFrame::PosixToggle, this, _1)));

        OnCompositionChanged();

        CRect rcWin(0, 0, 800, 600);

        Rc::PutInto(Rc::Screen, rcWin, 0 |
                                       Rc::Center
                                       //Rc::Bottom | Rc::Right
                                       //Rc::XCenter | Rc::Top
                                       );
        MoveWindow(rcWin, FALSE);

        CRect rc;
        GetClientRect(rc);

        CRect rcView = rc;
        rcView.DeflateRect(4, 2);
        Console.Create(m_hWnd, rcView, NULL, WS_CHILD | WS_VISIBLE, WS_EX_TRANSPARENT, ID_CONSOLE_VIEW);

        MessageLoop.AddMessageFilter(this);
        MessageLoop.AddIdleHandler(this);

        SetupLogoString();
#ifdef _DEBUG
        OnTimer(Tid_UpdateUptimeText);
#endif
        DlgResize_Init(false, false);
        return 0;
    }

    void MainFrame::SetupLogoString()
    {
        CStringW product = CA2W(Runtime::Info().Executable.Version.ProductName.c_str());
        CStringW version = CA2W(Runtime::Info().Executable.Version.ProductVersion.c_str());
        CStringW copyrights = CA2W(Runtime::Info().Executable.Version.LegalCopyright.c_str());

        CStringW temp;
        temp.Format(L"%s v. %s", (PCWSTR)product, (PCWSTR)version);
        Logo = temp;
        ::SetWindowTextW(m_hWnd, Logo);
        
        FullLogo.Format(L"%s\n%s", (PCWSTR)Logo, (PCWSTR)copyrights);
    }

    void MainFrame::ShowLogo()
    {
        Console.WriteLine(L"%s\n", (PCWSTR)FullLogo);
    }

    void MainFrame::OnTimer(UINT_PTR id)
    {
        if (Tid_UpdateUptimeText == id)
        {
            KillTimer(Tid_UpdateUptimeText);

            CStringW text;
            text.Format(L"%s (uptime: %.2f)", (PCWSTR)Logo, UpTime.Seconds());
            ::SetWindowTextW(m_hWnd, text);

            SetTimer(Tid_UpdateUptimeText, Tid_UpdateUptimeTextInterval);
        }
    }

    void MainFrame::ShowHelp(String const& topic)
    {
        if (topic.empty())
        {
            Console.WriteLine(L"\n%s\n\n%s\n", FullLogo, _LS("Краткая справка:"));

            Command::Internals().ShowHelp(Console);

            Console.WriteLine(L"\n%s\n%s\n", 
                _LS("Команды с префиксом `i` - внутренние - "),
                _LS("введены для частичной POSIX-совместимости.")
                );
        }
        else
            Console.WriteLine(L"`%s` - %s", topic.c_str(), _LS("Раздел справки отсутствует."));
    }

    void MainFrame::ShowInfo(String const& topic)
    {
        if (topic.empty())
            Console.WriteLine("\n%s\n", Runtime::Info().SimpleReport().c_str());
        else
            Console.WriteLine(L"`%s` - %s", topic.c_str(), _LS("Информация отсутствует."));
    }

    void MainFrame::PosixToggle(String const& args)
    {
        Posix = (args != L"off");
        Console.SyncWithViewport();
    }
}
