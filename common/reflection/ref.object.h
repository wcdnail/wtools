#pragma once

#if 0
#include <boost/serialization/access.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#endif

namespace Ref
{
    //
    // class T - type
    // class S - string type
    // S const& (T::*TS)(void) - store T to string
    // void (T::*FS)(String const&) - restore T from string
    // 
    template <class T, class S, S const& (T::*TS)(void), void (T::*FS)(S const&)>
    class Object: public T
                // , public boost::serialization::wrapper_traits<const Object<T, S, TS, FS> >
    {
    private:
        // typedef BOOST_DEDUCED_TYPENAME T super;

    public:
        Object()
            // : super()
        {}

    private:
        friend class boost::serialization::access;

        // BOOST_SERIALIZATION_SPLIT_MEMBER()

        template <class A>
        void save(A& ar, const unsigned int) const
        {
            // BOOST_DEDUCED_TYPENAME S x = ((*this).*(TS))();
            //ar & boost::serialization::make_nvp("X", x);
        }

        template <class A>
        void load(A& ar, const unsigned int)
        {
            // BOOST_DEDUCED_TYPENAME S x;
            //ar & boost::serialization::make_nvp("X", x);
            ((*this).*(TS))(x);
        }
    };
}
