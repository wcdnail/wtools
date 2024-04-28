#pragma once

#include "conf.var.h"

namespace Config
{
    class Section: Noncopyable
    {
    private:
        struct Accessor;

    public:
        static Section& DummySection();

        Section(String const& name, Section& parent = DummySection());
        ~Section();

        Accessor operator [] (String const& name);

        template <class T>
        void Bind(String const& name, T& vref);

        #define _AutoBind(V) \
            Bind(BOOST_JOIN(L, BOOST_STRINGIZE(V)), V)

    private:
        String Name_;
        VarMap Vars_;
        SectionPointeeVector Siblings_;

        void AddSibling(Section* sibling);
        void Initialize(Var&) {}

    private:
        Section();
        bool IsDummy() const;
    };

    struct Section::Accessor: Noncopyable
    {
        Section& Section_;
        String const& Name_;

        Accessor(Section& section, String const& name);
        template <class T>
        void operator = (T& vref) const;
    };

    inline Section::Accessor Section::operator [] (String const& name)
    {
        return Accessor(*this, name);
    }

    inline Section::Accessor::Accessor(Section& section, String const& name)
        : Section_(section)
        , Name_(name)
    {}

    template <class T>
    inline void Section::Accessor::operator = (T& vref) const
    {
        Section_.Bind(Name_, vref);
    }

    template <class T>
    inline void Section::Bind(String const& name, T& vref)
    {
        VarMap::iterator it = Vars_.find(name);
        if (it == Vars_.end())
        {
            VarMap::_Pairib rv = Vars_.insert(std::make_pair(name, Var(vref)));
            Initialize(rv.first->second);
        }
        else
        {
            //Debug::Trace << "`" << name << L"` already in `" << Name_ << L"`\n";
        }
    }
}
