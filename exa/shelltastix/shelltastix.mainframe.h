#pragma once

#include "console.view.h"
#include "filesystem.h"
#include "cp.command.h"
#include <dh.timer.h>
#include <boost/noncopyable.hpp>
#include <atlapp.h>
#include <atlstr.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <atltheme.h>
#include "res/resource.h"

namespace Shelltastix
{
    typedef CWinTraits<WS_POPUP 
                     | WS_THICKFRAME 
                     | WS_SYSMENU
                     | WS_CAPTION
                     | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
                     | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                     , 0
                     > MainFrameTraits;

    class MainFrame: boost::noncopyable
                   , CFrameWindowImpl<MainFrame, CWindow, MainFrameTraits>
                   , CDialogResize<MainFrame>
                   , CThemeImpl<MainFrame>
                   , CMessageFilter
                   , CIdleHandler
    {
    private:
        typedef CFrameWindowImpl<MainFrame, CWindow, MainFrameTraits> Super;
        typedef CDialogResize<MainFrame> SuperResizer;
        typedef CThemeImpl<MainFrame> SuperTheme;

    public:
        typedef Command::Def::String String;

        MainFrame(CMessageLoop& loop);
        ~MainFrame();

        using Super::CreateEx;
        using Super::ShowWindow;
        using Super::UpdateWindow;

        CMessageLoop& GetMessageLoop();
        Console::View& GetConsole();
        Filesystem& GetFilesystem();

        void ShowLogo();
        void ShowHelp(String const& topic);
        void ShowInfo(String const& topic);
        void PosixToggle(String const& args);

    private:
        friend Super;
        friend SuperResizer;
        friend SuperTheme;

        bool Posix;
        Dh::Timer UpTime;
        CMessageLoop& MessageLoop;
        CStringW Logo;
        CStringW FullLogo;
        Console::View Console;
        Filesystem Fs;

        void OnException(char const* = NULL);
        virtual BOOL PreTranslateMessage(MSG*);
        virtual BOOL OnIdle();
        void OnExit();
        void OnQuit();
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle);
        void OnThemeChanged();
        void OnSetFocus(HWND);
        void OnKeyDown(UINT code, UINT, UINT);
        void OnLButtonDown(UINT, CPoint point);
        void OnRButtonDown(UINT, CPoint point);
        LRESULT OnCompositionChanged(UINT = 0, WPARAM = 0, LPARAM = 0);
        void OnTimer(UINT_PTR id);
        void SetupLogoString();

        static _AtlDlgResizeMap const* GetDlgResizeMap();

        DECLARE_FRAME_WND_CLASS_EX(_T("Shelltastix:MainFrame"), IDR_MAINFRAME, CS_VREDRAW | CS_HREDRAW, COLOR_WINDOW);

        BEGIN_MSG_MAP_EX(MainFrame)
            CHAIN_MSG_MAP(SuperTheme)
            MESSAGE_HANDLER_EX(WM_DWMCOMPOSITIONCHANGED, OnCompositionChanged)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_THEMECHANGED(OnThemeChanged)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_TIMER(OnTimer)
            CHAIN_MSG_MAP(SuperResizer)
            //CHAIN_MSG_MAP(Super)
        END_MSG_MAP()
    };
}
