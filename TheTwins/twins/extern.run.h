#pragma once

#include "extern.item.h"

namespace Twins
{
    namespace Extern
    {
        HRESULT Run(Item const& item, 
                    std::wstring const& directory, 
                    std::wstring const& arguments, 
                    int show = 1 /* SW_SHOWNORMAL */,
                    wchar_t const* operation = NULL);
    }
}
