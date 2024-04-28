#include "stdafx.h"
#include "string.utils.wide2multi.multi2wide.hxx"
#include <memory>

namespace Str
{
    void Convert(std::basic_string<char>& result, std::basic_string<char>&& source, unsigned reserved)
    {
        UNREFERENCED_ARG(reserved);
        result = std::move(source);
    }

    void Convert(std::basic_string<wchar_t>& result, std::basic_string<wchar_t>&& source, unsigned reserved)
    {
        UNREFERENCED_ARG(reserved);
        result = std::move(source);
    }

    void Convert(std::basic_string<char>& result, std::basic_string<wchar_t> const& source, unsigned codepage)
    {
        size_t l = source.length();
        if (l < 1) {
            return;
        }
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(l + 1);
        WideCharToMultiByte(codepage, 0, source.c_str(), static_cast<int>(l), buffer.get(), static_cast<int>(l), nullptr, nullptr);
        std::basic_string<char>(buffer.get(), l).swap(result);
    }

    void Convert(std::basic_string<wchar_t>& result, std::basic_string<char> const& source, unsigned codepage)
    {
        size_t l = source.length();
        if (l < 1) {
            return;
        }
        std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(l + 1);
        MultiByteToWideChar(codepage, 0, source.c_str(), static_cast<int>(l), buffer.get(), static_cast<int>(l));
        std::basic_string<wchar_t>(buffer.get(), l).swap(result);
    }
}
