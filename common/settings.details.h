#pragma once

#include "string.utils.wide2multi.multi2wide.hxx"
#include <iosfwd>
#include <string>

namespace Conf
{
    template <class C, class T, size_t Size, class Array>
    inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& stream, Array (&arr)[Size])
    {
        for (size_t i=0; i<Size; i++)
        {
            Array temp;
            stream >> temp;
            arr[i]  = temp;
        }

        return stream;
    }

    template <class C, class T, size_t Size, class Array>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Array (&arr)[Size])
    {
        for (size_t i=0; i<Size; i++)
            stream << arr[i] << (C)' ';

        return stream;
    }
}

#ifdef _WIN32

#include <atlstr.h>
#include <atltypes.h>

namespace Conf
{
    template <class C, class T>
    inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& stream, RECT& rect)
    {
        stream >> rect.left
               >> rect.top
               >> rect.right
               >> rect.bottom
               ;

        return stream;
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, RECT const& rect)
    {
        stream << rect.left << (C)' ' 
               << rect.top << (C)' ' 
               << rect.right << (C)' ' 
               << rect.bottom << (C)' '
               ;

        return stream;
    }

    inline bool operator == (RECT const& l, RECT const& r)
    {
        return (l.left   == r.left)
            && (l.top    == r.top)
            && (l.right  == r.right)
            && (l.bottom == r.bottom)
            ;
    }

    inline bool operator == (CRect const& l, CRect const& r)
    {
        return (l.left   == r.left)
            && (l.top    == r.top)
            && (l.right  == r.right)
            && (l.bottom == r.bottom)
            ;
    }

    template <class C, class T>
    inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& stream, ATL::CString& str)
    {
        //std::basic_string<C, T> temp;
        //std::getline(stream, temp, 0);

        std::string temp((std::istreambuf_iterator<C>(stream))
                        , std::istreambuf_iterator<C>());

        std::basic_string<TCHAR, T> cvt;
        Str::Convert(cvt, temp);
        str = cvt.c_str();

        return stream;
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, ATL::CString& str)
    {
        std::basic_string<C, T> cvt;
        Str::Convert(cvt, std::basic_string<TCHAR, T>((PCTSTR)str));
        stream << cvt.c_str();

        return stream;
    }
}
#endif
