#pragma once

#include <string>

namespace Twins
{
    namespace Extern
    {
        struct Item
        {
            std::wstring Name;
            std::wstring Path;

            Item(std::wstring const& name = L"", std::wstring const& path = L"");
            ~Item();

            Item(Item const& rhs);
            Item& operator = (Item const& rhs);
            void Swap(Item& rhs);
        };
    }
}
