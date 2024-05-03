#pragma once

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

namespace Shelltastix
{
    namespace Console
    {
        class CommandLine;

        class Buffer: boost::noncopyable
        {
        public:
            typedef std::wstring String;
            typedef String::size_type UInt;

            Buffer();
            ~Buffer();

            void Reset();
            void WriteRaw(wchar_t const* rawString);
            void Write(String const& line);
            void WriteLine(String const& line);
            void WriteV(PCSTR format, va_list ap);
            void WriteV(PCWSTR format, va_list ap);
            void WriteLineV(PCSTR format, va_list ap);
            void WriteLineV(PCWSTR format, va_list ap);
            void Write(CommandLine const& cline);
            void NewLine();
            int GetLinesCount() const;
            PCWSTR GetDisplayString(UInt& len, int startLine);

        private:
            typedef std::vector<int> LinePos;

            String buffer_;
            LinePos pos_;

            void Append(String const& string);
            void AppendLinesPos(String const& string);
        };
    }
}