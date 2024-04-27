#ifndef _cf_string_utils_W2M_M2W_h__
#define _cf_string_utils_W2M_M2W_h__

#include <string>
#include <memory>

namespace Str
{
    inline void Convert(std::basic_string<char>& result, std::basic_string<char> const& source, unsigned rsvd = 0)
    {
        result = source;
    }

    inline void Convert(std::basic_string<wchar_t>& result, std::basic_string<wchar_t> const& source, unsigned rsvd = 0)
    {
        result = source;
    }

    inline void ConvertFrom(std::basic_string<char>& result, std::basic_string<wchar_t> const& source
#ifdef _WIN32
        , unsigned codepage = CP_ACP
#endif
        )
    {
        size_t l = source.length();
        if (l > 0)
        {
            ++l;
            std::unique_ptr<char[]> buffer(new char[l]);
#ifdef _WIN32
            ::WideCharToMultiByte(codepage, 0, source.c_str(), (int)l, buffer.get(), (int)l, NULL, NULL);
#endif
            result = buffer.get();
        }
        else {
            std::string().swap(result);
        }
    }

    inline void Convert(std::basic_string<wchar_t>& result, std::basic_string<char> const& source
#ifdef _WIN32
        , unsigned codepage = CP_ACP
#endif
        )
    {
        size_t l = source.length();
        if (l > 0)
        {
            ++l;
            std::unique_ptr<wchar_t[]> buffer(new wchar_t[l]);
#ifdef _WIN32
            ::MultiByteToWideChar(codepage, 0, source.c_str(), (int)l, buffer.get(), (int)l);
#endif
            result = buffer.get();
        }
        else {
            std::wstring().swap(result);
        }
    }
}

#endif // _cf_string_utils_W2M_M2W_h__

