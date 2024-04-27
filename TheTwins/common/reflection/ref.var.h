#pragma once

#include "wcdafx.api.h"
//#include "ref.serialization.h"
#include <string.hp.h>
#include <typeinfo>
#include <memory>
#include <string>

namespace Ref
{
    class Var //: public Serializable<Var>
    {
    public:
        WCDAFX_API ~Var();

        WCDAFX_API Var(Var const& rhs);
        WCDAFX_API Var& operator = (Var const& rhs);

        WCDAFX_API void Swap(Var& rhs);

        template <class T>
        Var(CharString const& name, T& object);

        template <class T>
        Var& operator = (T& object);

        WCDAFX_API void SetName(CharString const& name);
        WCDAFX_API CharString const& GetName() const;
        WCDAFX_API std::type_info const& GetType() const;

        template <class T>
        T& Get();

        template <class Archive>
        void Serialize(Archive& ar);

    private:
        class Int;
        template <class> class Impl;
        std::shared_ptr<Int> ref_;

        static void CheckType(CharString const& name, std::type_info const& target, std::type_info const& source);
    };
}

#include "ref.var.inl"
#include "ref.var.serialization.inl"
#include "ref.var.streaming.inl"
