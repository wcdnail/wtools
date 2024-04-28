#pragma once

#include "wcdafx.api.h"
#include "ref.var.h"
#include <map>

namespace Ref
{
    class List // : public Serializable<List>
    {
    public:
        class Accessor;
        typedef std::map<LString, Var> Map;
            
        typedef Map::iterator iterator;
        typedef Map::const_iterator const_iterator;

        WCDAFX_API ~List();
        WCDAFX_API List(LString const& name);
        WCDAFX_API List(List& root, LString const& name);
        WCDAFX_API List(List const& rhs);
        WCDAFX_API List& operator = (List const& rhs);

        WCDAFX_API void SetName(LString const& name);
        WCDAFX_API LString const& GetName() const;

        template <class T>
        Var& Assign(LString const& varName, T& object);

        WCDAFX_API Var& Get(LString const& varName);

        WCDAFX_API Accessor operator[] (LString const& varName);

        WCDAFX_API iterator begin();
        WCDAFX_API iterator end();
        WCDAFX_API const_iterator begin() const;
        WCDAFX_API const_iterator end() const;

        WCDAFX_API void Swap(List& rhs);

        template <class Archive>
        void Serialize(Archive& ar);
        
    private:
        Map list_;
        LString name_;

        WCDAFX_API LString GetVarName(LString const& varName) const;
    };
}

#include "ref.var.list.inl"
#include "ref.var.list.serialization.inl"
#include "ref.var.list.streaming.inl"
