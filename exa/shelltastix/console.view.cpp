#include "stdafx.h"
#include "console.view.h"
#include "cp.internal.commands.h"
#include "execution.state.h"
#include <gdi.autopaint.dc.h>
#include <rect.gdi+.h>
#include <dh.timer.h>
#include <dh.tracing.h>
#include <locale.helper.h>
#include <keyboard.extra.h>
#include <generic.increment.h>
#include <atlstr.h>
#include <atlconv.h>
#include <exception>
#include <algorithm>
#include <cstdarg>
#include <boost/bind.hpp>

namespace Shelltastix
{
    extern bool IsCompositionEnabled();

    namespace Console
    {
        enum ConsoleView_Hardcoded
        {
            NoramlDraw          = 0x00000000,
            DontDrawCommandLine = 0x01000000,

            WheelScrollAmount   = 3,
        };

        View::View(bool& posix, Filesystem& filesystem) 
            : Super()
            , verbose_(true)
            , lineWrap_(false)
            , echo_(true)
            , scroll_(false)
            , firstDisplayLine_(0)
            , displayLineCount_(0)
            , buffer_()
            , commandLine_(posix)
            , filesystem_(filesystem)
            , font_(L"Terminus (TTF)", 12)
            , colorBack_(0xc01f1f5f)
            , colorEdge_(0xff000000)
            , penEdge_(colorEdge_)
            , penEdgeWhenScrolling_(Gdiplus::Color(0xff5aff5a), 4.f)
            , brushBack_(colorBack_)
            , brushText_(Gdiplus::Color(0xffdfdf9f))
            , formatText_()
        {
            formatText_.SetFormatFlags(0
                | Gdiplus::StringFormatFlagsMeasureTrailingSpaces 
                | (lineWrap_ ? 0 : Gdiplus::StringFormatFlagsNoWrap) // TODO: manage line wrap
                );
        }

        View::~View() 
        {}

        void View::SetupInternalCommands()
        {
            Command::Internals().Add(Command::Def(L"cls",   L"Очистка экрана.", boost::bind(&View::ClearScreen, this)));
            Command::Internals().Add(Command::Def(L"clear", L"Очистка экрана.", boost::bind(&View::ClearScreen, this)));
            Command::Internals().Add(Command::Def(L"zero",  L"Полный сброс.", boost::bind(&View::FullReset, this)));
        }

        LRESULT View::ToParentSync(UINT message, WPARAM wParam, LPARAM lParam)
        {
            return ::SendMessage(GetParent(), message, wParam, lParam);
        }

        LRESULT View::OnLButtonDown(UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (Keyboard::IsAltPressed())
                return ToParentSync(message, wParam, lParam);

            return 0;
        }

        LRESULT View::OnRButtonDown(UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (Keyboard::IsAltPressed())
                return ToParentSync(message, wParam, lParam);

            return 0;
        }

        LRESULT View::OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam)
        {
            UINT code = (UINT)wParam;

            scroll_ = Keyboard::IsScrollLockOn();
            if (scroll_)
            {
                ScrollByKey(code);
                return 0;
            }

            if (Execution().Busy())
                return ToParentSync(message, wParam, lParam);

            bool ctrlPressed = Keyboard::IsCtrlPressed();
           
            switch (code)
            {
            case VK_SCROLL: break;
            case VK_RETURN: OnEnter(); break;
            case VK_ESCAPE: commandLine_.Reset();  break;
            case VK_INSERT: commandLine_.ToggleInsertion(); break;
            case VK_DELETE: 
            case VK_BACK: commandLine_.Erase(1, VK_BACK == code); break;
            case VK_RIGHT:  
            case VK_LEFT: commandLine_.MoveCursor(VK_LEFT == code ? -1 : 1); break;
            case VK_UP:     
            case VK_DOWN: commandLine_.Set(commandLine_.FromHistory(VK_UP == code)); break;
            case VK_HOME:   
            case VK_END:
            {
                int len = commandLine_.GetLength();
                commandLine_.MoveCursor(VK_END == code ? len : -len); 
                break;
            }
            case (UINT)'L': 
                if (ctrlPressed)
                    ClearScreen();
                break;

            default: 
                return ToParentSync(message, wParam, lParam);
            }

            ::InvalidateRect(m_hWnd, NULL, FALSE);
            return 0;
        }

        LRESULT View::OnChar(UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (!scroll_)
            {
                UINT code = (UINT)wParam;
                if (code > 31)
                {
                    commandLine_.Append((wchar_t)code);
                    ::InvalidateRect(m_hWnd, NULL, FALSE);
                }
            }
            return 0;
        }

        int View::OnCreate(LPCREATESTRUCT)
        {
            SetupInternalCommands();
            SetFocus();
            return 0;
        }

        BOOL View::OnEraseBkgnd(CDCHandle)
        {
            return TRUE;
        }

        void View::FillBackground(CDC& dc, Gdiplus::Graphics& graphics, CRect const& rc) const
        {
            Gdiplus::Rect bkRect = ToRect(rc);
            graphics.FillRectangle(&brushBack_, bkRect);
            graphics.DrawRectangle(&penEdge_, bkRect);
        }

        bool View::GetDisplayLineCount(int& displayLineCount, Gdiplus::Graphics& gr, CRect const& rc)
        {
            Gdiplus::RectF letter;
            gr.MeasureString(L"A", 1, &font_, Gdiplus::PointF(), &formatText_, &letter);

            // TODO: Учесть line wrap
            if (letter.Height > 0)
            {
                displayLineCount = ((rc.Height() - (int)letter.Height) / (int)letter.Height);
                return true;
            }
            
            return false;
        }

        template <class T>
        T View::LastDisplayLine() const
        {
            T rv = (T)(buffer_.GetLinesCount() - displayLineCount_);
            return rv;
        }

        LRESULT View::OnPaint(UINT, WPARAM wParam, LPARAM lParam)
        {
            Gdi::AutoPaintDc autoDc(m_hWnd);
            CRect const& rc = autoDc.GetRect();
            CDC& dc = autoDc.GetValidDc();
            
            Gdiplus::Graphics graphics(dc);
            graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSingleBitPerPixelGridFit);

            FillBackground(dc, graphics, rc);

            Gdiplus::RectF bbox;
            if (GetDisplayLineCount(displayLineCount_, graphics, rc) && (displayLineCount_ > 0))
            {
                if (!scroll_)
                    firstDisplayLine_ = LastDisplayLine<int>();

                UInt textLen = 0;
                PCWSTR text = buffer_.GetDisplayString(textLen, firstDisplayLine_);
                if (textLen > 0)
                {
                    CRect rcText = rc;
                    rcText.DeflateRect(2, 2);

                    graphics.MeasureString(text, (int)textLen, &font_, ToRectF(rcText), &formatText_, &bbox);
                    graphics.DrawString(text, (int)textLen, &font_, bbox, &formatText_, &brushText_);
                }
            }

            if (!Execution().Busy())
            {
                Gdi::PaintArgs args(dc, graphics, rc, bbox, font_, formatText_, brushText_);
                commandLine_.Draw(args);
            }

            bool scroll = Keyboard::IsScrollLockOn();
            if (scroll)
            {
                // TODO: draw scroll bar...

                Gdiplus::Rect edgeRect = ToRect(rc);
                edgeRect.Inflate(-2, -2);
                graphics.DrawRectangle(&penEdgeWhenScrolling_, edgeRect);
            }
            scroll_ = scroll;

            return 0;
        }

        BOOL View::OnMouseWheel(UINT flags, short delta, CPoint)
        {
            scroll_ = true;
            int amount = delta < 0 ? WheelScrollAmount : -WheelScrollAmount;
            Generic::Increment<int>(firstDisplayLine_, amount, 0, LastDisplayLine<int>()); 
            ::InvalidateRect(m_hWnd, NULL, FALSE);
            return TRUE;
        }

        void View::ScrollByKey(UINT code)
        {
            int temp;

            switch (code)
            {
            case VK_SCROLL: /* Nothing to do... */ break;

            /* Up/down arrows */
            case VK_UP:
            case VK_DOWN:
                temp = (VK_DOWN == code ? 1 : -1);
                Generic::Increment<int>(firstDisplayLine_, temp, 0, LastDisplayLine<int>()); 
                break;

            /* Home/end */
            case VK_HOME:   firstDisplayLine_ = 0; break;
            case VK_END:    firstDisplayLine_ = LastDisplayLine<int>(); break;
            
            /* Page up/down */
            case VK_PRIOR:
            case VK_NEXT:   
                temp = (VK_NEXT == code ? displayLineCount_ : -displayLineCount_);
                Generic::Increment<int>(firstDisplayLine_, temp, 0, LastDisplayLine<int>()); 
                break;

            /* Right/left arrows */
            case VK_RIGHT:  
            case VK_LEFT:   
                // TODO: horizontal scrolling
                break;

            default: 
                return ;
            }

            ::InvalidateRect(m_hWnd, NULL, FALSE);
        }

        void View::OnEnter()
        {
            if (echo_)
                buffer_.Write(commandLine_);

            commandLine_.RunCurrentCommand(*this, filesystem_);
        }

        void View::FullReset()
        {
            buffer_.Reset();
            commandLine_.FullReset();
            SyncWithViewport();
        }

        void View::ClearScreen()
        {
            buffer_.Reset();
            SyncWithViewport();
        }

        void View::SyncWithViewport()
        {
            ::InvalidateRect(m_hWnd, NULL, FALSE);

            MSG msg;
            if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if (::GetMessageW(&msg, NULL, 0, 0))
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessageW(&msg);
                }
            }
        }
    }
}
