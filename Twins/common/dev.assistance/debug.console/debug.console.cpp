#include "stdafx.h"
#include "debug.console.h"
#include "basic.debug.console.h"
#include <fstream>

namespace Dh
{
	class DebugConsoleImpl: public BasicDebugConsole
	{
	public:
		DebugConsoleImpl(DebugConsole const& owner);
		virtual ~DebugConsoleImpl();

	private:
		CEdit console_;

		virtual HWND CreateConsole();
        virtual void PreWrite();
        virtual void WriteString(char const* string);
        virtual void WriteString(wchar_t const* string);
        virtual void Save(char const* fileName) const;
        virtual void OnDestroy();
	};

	DebugConsoleImpl::DebugConsoleImpl(DebugConsole const& owner)
		: BasicDebugConsole(owner)
	{
	}

	DebugConsoleImpl::~DebugConsoleImpl()
	{
	}

	HWND DebugConsoleImpl::CreateConsole()
	{
        CRect rc;
        GetClientRect(rc);

		console_.Create(m_hWnd, rc, NULL
			, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL
			| ES_READONLY
			| ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL 
			| ES_SUNKEN | ES_WANTRETURN
			, 0
			, ID_LOG_CTL);

		console_.FmtLines(TRUE);

		return console_.m_hWnd;
	}

	void DebugConsoleImpl::PreWrite()
	{
		int n = console_.GetWindowTextLength();
        console_.SetSel(n, n, GetParameters().autoScroll ? TRUE : FALSE);
	}

	void DebugConsoleImpl::WriteString(char const* string)
	{
		::SendMessageA(console_, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)string);
	}

	void DebugConsoleImpl::WriteString(wchar_t const* string)
	{
		::SendMessageW(console_, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)string);
	}

    void DebugConsoleImpl::Save(char const* filePathName) const
    {
        std::streamsize len = (std::streamsize)::GetWindowTextLengthA(console_);
        if (len > 0)
        {
            std::string buffer((std::string::size_type)len+1, '\0');
            int storedLen = ::GetWindowTextA(console_, &buffer[0], (int)len);
            if ( storedLen > 0 )
            {
                std::ofstream output(filePathName);
                output << buffer.c_str() << std::flush;
            }
        }
    }

    void DebugConsoleImpl::OnDestroy()
    {
        console_.DestroyWindow();
    }

	DebugConsole& DebugConsole::Instance()
	{
		static DebugConsole instance;
		return instance;
	}

    DebugConsole::DebugConsole()
        : impl_(new DebugConsoleImpl(*this))
    {
    }

	DebugConsole::DebugConsole(BasicDebugConsole *otherImpl)
		: impl_(otherImpl)
	{
	}

    DebugConsole::~DebugConsole()
    {
    }

	void* DebugConsole::GetConsoleSystemHandle() const
	{
		return impl_->m_hWnd;
	}

	void DebugConsole::ReceiveStdOutput(bool on) const
	{
		impl_->ReceiveStdOutput(on);
	}

	void DebugConsole::ReceiveDebugOutput(bool on) const
	{
		impl_->ReceiveDebugOutput(on);
	}

	void DebugConsole::SetParameters(int cx, int cy, int align, int fsize, char const* fname) const
	{
		impl_->CreateWindowIfNessesary();
		impl_->SetParameters(cx, cy, align, fsize, fname);
	}

    void DebugConsole::SetAutoScroll(bool on)
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

    void DebugConsole::Puts(char const* string) const
    {
        impl_->Puts(string);
    }

    void DebugConsole::Puts(wchar_t const* string) const
    {
        impl_->Puts(string);
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
            impl_->m_hWnd = NULL;
        }
    }
}
