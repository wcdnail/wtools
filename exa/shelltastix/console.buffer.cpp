#include "stdafx.h"
#include "console.buffer.h"
#include "console.command.line.h"
#include <atlstr.h>
#include <atlconv.h>

namespace Shelltastix
{
    namespace Console
    {
        Buffer::Buffer()
            : buffer_()
            , pos_(0)
        {}

        Buffer::~Buffer()
        {}

        void Buffer::Reset()
        {
            buffer_.clear(); 
            pos_.clear();
        }

        void Buffer::WriteRaw(wchar_t const* rawString)
        {
            if (NULL != rawString)
            {
                Append(String(rawString)); 
            }
        }

        void Buffer::Write(String const& line) 
        {
            Append(line); 
        }

        void Buffer::WriteLine(String const& line) 
        { 
            Append(line); 
            NewLine(); 
        } 

        template <class CT>
        static typename CStringT< CT, StrTraitATL< CT, ChTraitsCRT< CT > > > _FormatV(CT const* format, va_list ap)
        {
            CStringT< CT, StrTraitATL< CT, ChTraitsCRT< CT > > > rv;
            rv.FormatV(format, ap);
            return rv;
        }

        void Buffer::WriteV(PCSTR format, va_list ap) 
        {
            Append(CA2W(_FormatV(format, ap)).m_psz); 
        }

        void Buffer::WriteV(PCWSTR format, va_list ap) 
        { 
            Append((PCWSTR)_FormatV(format, ap)); 
        }

        void Buffer::WriteLineV(PCSTR format, va_list ap) 
        { 
            Append(CA2W(_FormatV(format, ap)).m_psz); 
            NewLine(); 
        }

        void Buffer::WriteLineV(PCWSTR format, va_list ap) 
        {
            Append((PCWSTR)_FormatV(format, ap)); NewLine(); 
        }

        void Buffer::NewLine() 
        {
            Append(L"\n"); 
        }

        void Buffer::AppendLinesPos(String const& string)
        {
            if (string.length() < 1)
                return ;

            UInt n = 0, p = 0;
            int l = (int)buffer_.length();
            if (0 == l)
                pos_.push_back(0);

            do 
            {
                n = string.find(L'\n', p);

                if (String::npos != n)
                    pos_.push_back(l + (int)n + 1);
                else 
                    break;

                p = n + 1;
            }
            while (String::npos != n);
        }

        void Buffer::Append(String const& string) 
        {
            Buffer::AppendLinesPos(string);
            buffer_ += string; // TODO: следить за размером буфера.
        }

        int Buffer::GetLinesCount() const
        {
            return (int)pos_.size();
        }

        PCWSTR Buffer::GetDisplayString(UInt& len, int startLine)
        {
            len = buffer_.length();
            PCWSTR rv = buffer_.c_str();

            if (startLine >= 0 && startLine < GetLinesCount())
            {
                int n = pos_[startLine];
                if (n < (int)len)
                {
                    rv = &buffer_[n];
                    len -= n;
                }
                else
                {
                    rv = L"";
                    len = 0;
                }
            }

            return rv;
        }

        void Buffer::Write(CommandLine const& cline)
        {
            Append(cline.GetPrompt());
            Append(cline.GetLine());
            NewLine();
        }
    }
}
