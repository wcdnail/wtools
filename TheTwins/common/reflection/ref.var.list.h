#pragma once

#include "wcdafx.api.h"
#include "ref.var.h"
#include <map>

namespace Ref
{
    class List: public Serializable<List>
    {
    public:
        class Accessor;
        typedef std::map<CharString, Var> Map;
            
        typedef Map::iterator iterator;
        typedef Map::const_iterator const_iterator;

        WCDAFX_API ~List();
        WCDAFX_API List(CharString const& name);
        WCDAFX_API List(List& root, CharString const& name);
        WCDAFX_API List(List const& rhs);
        WCDAFX_API List& operator = (List const& rhs);

        WCDAFX_API void SetName(CharString const& name);
        WCDAFX_API CharString const& GetName() const;

        template <class T>
        Var& Assign(CharString const& varName, T& object);

        WCDAFX_API Var& Get(CharString const& varName);

        WCDAFX_API Accessor operator[] (CharString const& varName);

        WCDAFX_API iterator begin();
        WCDAFX_API iterator end();
        WCDAFX_API const_iterator begin() const;
        WCDAFX_API const_iterator end() const;

        WCDAFX_API void Swap(List& rhs);

        template <class Archive>
        void Serialize(Archive& ar);
        
    private:
        Map list_;
        CharString name_;

        WCDAFX_API CharString GetVarName(CharString const& varName) const;
    };
}

#include "ref.var.list.inl"
#include "ref.var.list.serialization.inl"
#include "ref.var.list.streaming.inl"
