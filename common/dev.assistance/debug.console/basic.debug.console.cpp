#include "stdafx.h"
#include "basic.debug.console.h"
#include "debug.console.h"
#include <string.utils.error.code.h>
#include <dh.tracing.h>
#include <trect.h>
#include <string>
#include <iostream>

namespace DH
{
    BasicDebugConsole::~BasicDebugConsole() = default;
    BasicDebugConsole::Parameters::~Parameters() = default;

    BasicDebugConsole::BasicDebugConsole(DebugConsole const& owner)
        : params_("Liberation Mono", 11, cf::put_at::right | cf::put_at::bottom, 600, 240) // "Cascadia Mono Light"
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
        DH::Printf(0, L"ERROR", L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
        ATLASSERT(false);
        return ERROR_CALL_NOT_IMPLEMENTED;
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
            CLEARTYPE_NATURAL_QUALITY, 0, params_.fname);
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

    bool BasicDebugConsole::ReceiveDebugOutput(PCWSTR pszWindowName, bool bGlobal)
    {
        return debugOutputListener_.Start(pszWindowName, bGlobal);
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

    BasicDebugConsole::StringItem::~StringItem() = default;

    BasicDebugConsole::StringItem::StringItem(StringPair&& pair, DWORD dwPID)
        : pair_{std::move(pair)}
        ,  pid_{dwPID}
        ,   ts_{DH::LogUpTime()}
    {
        if (dwCurrentPID == pid_) {
            pid_ = GetCurrentProcessId();
        }
    }

    BasicDebugConsole::StringItem::StringItem(std::string_view nrView, DWORD dwPID)
        : StringItem(std::make_pair(std::string{nrView.data(), nrView.length()}, std::wstring{}), dwPID)
    {
    }

    BasicDebugConsole::StringItem::StringItem(std::wstring_view wdView, DWORD dwPID)
        : StringItem(std::make_pair(std::string{}, std::wstring{wdView.data(), wdView.length()}), dwPID)
    {
    }

    std::wstring BasicDebugConsole::StringItem::GetText() const
    {
        if (!pair_.second.empty()) {
            return pair_.second;
        }
        if (pair_.first.empty()) {
            return {};
        }
        ATL::CStringW const sTemp{pair_.first.c_str(), static_cast<int>(pair_.first.length())};
        return {sTemp.GetString(), static_cast<size_t>(sTemp.GetLength())};
    }

    void BasicDebugConsole::PutsNarrow(std::string_view nrView, DWORD dwPID)
    {
        {
            StringItem sItem{nrView, dwPID};
            std::lock_guard<std::recursive_mutex> lk(cacheMx_);
            cache_.emplace_back(std::move(sItem));
        }
        if (m_hWnd) {
            PostMessageW(WM_SYNC_STRINGS);
        }
    }

    void BasicDebugConsole::PutsWide(std::wstring_view wdView, DWORD dwPID)
    {
        {
            StringItem sItem{wdView, dwPID};
            std::lock_guard<std::recursive_mutex> lk(cacheMx_);
            cache_.emplace_back(std::move(sItem));
        }
        if (m_hWnd) {
            PostMessageW(WM_SYNC_STRINGS);
        }
    }

    void BasicDebugConsole::FormatVNarrow(DWORD dwPID, std::string_view nrFormat, va_list vaList)
    {
        ATL::CStringA sTemp;
        sTemp.FormatV(nrFormat.data(), vaList);
        PutsNarrow(std::string_view{sTemp.GetString(), static_cast<size_t>(sTemp.GetLength())}, dwPID);
    }

    void BasicDebugConsole::FormatVWide(DWORD dwPID, std::wstring_view nrFormat, va_list vaList)
    {
        ATL::CStringW sTemp;
        sTemp.FormatV(nrFormat.data(), vaList);
        PutsWide(std::wstring_view{sTemp.GetString(), static_cast<size_t>(sTemp.GetLength())}, dwPID);
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
        DH::Printf(0, L"ERROR", L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
        ATLASSERT(false);
    }

    void BasicDebugConsole::WriteNarrow(std::string&, double, DWORD)
    {
        DH::Printf(0, L"ERROR", L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
        ATLASSERT(false);
    }

    void BasicDebugConsole::WriteWide(std::wstring&, double, DWORD)
    {
        DH::Printf(0, L"ERROR", L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
        ATLASSERT(false);
    }

    void BasicDebugConsole::PostWrite()
    {
    }

    void BasicDebugConsole::LoadStringsFromCache()
    {
        std::lock_guard<std::recursive_mutex> lk(cacheMx_);
        while (!cache_.empty()) {
            StringItem& sp = cache_.front();
            PreWrite();
            if (sp.pair_.first.empty()) {
                WriteWide(sp.pair_.second, sp.ts_, sp.pid_);
            }
            else {
                WriteNarrow(sp.pair_.first, sp.ts_, sp.pid_);
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

    BOOL BasicDebugConsole::SubclassWindow(HWND hWnd)
    {
        BOOL const bRes{WndSuper::SubclassWindow(hWnd)};
        if (!bRes) {
            auto const hCode = static_cast<HRESULT>(GetLastError());
            DH::TPrintf(TL_Error, L"%s failed: 0x%08x %s\n", __FUNCTIONW__,
                hCode , Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return bRes;
        }
        SendMessageW(WM_CREATE);
        PostMessageW(WM_SYNC_STRINGS);
        ShowWindow(SW_SHOW);
        return bRes;
    }
}
