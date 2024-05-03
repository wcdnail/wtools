#pragma once

#include "rx/resource.h"
#include "uia.element.h"
#include <mycurl/google.translate.h>
#include <windows.uses.ole.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <atlframe.h>
#include <shobjidl.h>
#include <trayiconimpl.h>
#include <sqlite/sqlite3.h>

namespace Lope
{
    typedef CWinTraits<WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0> AppFrameTraits;

    class App: CFrameWindowImpl<App, CWindow, AppFrameTraits>
             , CTrayIconImpl<App>
    {
    private:
        typedef CFrameWindowImpl<App, CWindow, AppFrameTraits> Super;
        typedef CTrayIconImpl<App> SuperTray;

    public:
        App(HINSTANCE instance, boost::property_tree::ptree const& opts);
        int Run();

    private:
        friend class Super;
        friend class SuperTray;

        struct StringsStorage
        {
            CString Init;

            StringsStorage(boost::property_tree::ptree const& opts)
                : Init(CA2T(opts.get<std::string>("Strings.Init").c_str(), CP_UTF8))
            {}
        };

        struct PopupInfo
        {
            Uia::Element element;
            Google::HttpTranslator::ResultPairs translate;

            PopupInfo(Uia::Element const& elm)
                : element(elm)
                , translate() 
            {}
        };

        typedef boost::shared_ptr<PopupInfo> PopupInfoPtr;
        typedef std::deque<PopupInfoPtr> PopupInfoDeque;
        typedef CComPtr<IUserNotification> IUserNotificationPtr;
        typedef CComPtr<IUserNotification2> IUserNotification2Ptr;

        enum
        {
            MY_POPHINT = WM_USER + 0x600
        };

        Initialize::OLE InitOle;
        Uia::AutomationPtr Automation;
        IUserNotificationPtr UserNotify;
        Uia::Element CurrentElement;
        CPoint CurrentPoint;
        Google::HttpTranslator Translator;
        mutable boost::mutex TransMx;
        boost::thread TranslatorThread;
        boost::shared_ptr<void> TranslateThreadDone;
        mutable PopupInfoDeque PopupInfos;
        mutable boost::mutex PopupInfosMx;
        CIcon MainIcon;
        int InterceptMode;
        unsigned LoopTimeout;
        StringsStorage Strings;
        std::string SourceLang;
        std::string TargetLang;
        CIcon StateIcon[4];

        void OnEnter(Uia::Element const& elm);
        void OnLeave(Uia::Element const& elm);
        void OnDestroy();
        void OnExitHnd(UINT, int, HWND);
        void TranslateProc() const;
        LRESULT OnMyPophint(UINT, WPARAM, LPARAM);

        BEGIN_MSG_MAP_EX(App)
            MSG_WM_DESTROY(OnDestroy)
            COMMAND_ID_HANDLER_EX(ID_LOPE_EXIT, OnExitHnd)
            MESSAGE_HANDLER_EX(MY_POPHINT, OnMyPophint)
            CHAIN_MSG_MAP(SuperTray)
        END_MSG_MAP()

    private:
        App(App const&);
        App& operator = (App const&);
    };
}
