#ifndef _DH_basic_debug_console_h__
#define _DH_basic_debug_console_h__

#include "debug.output.listener.h"
#include "ostream.listener.h"
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <deque>
#include <utility>
#include <string>

namespace Dh
{
	class DebugConsole;

	class BasicDebugConsole: public CWindowImpl<BasicDebugConsole>
		                   , public CDialogResize<BasicDebugConsole>
			               , boost::noncopyable
	{
    private:
        enum { WM_SYNC_STRINGS = WM_USER + 1 };

        typedef std::pair<std::string, std::wstring> StringPair;
        typedef std::deque<StringPair> StringQue;

	protected:
		BasicDebugConsole(DebugConsole const& owner);

	public:
		struct Parameters
		{
			Parameters(char const* fn, int fs, int a, int w, int h)
				: fname(fn)
				, fsize(fs)
				, align(a)
				, cx(w)
				, cy(h)
                , autoScroll(true)
			{
			}

			char const* fname;		// шрифт
			int fsize;				// размер шрифта
			int align;				// позиция окна на экране
			int cx;					// ширина
			int cy;					// высота
            bool autoScroll;
		};

		DECLARE_WND_CLASS_EX(_T("WCD_DH_DEBUG_CONSOLE"), CS_VREDRAW | CS_HREDRAW, (COLOR_WINDOW-1))

		enum
		{
			ID_LOG_CTL = 1000,
		};

		virtual ~BasicDebugConsole();

		void CreateWindowIfNessesary();
		void PutWindow();
		void SetupFont();

        Parameters& GetParameters();
		void SetParameters(int cx, int cy, int align, int fsize, char const* fname);

		void Puts(char const* string);
		void Puts(wchar_t const* string);
		virtual void Clean() const;

		void ReceiveStdOutput(bool on) const;
		void ReceiveDebugOutput(bool on) const;

        void AskPathAndSave() const;
        virtual void Save(char const* filePathName) const;

        static std::string GenerateLogFilename();

		BEGIN_DLGRESIZE_MAP(BasicDebugConsole)
			DLGRESIZE_CONTROL(ID_LOG_CTL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		END_DLGRESIZE_MAP()

		BEGIN_MSG_MAP_EX(BasicDebugConsole)
			MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroyNative)
            MSG_WM_COMMAND(OnCommand)
			MESSAGE_HANDLER_EX(WM_SYNC_STRINGS, OnSyncStrings)
			CHAIN_MSG_MAP(CDialogResize<BasicDebugConsole>)
		END_MSG_MAP()

	private:
		Parameters params_;
		CFont consoleFont_;
		HWND consoleHandle_;
		std_ostream_listener<char> coutListener_;
		std_ostream_listener<char> cerrListener_;
		std_ostream_listener<wchar_t> wcoutListener_;
		std_ostream_listener<wchar_t> wcerrListener_;
		mutable DebugOutputListener debugOutputListener_;
        StringQue cache_;
        boost::mutex cacheMx_;

		virtual HWND CreateConsole() = 0;

        virtual void PreWrite() = 0;
        virtual void WriteString(char const*) = 0;
        virtual void WriteString(wchar_t const*) = 0;
        virtual void PostWrite();

        virtual void OnCommand(UINT notifyCode, int id, HWND);
        virtual void OnDestroy();

		int OnCreate(LPCREATESTRUCT);
        void OnDestroyNative();
        void LoadStringsFromCache();
        LRESULT OnSyncStrings(UINT = 0, WPARAM = 0, LPARAM = 0);
	};
}

#endif // _DH_basic_debug_console_h__
