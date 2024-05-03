#pragma once

#include "cp.command.h"
#include <gdi+.paint.args.h>
#include <boost/noncopyable.hpp>
#include <string>
#include <deque>

namespace Shelltastix
{
    class Filesystem;

    namespace Console
    {
        class View;

        class CommandLine: boost::noncopyable
        {
        public:
            typedef std::wstring String;
            typedef std::deque<String> History;

            CommandLine(bool& posix);
            ~CommandLine();

            void Draw(Gdi::PaintArgs const& args) const;
            void Reset();
            void Set(String const& line);
            void Append(wchar_t symbol);
            void Erase(int count, bool back);
            void MoveCursor(int amount);
            String const& FromHistory(bool up);
            int GetLength() const;
            String const& GetPrompt() const;
            String const& GetLine() const;
            void RunCurrentCommand(View& Con, Filesystem& Fs);
            void FullReset();
            void ToggleInsertion();
            
        private:
            bool& posix_;
            bool insert_;
            mutable String prompt_;
            String line_;
            History history_;
            int hindex_;
            int cpos_;

            Gdiplus::RectF GetCursorBbox(Gdi::PaintArgs const& args) const;
            void DrawCursor(Gdi::PaintArgs const& args) const;
            void DrawCommandLine(Gdi::PaintArgs const& args) const;
            void DrawPrompt(Gdi::PaintArgs const& args) const;
            void BuildPrompt() const;
            void ToHistory(String const& line);
            void NextHistoryIndex(bool up);
        };

        inline int CommandLine::GetLength() const { return (int)line_.length(); }
        inline CommandLine::String const& CommandLine::GetPrompt() const { return prompt_; }
        inline CommandLine::String const& CommandLine::GetLine() const { return line_; }
    }
}
