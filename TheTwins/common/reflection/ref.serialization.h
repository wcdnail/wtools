#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/nvp.hpp>

namespace Ref
{
    template <class Derived>
    class Serializable: public boost::serialization::wrapper_traits<const Derived>
    {
    public:
        ~Serializable();

    protected:
        Serializable();

    private:
        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive& ar, const unsigned int);
    };

    template <class Derived>
    inline Serializable<Derived>::Serializable()
    {}

    template <class Derived>
    inline Serializable<Derived>::~Serializable()
    {}

    template <class Derived>
    template <class Archive>
    inline void Serializable<Derived>::serialize(Archive& ar, const unsigned int)
    {
        static_cast<Derived*>(this)->Serialize(ar);
    }
}
