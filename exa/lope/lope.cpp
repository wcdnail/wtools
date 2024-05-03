#include "stdafx.h"
#include "lope.h"
#include <dh.tracing.h>
#include <boost/property_tree/ini_parser.hpp>
#include <atlconv.h>

int APIENTRY _tWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int)
{
    int code = ERROR_BAD_ARGUMENTS;

    Dh::ThreadPrintf("STARTUP : Using sqlite v. %s\n", ::sqlite3_libversion());

    try
    {
        boost::property_tree::ptree opts;
        boost::property_tree::ini_parser::read_ini("lope.ini", opts);

        Lope::App app(instance, opts);
        code = app.Run();
    }
    catch (std::exception const& ex)
    {
        ::MessageBoxA(NULL, ex.what(), "Fault!", MB_ICONEXCLAMATION);
    }

    return code;
}

namespace Lope
{
    App::App(HINSTANCE instance, boost::property_tree::ptree const& opts)
        : InitOle()
        , Automation()
        , UserNotify()
        , CurrentElement()
        , CurrentPoint()
        , Translator()
        , TransMx()
        , TranslatorThread(&App::TranslateProc, this)
        , TranslateThreadDone(::CreateEvent(NULL, FALSE, FALSE, NULL), CloseHandle)
        , MainIcon(::LoadIcon(instance, MAKEINTRESOURCE(IDR_MAINFRAME)))
        , InterceptMode(opts.get<int>("Lope.InterceptMode"))
        , LoopTimeout(opts.get<unsigned>("Lope.LoopTimeout"))
        , Strings(opts)
        , SourceLang(opts.get<std::string>("Translator.SourceLang"))
        , TargetLang(opts.get<std::string>("Translator.TargetLang"))
    {
        HRESULT hr = Automation.CoCreateInstance(CLSID_CUIAutomation);

        if (!Automation)
        {
            CStringA message;
            message.Format("Creating IUIAutomation failed - %s", Str::ErrorCode<char>::SystemMessage(hr));
            throw std::runtime_error(message);
        }

        IUserNotificationPtr userNotify;
        IUserNotification2Ptr userNotify2;

        hr = userNotify2.CoCreateInstance(CLSID_UserNotification);
        if (userNotify2)
            UserNotify = userNotify2;

        else
        {
            Dh::ThreadPrintf("LOPECTOR: Creating IUserNotification2 failed - %s\n", Str::ErrorCode<char>::SystemMessage(hr));

            hr = userNotify.CoCreateInstance(CLSID_UserNotification);
            if (userNotify)
                UserNotify = userNotify2;
            else
                Dh::ThreadPrintf("LOPECTOR: Creating IUserNotification failed - %s\n", Str::ErrorCode<char>::SystemMessage(hr));
        }

        StateIcon[0].LoadIcon(IDI_RB);
        StateIcon[1].LoadIcon(IDI_GB);
        StateIcon[2].LoadIcon(IDI_BB);
        StateIcon[3].LoadIcon(IDI_OB);

        if (opts.get<int>("Proxy.Use"))
        {
            Translator.SetProxy(opts.get<std::string>("Proxy.Address").c_str()
                              , opts.get<unsigned>("Proxy.Type")
                              , opts.get<std::string>("Proxy.User").c_str()
                              );
        }

#ifdef _EXTRA_DEBUG
        Translator.SetVerbose(true);
        Translator.SetTrace();
#endif
    }

    int App::Run()
    {
        if (!Create(NULL, CRect()))
        {
            HRESULT hr = ::GetLastError();

            CStringA message;
            message.Format("Creating App frame failed - %s", Str::ErrorCode<char>::SystemMessage(hr));

            throw std::runtime_error(message);
        }

        InstallIcon(Strings.Init, StateIcon[0], IDR_MAINFRAME);

        if (UserNotify)
        {
            HRESULT hr = UserNotify->SetIconInfo(StateIcon[2], L"");
            hr = UserNotify->SetBalloonInfo(L"Title: Trololo...", L"Text: Trololo...", 0);
            hr = UserNotify->SetBalloonRetry(0, 250, 0);
            hr = UserNotify->Show(NULL, 250);
        }

        SetTrayIcon(StateIcon[1]);

        MSG message = {0};

        for (;;)
        {
            CPoint pt;
            ::GetCursorPos(&pt);
            if (CurrentPoint != pt)
            {
                CurrentPoint = pt;

                Uia::Element temp(Automation, pt);
                if (CurrentElement != temp)
                {
                    if (CurrentElement)
                        OnLeave(CurrentElement);

                    CurrentElement = temp;
                    OnEnter(CurrentElement);
                }
            }

            if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&message);
                ::DispatchMessage(&message);
            }

            if (WM_QUIT == message.message)
                break;

            ::Sleep(LoopTimeout);
        }

        return (int)message.wParam;
    }

    void App::OnExitHnd(UINT, int, HWND)
    {
        DestroyWindow();
    }

    void App::OnDestroy()
    {
        SetEvent(TranslateThreadDone.get());
        ::PostQuitMessage(0);
        SetMsgHandled(FALSE);
    }

    LRESULT App::OnMyPophint(UINT, WPARAM, LPARAM)
    {
        return 0;
    }

    void App::TranslateProc() const
    {
        while (1)
        {
            DWORD eventId = ::WaitForSingleObject(TranslateThreadDone.get(), 100);
            if (WAIT_OBJECT_0 == eventId)
                break;

            try
            {
                PopupInfoPtr infoPtr;
                {
                    boost::mutex::scoped_lock lk(PopupInfosMx);

                    if (PopupInfos.empty())
                        continue;

                    infoPtr = PopupInfos.front();
                    PopupInfos.pop_front();
                }

                std::wstring name = infoPtr->element.GetName();
                std::wstring help = infoPtr->element.GetHelpText();

                Dh::ThreadPrintf(_T("ONENTER : %04d, %04d #> %s `%s`\n"), infoPtr->element.Pt.x, infoPtr->element.Pt.y, infoPtr->element.StringDump(), name.c_str());

#if 1
                if (name.empty())
                    continue;

                {
                    boost::mutex::scoped_lock lk(TransMx);
                    infoPtr->translate = Translator.Translate(name, SourceLang.c_str(), TargetLang.c_str());
                }

                //CStringW translations;
                //for (Google::HttpTranslator::ResultPairs::const_iterator it = rm.begin(); it != rm.end(); ++it)
                //{
                //    if (!translations.IsEmpty())
                //        translations.Append(L"\r\n\r\n");

                //    translations.Format(L"`%s` - \r\n`%s`", it->first.c_str(), it->second.c_str());
                //}

                //PopupHints.AddHint(translations);
                //::PostMessage(m_hWnd, MY_POPHINT, 0, 0);
#endif
            }
            catch (std::exception const& ex)
            {
                Dh::ThreadPrintf("EXCPTION: %s\n", ex.what());
            }
        }
    }

    void App::OnEnter(Uia::Element const& elm)
    {
        //Dh::ThreadPrintf(_T("ONENTER : %04d, %04d #> %s\n"), elm.Pt.x, elm.Pt.y, elm.StringDump());

        try
        {
            boost::mutex::scoped_lock lk(PopupInfosMx);
            PopupInfos.push_front(PopupInfoPtr(new PopupInfo(elm)));
        }
        catch (std::exception const& ex)
        {
            Dh::ThreadPrintf("EXCPTION: %s\n", ex.what());
        }
    }

    void App::OnLeave(Uia::Element const& elm)
    {
        //Dh::ThreadPrintf(_T("ONLEAVE : %04d, %04d #> %s\n"), elm.Pt.x, elm.Pt.y, elm.StringDump());
    }
}
