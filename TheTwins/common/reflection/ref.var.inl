#pragma once

#include "wcdafx.api.h"
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

namespace Ref
{
    class Var::Int
    {
    public:
        void SetName(CharString const& name) { name_ = name; }
        CharString const& GetName() const { return name_; }

        std::type_info const& Type() const { return type_; }

        virtual void Serialize(boost::archive::xml_oarchive& ar) = 0;
        virtual void Serialize(boost::archive::xml_iarchive& ar) = 0;
        virtual void Serialize(std::ostream& ar) = 0;
        virtual void Serialize(std::wostream& ar) = 0;

    protected:
        WCDAFX_API Int(CharString const& name, std::type_info const& type);
        WCDAFX_API virtual ~Int() = 0;

        CharString name_;
        std::type_info const& type_;
    };

    template <class T>
    class Var::Impl: public Var::Int
    {
    public:
        Impl(CharString const& name, T& object)
            : Int(name, typeid(T))
            , ref_(object)
        {}

        virtual ~Impl() {}

        T& GetRef() const { return ref_; }

        virtual void Serialize(boost::archive::xml_oarchive& ar);
        virtual void Serialize(boost::archive::xml_iarchive& ar);
        virtual void Serialize(std::ostream& ar);
        virtual void Serialize(std::wostream& ar);

    private:
        T& ref_;

        template <class Archive>
        void DoSerialize(Archive& ar);
    };

    template <class T>
    inline Var::Var(CharString const& name, T& object)
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

