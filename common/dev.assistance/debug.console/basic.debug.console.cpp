#include "stdafx.h"
#include "basic.debug.console.h"
#include "debug.console.h"
#include <trect.h>
#include <string>
#include <iostream>

namespace DH
{
    BasicDebugConsole::~BasicDebugConsole() = default;
    BasicDebugConsole::Parameters::~Parameters() = default;

    BasicDebugConsole::BasicDebugConsole(DebugConsole const& owner)
        : params_("Courier", 12, cf::put_at::right | cf::put_at::bottom, 600, 240)
        , consoleHandle_(nullptr)
        , coutListener_(owner, std::cout)
        , cerrListener_(owner, std::cerr)
        , wcoutListener_(owner, std::wcout)
        , wcerrListener_(owner, std::wcerr)
        , debugOutputListener_(owner)
    {
    }

    HRESULT BasicDebugConsole::PreCreateWindow()
    {
        static ATOM gs_bdcAtom{0};
        return CCustomControl::PreCreateWindowImpl(gs_bdcAtom, GetWndClassInfo());
    }
    
    void BasicDebugConsole::PutWindow()
    {
        typedef cf::rect<LONG> myRect;

        myRect rcMy(0, 0, params_.cx, params_.cy);

        myRect const rcDesktop(0, 0,
            GetSystemMetrics(SM_CXMAXIMIZED) - GetSystemMetrics(SM_CXSIZEFRAME) * 2,
            GetSystemMetrics(SM_CYMAXIMIZED) - GetSystemMetrics(SM_CYSIZEFRAME) * 2);

        rcMy.put_into(rcDesktop, params_.align);

        CRect rc(rcMy.left(), rcMy.top(), rcMy.right(), rcMy.bottom());
        MoveWindow(rc);
    }

    void BasicDebugConsole::SetupFont()
    {
        HFONT const tempFont = ::CreateFontA(-params_.fsize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, params_.fname);

        consoleFont_.Attach(tempFont);
    }

    int BasicDebugConsole::OnCreate(LPCREATESTRUCT)
    {
        DWORD const dwStyle{GetStyle()};
        bool const bIsChild{0 != (dwStyle & WS_CHILD)};
        if (bIsChild) {
            ModifyStyle(0, WS_BORDER, SWP_FRAMECHANGED);
        }
        else {
            PutWindow();
        }
        SetupFont();
        consoleHandle_ = CreateConsole();
        DlgResize_Init(false, bIsChild ? false : true);
        return 0;
    }

    void BasicDebugConsole::OnDestroyNative()
    {
        OnDestroy();
        consoleFont_.DeleteObject();
        SetMsgHandled(FALSE); 
    }

    void BasicDebugConsole::OnDestroy()
    {
    }

    void BasicDebugConsole::ReceiveStdOutput(bool on) const
    {
        coutListener_.toggle(on);
        cerrListener_.toggle(on);
        wcoutListener_.toggle(on);
        wcerrListener_.toggle(on);
    }

    void BasicDebugConsole::ReceiveDebugOutput(bool on, PCWSTR pszWindowName, bool bGlobal)
    {
        (on ? debugOutputListener_.Start(pszWindowName, bGlobal) : debugOutputListener_.Stop());
    }

    void BasicDebugConsole::CreateWindowIfNessesary()
    {
        if (!m_hWnd) {
            Create(nullptr, CWindow::rcDefault, ::GetCommandLine(), WS_OVERLAPPEDWINDOW, WS_EX_TOOLWINDOW);
        }
    }

    BasicDebugConsole::Parameters& BasicDebugConsole::GetParameters()
    {
        return params_;
    }

    void BasicDebugConsole::SetParameters(int cx, int cy, int align, int fsize, char const* fname)
    {
        if ((cx != params_.cx) || (cy != params_.cy) || (align != params_.align))
        {
            params_.cx = cx;
            params_.cy = cy;
            params_.align = align;
            PutWindow();
        }

        std::string const newFontName(fname);
        std::string const oldFontName(params_.fname);

        if ((newFontName != oldFontName) || (fsize != params_.fsize)) {
            params_.fname = fname;
            params_.fsize = fsize;
            SetupFont();
        }
    }

    void BasicDebugConsole::Puts(char const* string)
    {
        {
            std::lock_guard<std::mutex> lk(cacheMx_);
            cache_.push_back(std::make_pair(string, L""));
        }
        ::PostMessage(m_hWnd, WM_SYNC_STRINGS, 0, 0);
    }

    void BasicDebugConsole::Puts(wchar_t const* string)
    {
        {
            std::lock_guard<std::mutex> lk(cacheMx_);
            cache_.push_back(std::make_pair("", string));
        }
        ::PostMessage(m_hWnd, WM_SYNC_STRINGS, 0, 0);
    }

    void BasicDebugConsole::Clean() const
    {
        ::SetWindowText(consoleHandle_, _T(""));
    }

    LRESULT BasicDebugConsole::OnSyncStrings(UINT /* = 0 */, WPARAM /* = 0 */, LPARAM /* = 0 */)
    {
        LoadStringsFromCache();
        return 0;
    }

    void BasicDebugConsole::OnCommand(UINT, int, HWND)
    {
    }

    void BasicDebugConsole::PreWrite()
    {
    }

    void BasicDebugConsole::WriteString(char const*)
    {
    }

    void BasicDebugConsole::WriteString(wchar_t const*)
    {
    }

    void BasicDebugConsole::PostWrite()
    {
    }

    void BasicDebugConsole::LoadStringsFromCache()
    {
        std::lock_guard<std::mutex> lk(cacheMx_);
        while (!cache_.empty()) {
            StringPair const& sp = cache_.front();
            PreWrite();
            if (sp.first.empty()) {
                WriteString(sp.second.c_str());
            }
            else {
                WriteString(sp.first.c_str());
            }
            PostWrite();
            cache_.pop_front();
        }
    }

    std::string BasicDebugConsole::GenerateLogFilename()
    {
        char moduleName[2048] = {0};
        ::GetModuleFileNameA(nullptr, moduleName, _countof(moduleName)-1);

        std::string logFilename = moduleName;
        std::string::size_type l = logFilename.length();
        std::string::size_type n = logFilename.rfind('\\');
        if ( std::string::npos != n )
        {
            ++n;
            logFilename = logFilename.substr(n, l-n);
        }

        l = logFilename.length();
        n = logFilename.rfind('.');
        if ( std::string::npos != n )
        {
            logFilename = logFilename.substr(0, l-(l-n));
        }

        __time64_t ts = ::_time64(&ts);
        tm localTime = {0};
        if (0 == ::_localtime64_s(&localTime, &ts))
        {
            char timeStr[256] = {0};
            const size_t rv = ::strftime(timeStr, _countof(timeStr), "%d_%m_%Y__%H_%M_%S", &localTime);
            if (rv > 0)
            {
                logFilename += ".";
                logFilename += timeStr;
            }
        }

        char tickCountStr[256] = {0};
        if (0 == ::_i64toa_s(::GetTickCount(), tickCountStr, _countof(tickCountStr), 10)) {
            logFilename += "_";
            logFilename += tickCountStr;
        }

        logFilename += ".txt";
        return logFilename;
    }

    void BasicDebugConsole::AskPathAndSave() const
    {
        WTL::CFolderDialog dlg(m_hWnd, _T("Saving to file..."));

        TCHAR modulePath[2048] = {0};
        ::GetModuleFileName(nullptr, modulePath, _countof(modulePath)-1);
        dlg.SetInitialFolder(modulePath, true);

        const INT_PTR rv = dlg.DoModal(m_hWnd);
        if ((IDOK == rv) && (nullptr != dlg.m_pidlSelected)) {
            char targetDir[MAX_PATH] = {0};
            ::SHGetPathFromIDListA(dlg.m_pidlSelected, targetDir);

            std::string logFilenPath = targetDir;
            logFilenPath += "\\";
            logFilenPath += GenerateLogFilename();
            
            Save(logFilenPath.c_str());
        }
    }

    void BasicDebugConsole::Save(char const*) const
    {
    }
}
