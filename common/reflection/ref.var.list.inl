#pragma once

#include <algorithm>
#include <sstream>

namespace Ref
{
    class List::Accessor
    {
    public:
        ~Accessor() {}

        template <class T>
        Var& operator = (T& object)
        {
            return list_.Assign(varName_, object);
        }

        Var& Get() const
        {
            return list_.Get(varName_);
        }

    private:
        friend class List;
        Accessor(List& list, LString const& varName)
            : list_(list)
            , varName_(varName)
        {}

        List& list_;
        LString const& varName_;
    };

    template <class T>
    inline Var& List::Assign(LString const& varName, T& object)
    {
        auto rv = list_.emplace(std::make_pair<LString, Var>(GetVarName(varName), Var(varName, object)));
        return rv.first->second;
    }
}
