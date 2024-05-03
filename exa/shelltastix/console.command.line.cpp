#include "stdafx.h"
#include "console.command.line.h"
#include "console.view.h"
#include "filesystem.h"
#include "cp.internal.commands.h"
#include "run.external.h"
#include <rect.gdi+.h>
#include <locale.helper.h>
#include <generic.increment.h>
#include <string.utils.error.code.h>
#include <rubbish.heap.h>
#include <atlstr.h>
#include <exception>

namespace Shelltastix
{
    namespace Console
    {
        CommandLine::CommandLine(bool& posix)
            : posix_(posix)
            , insert_(true)
            , prompt_(L"$#")
            , line_()
            , history_()
            , hindex_(0)
            , cpos_(0)
        {}

        CommandLine::~CommandLine()
        {}

        static float _CursorWidth(Gdi::PaintArgs const& args, PCWSTR text)
        {
            Gdiplus::RectF bbox;
            args.gr.MeasureString(text, 1, &args.font, 
                Gdiplus::RectF(), Gdiplus::StringFormat::GenericTypographic(), 
                &bbox);

            return bbox.Width;
        }

        Gdiplus::RectF CommandLine::GetCursorBbox(Gdi::PaintArgs const& args) const
        {
            Gdiplus::RectF rv(args.bbox.GetRight(), args.bbox.GetTop(), 
                _CursorWidth(args, L"A"), args.bbox.Height);

            String::size_type n = cpos_;
            String::size_type l = line_.length();
            if (n < l)
            {
                PCWSTR text = &line_[n];
                int len = (int)l - (int)n;
                Gdiplus::RectF bbox;
                args.gr.MeasureString(text, len, &args.font, Gdiplus::RectF(), &args.format, &bbox);
                rv.X -= bbox.Width - 2;
            }

            // FIXME: customize `insert` cursor
            if (insert_)
            {
                rv.Y = rv.GetBottom() - 3;
                rv.Height = 3;
            }

            return rv;
        }

        inline void CommandLine::DrawCursor(Gdi::PaintArgs const& args) const
        {
            Gdiplus::RectF rect = GetCursorBbox(args);

            // TODO: Выбрать форму и тип курсора.
#if 0
            args.gr.FillRectangle(&args.brush, rect);
            args.gr.DrawRectangle(&Gdiplus::Pen(Gdiplus::Color::Red), rect);
#endif

            CRect rc = FromRectF(rect);
            if (rc.top < args.rc.bottom)
                args.dc.InvertRect(rc);
        }

        inline void CommandLine::DrawCommandLine(Gdi::PaintArgs const& args) const
        {
            PCWSTR text = line_.c_str();
            int len = (int)line_.length();

            Gdiplus::RectF rect = args.bbox;
            if (len > 0)
            {
                Gdiplus::RectF bbox;
                args.gr.MeasureString(text, len, &args.font, Gdiplus::RectF(), &args.format, &bbox);

                rect = Gdiplus::RectF(args.bbox.GetRight(), args.bbox.GetBottom() - bbox.Height, bbox.Width + 1, bbox.Height);
                args.gr.DrawString(text, len, &args.font, rect, &args.format, &args.brush);
            }

            Gdi::PaintArgs crArgs(args.dc, args.gr, args.rc, rect, args.font, args.format, args.brush);
            DrawCursor(crArgs);
        }

        inline void CommandLine::DrawPrompt(Gdi::PaintArgs const& args) const
        {
            PCWSTR text = prompt_.c_str();
            int len = (int)prompt_.length();

            Gdiplus::RectF bbox;
            args.gr.MeasureString(text, len, &args.font, Gdiplus::RectF(), &args.format, &bbox);

            Gdiplus::RectF rect(args.bbox.X, args.bbox.GetBottom(), bbox.Width + 1, bbox.Height);
            args.gr.DrawString(text, len, &args.font, rect, &args.format, &args.brush);

            Gdi::PaintArgs cmArgs(args.dc, args.gr, args.rc, rect, args.font, args.format, args.brush);
            DrawCommandLine(cmArgs);
        }

        void CommandLine::Draw(Gdi::PaintArgs const& args) const
        {
            BuildPrompt();
            DrawPrompt(args);
        }

        void CommandLine::BuildPrompt() const
        {
            String path = Filesystem::GetCurrentPath(posix_);

            if (posix_)
            {
                String user = RubbishHeap::GetCurrentUserName();
                String host = RubbishHeap::GetHostName();
                String symb = RubbishHeap::IsCurrentUserAdmin() ? L"#" : L"$";

                prompt_ = user + L"@" + host + L":" + path + symb;
            }
            else
            {
                prompt_ = path + L">";
            }
        }

        void CommandLine::Reset()
        {
            line_.clear();
            cpos_ = 0;
        }

        void CommandLine::Set(String const& line)
        {
            line_ = line;
            cpos_ = (int)line_.length();
        }

        void CommandLine::Append(wchar_t symbol)
        {
            wchar_t temp[2] = {symbol, 0};

            if (insert_)
                line_.insert(cpos_, temp, 1);
            else
                line_.replace(cpos_, 1, temp, 1);

            MoveCursor(1);
        }

        void CommandLine::Erase(int count, bool back)
        {
            if (back)
            {
                int last = cpos_;
                MoveCursor(-1);
                count = !last ? 0 : count;
            }

            line_.erase(cpos_, count);
        }

        void CommandLine::MoveCursor(int amount)
        {
            Generic::Increment(cpos_, amount, 0, (int)line_.length());
        }

        void CommandLine::ToHistory(String const& line)
        {
            History::const_iterator it = std::find(history_.begin(), history_.end(), line);
            if (it == history_.end())
            {
                history_.push_front(line);
                hindex_ = 0;
            }
            else
            {
                // FIXME: last entered item
                hindex_ = (int)(it - history_.begin());
            }
        }

        void CommandLine::NextHistoryIndex(bool up)
        {
            if (!history_.empty())
                Generic::Increment(hindex_, up ? 1 : -1, 0, (int)history_.size()-1, true);
        }

        CommandLine::String const& CommandLine::FromHistory(bool up)
        {
            if (!history_.empty())
            {
                String const& temp = *(history_.begin() + hindex_);
                NextHistoryIndex(up);
                return temp;
            }

            static String dummy;
            return dummy;
        }

        void CommandLine::RunCurrentCommand(View& Con, Filesystem& Fs)
        {
            if (!line_.empty())
            {
                ToHistory(line_);
                String name = Command::ExtractCommand(line_);
                String args = Command::ExtractArguments(line_);

                Command::Def command(name);
                try
                {
                    if (!Command::Internals().Run(command, args))
                    {
                        RunExternal(Con, Fs, name, args);
                    }
                }
                catch (std::exception const& ex)
                {
                    Con.WriteLine("\n%S: %s", _LS("Внутренняя ошибка"), ex.what());
                }

                Reset();
            }
        }

        void CommandLine::FullReset()
        {
            Reset();
            prompt_.clear();
            history_.clear();
            hindex_ = 0;
        }

        void CommandLine::ToggleInsertion()
        {
            insert_ = !insert_;
        }
    }
}
