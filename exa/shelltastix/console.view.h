#pragma once

#include "console.buffer.h"
#include "console.command.line.h"
#include <windows.uses.gdi+.h>
#include <boost/noncopyable.hpp>
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include <cstdarg>

namespace Shelltastix
{
    class MainFrame;
    class Filesystem;

    namespace Console
    {
        class View: boost::noncopyable
                  , ATL::CWindowImpl<View, ATL::CWindow>
        {
        private:
            typedef ATL::CWindowImpl<View, ATL::CWindow> Super;
            typedef Buffer::String String;
            typedef Buffer::UInt UInt;

        public:
            View(bool& posix, Filesystem& filesystem);
            ~View();

            using Super::Create;
            using Super::SetFocus;
            using Super::SetWindowText;

            bool IsVerboseOutput() const;
            void Reset();

            void NewLine();
            void WriteRaw(wchar_t const* rawString);
            void Write(String const& line);
            void Write(PCSTR format, ...);
            void Write(PCWSTR format, ...);
            void WriteLine(String const& line);
            void WriteLine(PCSTR format, ...);
            void WriteLine(PCWSTR format, ...);

            void ClearScreen();
            void FullReset();

        private:
            friend Super;
            friend class MainFrame;

            bool verbose_;
            bool lineWrap_;
            bool echo_;
            bool scroll_;
            int firstDisplayLine_;
            int displayLineCount_;
            Buffer buffer_;
            CommandLine commandLine_;
            Filesystem& filesystem_;
            Gdiplus::Font font_;
            Gdiplus::Color colorBack_;
            Gdiplus::Color colorEdge_;
            Gdiplus::Pen penEdge_;
            Gdiplus::Pen penEdgeWhenScrolling_;
            Gdiplus::SolidBrush brushBack_;
            Gdiplus::SolidBrush brushText_;
            Gdiplus::StringFormat formatText_;

            void SetupInternalCommands();
            LRESULT ToParentSync(UINT message, WPARAM wParam, LPARAM lParam);
            LRESULT OnLButtonDown(UINT message, WPARAM wParam, LPARAM lParam);
            LRESULT OnRButtonDown(UINT message, WPARAM wParam, LPARAM lParam);
            LRESULT OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam);
            LRESULT OnChar(UINT message, WPARAM wParam, LPARAM lParam);
            BOOL OnMouseWheel(UINT flags, short delta, CPoint);
            int OnCreate(LPCREATESTRUCT);
            BOOL OnEraseBkgnd(CDCHandle);
            bool GetDisplayLineCount(int& displayLineCount, Gdiplus::Graphics& gr, CRect const& rc);
            void FillBackground(CDC& dc, Gdiplus::Graphics& graphics, CRect const& rc) const;
            LRESULT OnPaint(UINT, WPARAM wParam, LPARAM lParam);
            void SyncWithViewport();
            void OnEnter();
            void ScrollByKey(UINT code);
            template <class T>
            T LastDisplayLine() const;

            BEGIN_MSG_MAP_EX(View)
                MESSAGE_HANDLER_EX(WM_LBUTTONDOWN, OnLButtonDown)
                MESSAGE_HANDLER_EX(WM_RBUTTONDOWN, OnRButtonDown)
                MESSAGE_HANDLER_EX(WM_KEYDOWN, OnKeyDown)
                MESSAGE_HANDLER_EX(WM_CHAR, OnChar)
                MSG_WM_MOUSEWHEEL(OnMouseWheel)
                MSG_WM_CREATE(OnCreate)
                MSG_WM_ERASEBKGND(OnEraseBkgnd)
                MESSAGE_HANDLER_EX(WM_PAINT, OnPaint)
            END_MSG_MAP()
        };

        inline bool View::IsVerboseOutput() const 
        {
            return verbose_; 
        }

        inline void View::Reset()
        {
            buffer_.Reset();
            SyncWithViewport();
        }

        inline void View::NewLine()
        {
            buffer_.NewLine();
        }

        inline void View::WriteRaw(wchar_t const* rawString)
        {
            buffer_.WriteRaw(rawString);
            SyncWithViewport();
        }

        inline void View::Write(String const& line)
        {
            buffer_.Write(line);
            SyncWithViewport();
        }

        inline void View::Write(PCSTR format, ...)
        {
            va_list ap;
            va_start(ap, format);
            buffer_.WriteV(format, ap);
            va_end(ap);

            SyncWithViewport();
        }

        inline void View::Write(PCWSTR format, ...)
        {
            va_list ap;
            va_start(ap, format);
            buffer_.WriteV(format, ap);
            va_end(ap);

            SyncWithViewport();
        }

        inline void View::WriteLine(String const& line)
        {
            buffer_.WriteLine(line);
            SyncWithViewport();
        }

        inline void View::WriteLine(PCSTR format, ...)
        {
            va_list ap;
            va_start(ap, format);
            buffer_.WriteLineV(format, ap);
            va_end(ap);

            SyncWithViewport();
        }

        inline void View::WriteLine(PCWSTR format, ...)
        {
            va_list ap;
            va_start(ap, format);
            buffer_.WriteLineV(format, ap);
            va_end(ap);

            SyncWithViewport();
        }
    }
}
