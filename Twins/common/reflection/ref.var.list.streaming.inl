#pragma once

#include <iosfwd>
#include <iomanip>

namespace Ref
{
    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, List::Accessor& accessor)
    {
        stream << accessor.Get();
        return stream;
    }

    namespace Hlp
    {
        struct Indent
        {
            enum { TabWidth = 4 };
            Indent(int n): tabNum(n) {}
            int tabNum;
        };

        template <class C, class T>
        inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Indent const& indent)
        {
            if (indent.tabNum > 0)
                stream << std::setw(indent.tabNum * Indent::TabWidth) << std::setfill((C)' ') << (C)' ';

            return stream;
        }
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, List& list)
    {
        static int depth = -1;
        ++depth;
        stream << std::endl << Hlp::Indent(depth) << (C)'[' << list.GetName().c_str() << (C)']';
        for (List::iterator it = list.begin(); it != list.end(); ++it)
        {
            stream << std::endl << Hlp::Indent(depth) << it->first.c_str() << (C)'=' << it->second;
        }
        --depth;
        return stream;
    }
}
