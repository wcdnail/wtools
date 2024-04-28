#include "stdafx.h"
#include "basic.debug.console.h"
#include "debug.console.h"
#include <trect.h>
#include <string>
#include <iostream>

namespace Dh
{
	BasicDebugConsole::BasicDebugConsole(DebugConsole const& owner)
		: params_("Courier", 14, cf::put_at::right | cf::put_at::bottom, 600, 240)
		, coutListener_(owner, std::cout)
		, cerrListener_(owner, std::cerr)
		, wcoutListener_(owner, std::wcout)
		, wcerrListener_(owner, std::wcerr)
		, debugOutputListener_(owner)
	{
	}

	BasicDebugConsole::~BasicDebugConsole()
	{
        if (NULL != m_hWnd)
        {
            // HACK: Leak?
            m_hWnd = NULL;
        }
	}

	void BasicDebugConsole::PutWindow()
	{
		typedef cf::rect<LONG> myRect;

		myRect rcMy(0, 0, params_.cx, params_.cy);

		myRect rcDesktop(0, 0
			, GetSystemMetrics(SM_CXMAXIMIZED) - GetSystemMetrics(SM_CXSIZEFRAME) * 2
			, GetSystemMetrics(SM_CYMAXIMIZED) - GetSystemMetrics(SM_CYSIZEFRAME) * 2);

		rcMy.put_into(rcDesktop, params_.align);

		CRect rc(rcMy.left(), rcMy.top(), rcMy.right(), rcMy.bottom());
		MoveWindow(rc);
	}

	void BasicDebugConsole::SetupFont()
	{
		HFONT tempFont = ::CreateFontA(-params_.fsize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, params_.fname);

		consoleFont_.Attach(tempFont);
		::SendMessage(consoleHandle_, WM_SETFONT, (WPARAM)consoleFont_.m_hFont, MAKELPARAM(TRUE, 0));
	}

	int BasicDebugConsole::OnCreate(LPCREATESTRUCT)
	{
		PutWindow();
	    consoleHandle_ = CreateConsole();
		SetupFont();
		DlgResize_Init(false, true);
		return TRUE;
	}

    void BasicDebugConsole::OnDestroyNative()
    {
        OnDestroy();
        SetMsgHandled(FALSE); 
        consoleFont_.DeleteObject();
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

	void BasicDebugConsole::ReceiveDebugOutput(bool on) const
	{
		(on ? debugOutputListener_.Start() : debugOutputListener_.Stop());
	}

	void BasicDebugConsole::CreateWindowIfNessesary()
	{
		if (!m_hWnd)
		{
			Create(NULL, CWindow::rcDefault, ::GetCommandLine(), WS_OVERLAPPEDWINDOW, WS_EX_TOOLWINDOW);
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

		std::string newFontName(fname);
		std::string oldFontName(params_.fname);

		if ((newFontName != oldFontName) || (fsize != params_.fsize))
		{
			params_.fname = fname;
			params_.fsize = fsize;
			SetupFont();
		}
	}

    void BasicDebugConsole::Puts(char const* string)
    {
        {
            boost::mutex::scoped_lock lk(cacheMx_);
            cache_.push_back(std::make_pair(string, L""));
        }
        ::PostMessage(m_hWnd, WM_SYNC_STRINGS, 0, 0);
    }

    void BasicDebugConsole::Puts(wchar_t const* string)
    {
        {
            boost::mutex::scoped_lock lk(cacheMx_);
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
        boost::mutex::scoped_lock lk(cacheMx_);

        while (!cache_.empty())
        {
            StringPair const& sp = cache_.front();

            PreWrite();

            if (sp.first.empty())
                WriteString(sp.second.c_str());
            else
                WriteString(sp.first.c_str());

            PostWrite();

            cache_.pop_front();
        }
    }

    std::string BasicDebugConsole::GenerateLogFilename()
    {
        char moduleName[2048] = {0};
        ::GetModuleFileNameA(NULL, moduleName, _countof(moduleName)-1);

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
            size_t rv = ::strftime(timeStr, _countof(timeStr), "%d_%m_%Y__%H_%M_%S", &localTime);
            if (rv > 0)
            {
                logFilename += ".";
                logFilename += timeStr;
            }
        }

        char tickCountStr[256] = {0};
        if (0 == ::_i64toa_s(::GetTickCount(), tickCountStr, _countof(tickCountStr), 10))
        {
            logFilename += "_";
            logFilename += tickCountStr;
        }

        logFilename += ".txt";

        return logFilename;
    }

    void BasicDebugConsole::AskPathAndSave() const
    {
        CFolderDialog dlg(m_hWnd, _T("Saving to file..."));

        TCHAR modulePath[2048] = {0};
        ::GetModuleFileName(NULL, modulePath, _countof(modulePath)-1);
        dlg.SetInitialFolder(modulePath, true);

        INT_PTR rv = dlg.DoModal(m_hWnd);
        if ((IDOK == rv) && (NULL != dlg.m_pidlSelected))
        {
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