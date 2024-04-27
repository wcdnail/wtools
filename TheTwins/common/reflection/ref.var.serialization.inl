#pragma once

#if 0
#include <boost/serialization/nvp.hpp>

namespace Ref
{
    template <class T> 
    inline void Var::Impl<T>::Serialize(boost::archive::xml_oarchive& ar) 
    {
        DoSerialize(ar); 
    }

    template <class T> 
    inline void Var::Impl<T>::Serialize(boost::archive::xml_iarchive& ar) 
    { 
        DoSerialize(ar); 
    }

    template <class Archive>
    inline void Var::Serialize(Archive& ar)
    {
        ref_->Serialize(ar);
    }

    template <class T>
    template <class Archive>
    void Var::Impl<T>::DoSerialize(Archive& ar)
    {
        ar & boost::serialization::make_nvp(name_.c_str(), ref_);
    }
}
#endif
