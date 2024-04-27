#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>

namespace Ref
{
    template <class T>
    class Container: public T
                   , public boost::serialization::wrapper_traits<const Container<T> >
    {
    private:
        typedef BOOST_DEDUCED_TYPENAME T super;

    public:
        typedef BOOST_DEDUCED_TYPENAME T::value_type value_type;
        typedef BOOST_DEDUCED_TYPENAME T::iterator iterator;
        typedef BOOST_DEDUCED_TYPENAME T::const_iterator const_iterator;
        typedef BOOST_DEDUCED_TYPENAME T::size_type size_type;

        Container();

    private:
        friend class boost::serialization::access;

        BOOST_SERIALIZATION_SPLIT_MEMBER()

        template <class A>
        void save(A& ar, const unsigned int) const;

        template <class A>
        void load(A& ar, const unsigned int);
    };

    template <class T>
    inline Container<T>::Container()
    {
    }

    template <class T>
    template <class A>
    inline void Container<T>::save(A& ar, const unsigned int) const
    {
        size_type count = super::size();
        ar & BOOST_SERIALIZATION_NVP(count);

        for (const_iterator it = super::cbegin(); it != super::cend(); ++it)
            ar & boost::serialization::make_nvp("item", *it);
    }

    template <class T>
    template <class A>
    inline void Container<T>::load(A& ar, const unsigned int)
    {
        size_type count = 0;
        ar & BOOST_SERIALIZATION_NVP(count);
        if (count > 0)
        {
            super temp;

            for (size_type i = 0; i < count; i++)
            {
                value_type item;
                ar & boost::serialization::make_nvp("item", item);
                temp.insert(temp.end(), item);
            }

            super::swap(temp);
        }
    }

    template <class C, class T, class Tc>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Container<Tc> const& cont)
    {
        for (BOOST_DEDUCED_TYPENAME Container<Tc>::const_iterator it = cont.begin(); it != cont.end(); ++it)
            stream << *it << (C)' ';

        return stream;
    }
}
