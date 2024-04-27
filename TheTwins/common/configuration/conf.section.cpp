#include "stdafx.h"
#include "conf.section.h"

namespace Config
{
    Section::Section()
        : Name_(L"Äíî")
        , Vars_()
    {}

    Section& Section::DummySection()
    {
        static Section dummy;
        return dummy;
    }

    bool Section::IsDummy() const { return &DummySection() == this; }

    void Section::AddSibling(Section* sibling)
    {
        if (!IsDummy() && (NULL != sibling))
            Siblings_.push_back(sibling);
    }

    Section::Section(String const& name, Section& parent)
        : Name_(name)
        , Vars_()
    {
        parent.AddSibling(this);
    }

    Section::~Section()
    {}
}
