#include "stdafx.h"
#include "debug.console.h"
#include "basic.debug.console.h"
#include "dc.rich.edit.impl.h"
#include "dc.listview.impl.h"
#include <string.utils.error.code.h>
#include <fstream>

namespace DH
{
    bool DebugConsole::SetPrivilege(HANDLE hToken, PCWSTR pszPrivilege, bool bEnable)
    {
        TOKEN_PRIVILEGES tp{0};
        LUID           luid{0};
        if (!LookupPrivilegeValueW(nullptr, pszPrivilege, &luid)) {
            return false;
        }
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr)) {
            return false;
        }
        return true;
    }

    bool DebugConsole::AdjustPrivileges() const
    {
        HRESULT       hCode{S_OK};
        ATL::CStringW sFunc{L"NONE"};
        HANDLE       handle{nullptr};
        HandlePtr    pToken{};
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"OpenProcessToken");
            goto reportError;
        }
        HandlePtr{handle, CloseHandle}.swap(pToken);

        if (!SetPrivilege(pToken.get(), SE_DEBUG_NAME, true)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"SetPrivilege('%s')", SE_DEBUG_NAME);
            goto reportError;
        }
        if (!SetPrivilege(pToken.get(), SE_CREATE_GLOBAL_NAME, true)) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"SetPrivilege('%s')", SE_CREATE_GLOBAL_NAME);
            goto reportError;
        }
        return true;
    reportError:
        auto const sMessage{Str::ErrorCode<>::SystemMessage(hCode)};
        FormatWide(dwCurrentPID, L"ERROR: %s failed: 0x%08x %s\n", sFunc.GetString(), hCode, sMessage.GetString());
        return false;
    }

#if 0
    class DebugConsoleImpl: public BasicDebugConsole
    {
    public:
        ~DebugConsoleImpl() override;
        DebugConsoleImpl(DebugConsole const& owner);

    private:
        WTL::CEdit console_;

        HWND CreateConsole() override;
        void PreWrite() override;
        void WriteNarrow(std::string& nrString, double, DWORD) override;
        void WriteWide(std::wstring& wdString, double, DWORD) override;
        void Save(char const* fileName) const override;
        void OnDestroy() override;
    };

    DebugConsoleImpl::~DebugConsoleImpl() = default;

    DebugConsoleImpl::DebugConsoleImpl(DebugConsole const& owner)
        : BasicDebugConsole(owner)
    {
    }

    HWND DebugConsoleImpl::CreateConsole()
    {
        CRect rc{};
        GetClientRect(rc);
        console_.Create(m_hWnd, rc, nullptr, 0
            | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL
            | ES_READONLY
            | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL 
            | ES_SUNKEN | ES_WANTRETURN,
            0,
            ID_LOG_CTL);
        console_.FmtLines(TRUE);
        console_.SetFont(consoleFont_.m_hFont, FALSE);
        return console_.m_hWnd;
    }

    void DebugConsoleImpl::PreWrite()
    {
        int n = console_.GetWindowTextLength();
        console_.SetSel(n, n, GetParameters().autoScroll ? TRUE : FALSE);
    }

    void DebugConsoleImpl::WriteNarrow(std::string& nrString, double, DWORD)
    {
        ::SendMessageA(console_, EM_REPLACESEL, static_cast<WPARAM>(FALSE), reinterpret_cast<LPARAM>(nrString.c_str()));
    }

    void DebugConsoleImpl::WriteWide(std::wstring& wdString, double, DWORD)
    {
        ::SendMessageW(console_, EM_REPLACESEL, static_cast<WPARAM>(FALSE), reinterpret_cast<LPARAM>(wdString.c_str()));
    }

    void DebugConsoleImpl::Save(char const* filePathName) const
    {
        std::streamsize const len = ::GetWindowTextLengthA(console_);
        if (len > 0) {
            std::string buffer(static_cast<std::string::size_type>(len) + 1, '\0');
            int const storedLen = ::GetWindowTextA(console_, &buffer[0], static_cast<int>(len));
            if (storedLen > 0) {
                std::ofstream output(filePathName);
                output << buffer.c_str() << std::flush;
            }
        }
    }

    void DebugConsoleImpl::OnDestroy()
    {
        console_.DestroyWindow();
    }
#endif

    DebugConsole& DebugConsole::Instance()
    {
        static DebugConsole instance;
        return instance;
    }

    DebugConsole::~DebugConsole() = default;

    DebugConsole::DebugConsole()
        : impl_{new DCListViewImpl(*this)}
    {
    }

    BOOL DebugConsole::SubclassWindow(HWND hWnd) const
    {
        return impl_->SubclassWindow(hWnd);
    }

    bool DebugConsole::Create(HWND hWndParent, CRect& rc, DWORD dwStyle, DWORD dwExStyle, UINT nID) const
    {
        HWND const hWnd{impl_->Create(hWndParent, rc, nullptr, dwStyle, dwExStyle, nID)};
        if (hWnd) {
            PostMessage(hWnd, BasicDebugConsole::WM_SYNC_STRINGS, 0, 0L);
        }
        return nullptr != hWnd;
    }


    DebugConsole::DebugConsole(std::unique_ptr<BasicDebugConsole>&& otherImpl)
        : impl_(std::move(otherImpl))
    {
    }

    HRESULT DebugConsole::Initialize() const
    {
        return impl_->PreCreateWindow();
    }

    void DebugConsole::ReceiveStdOutput(bool on) const
    {
        impl_->ReceiveStdOutput(on);
    }

    bool DebugConsole::ReceiveDebugOutput(PCWSTR pszWindowName, bool bGlobal) const
    {
        return impl_->ReceiveDebugOutput(pszWindowName, bGlobal);
    }

    void DebugConsole::SetParameters(int cx, int cy, int align, int fsize, char const* fname) const
    {
        impl_->CreateWindowIfNessesary();
        impl_->SetParameters(cx, cy, align, fsize, fname);
    }

    void DebugConsole::SetAutoScroll(bool on) const
    {
        impl_->GetParameters().autoScroll = on;
    }

    void DebugConsole::Show() const
    {
        impl_->CreateWindowIfNessesary();
        impl_->ShowWindow(SW_SHOW);
        impl_->UpdateWindow();
    }

    void DebugConsole::Hide() const
    {
        impl_->ShowWindow(SW_HIDE);
    }

    void DebugConsole::Clean() const
    {
        impl_->Clean();
    }

    void DebugConsole::SetTitleText(char const* string) const
    {
        ::SetWindowTextA(impl_->m_hWnd, string);
    }

    void DebugConsole::SetTitleText(wchar_t const* string) const
    {
        ::SetWindowTextW(impl_->m_hWnd, string);
    }

    void DebugConsole::PutsNarrow(std::string_view nrString, DWORD dwPID) const
    {
        impl_->PutsNarrow(nrString, dwPID);
    }

    void DebugConsole::PutsWide(std::wstring_view wdString, DWORD dwPID) const
    {
        impl_->PutsWide(wdString, dwPID);
    }

    void DebugConsole::FormatVNarrow(DWORD dwPID, std::string_view nrFormat, va_list vaList) const
    {
        impl_->FormatVNarrow(dwPID, nrFormat, vaList);
    }

    void DebugConsole::FormatVWide(DWORD dwPID, std::wstring_view wdFormat, va_list vaList) const
    {
        impl_->FormatVWide(dwPID, wdFormat, vaList);
    }

    void DebugConsole::FormatNarrow(DWORD dwPID, std::string_view nrFormat, ...) const
    {
        va_list vaList;
        va_start(vaList, nrFormat);
        impl_->FormatVNarrow(dwPID, nrFormat, vaList);
        va_end(vaList);
    }

    void DebugConsole::FormatWide(DWORD dwPID, std::wstring_view wdFormat, ...) const
    {
        va_list vaList;
        va_start(vaList, wdFormat);
        impl_->FormatVWide(dwPID, wdFormat, vaList);
        va_end(vaList);
    }

    std::wstring DebugConsole::GetStrings(std::wstring_view svSeparator) const
    {
        int count{0};
        std::wostringstream stm{};
        for (auto const& it: impl_->GetCache()) {
            if (!svSeparator.empty() && count > 0) {
                stm << svSeparator;
            }
            stm << it.GetText();
            ++count;
        }
        return stm.str();
    }

    void DebugConsole::AskPathAndSave() const
    {
        impl_->AskPathAndSave();
    }

    void DebugConsole::Save(char const* filePathName) const
    {
        impl_->Save(filePathName);
    }

    void DebugConsole::Destroy() const
    {
        if (impl_->m_hWnd)
        {
            impl_->DestroyWindow();
            impl_->m_hWnd = nullptr;
        }
    }
}
