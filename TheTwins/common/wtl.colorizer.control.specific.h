#pragma once

#include "wtl.colorizer.h"

namespace Cf
{
    template <typename T>
    struct Colorizer::SpecificMembers
    {
    };

    template <>
    struct Colorizer::SpecificMembers<WTL::CComboBox>
    {
        std::unique_ptr<Control<WTL::CListBox>> Listbox;

        SpecificMembers()
            : Listbox()
        {}

    private:
        SpecificMembers(SpecificMembers const&);
        SpecificMembers& operator = (SpecificMembers const&);
    };
}
