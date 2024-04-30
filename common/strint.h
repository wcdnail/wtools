#pragma once

#include <atlstr.h>
#include <string>
#include <sstream>
#include <type_traits>
#include <string_view>

using   LString = std::string;
using  OLStream = std::ostringstream;
using   WString = std::wstring;
using  WStrView = std::wstring_view;

namespace std
{
    template <typename BaseType, class StringTraits>
    struct hash<ATL::CStringT<BaseType, StringTraits>>
    {
        size_t operator()(const ATL::CStringT<BaseType, StringTraits>& _Keyval) const noexcept
        {
            return (_Hash_array_representation(_Keyval.GetString(), _Keyval.GetLength()));
        }
    };
}
