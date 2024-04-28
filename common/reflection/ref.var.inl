#pragma once

#include "wcdafx.api.h"
#if 0
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#endif

namespace Ref
{
    class Var::Int
    {
    public:
        void SetName(LString const& name) { name_ = name; }
        LString const& GetName() const { return name_; }

        std::type_info const& Type() const { return type_; }

#if 0
        virtual void Serialize(boost::archive::xml_oarchive& ar) = 0;
        virtual void Serialize(boost::archive::xml_iarchive& ar) = 0;
#endif
        virtual void Serialize(std::ostream& ar) = 0;
        virtual void Serialize(std::wostream& ar) = 0;

    protected:
        WCDAFX_API Int(LString const& name, std::type_info const& type);
        WCDAFX_API virtual ~Int() = 0;

        LString name_;
        std::type_info const& type_;
    };

    template <class T>
    class Var::Impl: public Var::Int
    {
    public:
        Impl(LString const& name, T& object)
            : Int(name, typeid(T))
            , ref_(object)
        {}

        virtual ~Impl() {}

        T& GetRef() const { return ref_; }

#if 0
        virtual void Serialize(boost::archive::xml_oarchive& ar);
        virtual void Serialize(boost::archive::xml_iarchive& ar);
#endif
        virtual void Serialize(std::ostream& ar);
        virtual void Serialize(std::wostream& ar);

    private:
        T& ref_;

        template <class Archive>
        void DoSerialize(Archive& ar);
    };

    template <class T>
    inline Var::Var(LString const& name, T& object)
        : ref_(new Impl<T>(name, object))
    {
    }

    template <class T>
    inline Var& Var::operator = (T& object)
    {
        ref_.reset(new Impl<T>(object));
        return *this;
    }

    template <class T>
    inline T& Var::Get()
    {
        CheckType(ref_->GetName(), typeid(T), ref_->Type());
        return static_cast<Impl<T>*>(ref_.get())->GetRef();
    }
}

