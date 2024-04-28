#include "stdafx.h"
#include "extern.item.h"

namespace Twins
{
    namespace Extern
    {
        Item::Item(std::wstring const& name/* = L""*/, std::wstring const& path/* = L""*/)
            : Name(name)
            , Path(path)
        {
        }

        Item::~Item()
        {
        }

        Item::Item(Item const& rhs)
            : Name(rhs.Name)
            , Path(rhs.Path)
        {
        }

        Item& Item::operator = (Item const& rhs)
        {
            Item temp(rhs);
            Swap(temp);
            return *this;
        }

        void Item::Swap(Item& rhs)
        {
            std::swap(Name, rhs.Name);
            std::swap(Path, rhs.Path);
        }
    }
}
