#pragma once

#include "wtl.colorizer.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>

namespace Cf
{
    template <typename T>
    struct Colorizer::SpecificMembers
    {
        using Super = T;

        SpecificMembers(SpecificMembers const&) = delete;
        SpecificMembers& operator = (SpecificMembers const&) = delete;

        ~SpecificMembers()
        {}

        SpecificMembers()
        {}
    };

    template <>
    struct Colorizer::SpecificMembers<WTL::CComboBox>
    {
        std::unique_ptr<Control<WTL::CListBox>> Listbox;

        SpecificMembers(SpecificMembers const&) = delete;
        SpecificMembers& operator = (SpecificMembers const&) = delete;

        ~SpecificMembers();
        SpecificMembers();
    };
}
