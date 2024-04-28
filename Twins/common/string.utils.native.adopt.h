#ifndef _cf_string_utils_native_adopt_h__
#define _cf_string_utils_native_adopt_h__

#if 0

#include <atlstr.h>
#include <atlconv.h>
#include <string>
#include <ostream>
#include <boost/serialization/wrapper.hpp>
#include <boost/serialization/access.hpp>

namespace Str
{
    template <typename C = wchar_t, typename Traits = ATL::StrTraitATL<C, ATL::ChTraitsCRT<C> > >
    class Adapter: ATL::CStringT<C, Traits>
                 , boost::serialization::wrapper_traits<const Adapter<C, Traits> >
    {
    private:
        typedef ATL::CSimpleStringT<C> SuperSuper;
        typedef ATL::CStringT<C, Traits> Super;

    public:
        typedef std::basic_string<C, std::char_traits<C>, std::allocator<C> > Opposite;
        typedef typename Opposite::size_type size_type;
        typedef typename Opposite::const_iterator const_iterator;
        typedef typename Opposite::value_type value_type;

        static const size_type npos;

        Adapter();
        Adapter(C const* source);
        Adapter(C const* beg, C const* end);
        Adapter(const_iterator beg, const_iterator end);
        Adapter(Adapter const& rhs);
        Adapter(Super const& source);
        Adapter(Opposite const& source);
        ~Adapter();

        using Super::Format;
        Opposite Native() const;
        bool LessThan(Adapter const& rhs) const;

        size_type length() const;
        C const* c_str() const;
        bool empty() const;
        const_iterator begin() const;
        const_iterator end() const;
        void clear();
        size_type rfind(C symbol) const;
        Adapter substr(size_type start, size_type count = npos) const;
        void swap(Adapter& rhs);
        void reserve(size_type size);
        void resize(size_type size);
        Adapter& operator += (Opposite const& op);
        Adapter& operator += (Adapter const& op);
        Adapter& operator += (C const* raw);
        Adapter& operator += (C ch);
        C& operator[] (size_t index);

    private:
        friend typename boost::serialization::access;
        using Super::SetLength;

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int)
        {
            Opposite temp = c_str();
            ar & boost::serialization::make_nvp(NULL, temp);
            *this = temp.c_str();
        }

    private:
        C* buffer() const { return const_cast<Adapter*>(this)->GetBuffer(); }
    };

    template <typename C, typename Traits>
    const typename Adapter<C, Traits>::size_type Adapter<C, Traits>::npos = Adapter<C, Traits>::Opposite::npos;

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter() 
        : Super()
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter(C const* source) 
        : Super(source)
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter(C const* beg, C const* end) 
        : Super(Opposite(beg, end).c_str())
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter(const_iterator beg, const_iterator end) 
        : Super(Opposite(beg, end).c_str())
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter(Adapter const& rhs) 
        : Super(rhs)
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter(Super const& source) 
        : Super(source)
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::Adapter(Opposite const& source) 
        : Super(source.c_str())
    {}

    template <typename C, typename Traits>
    inline Adapter<C, Traits>::~Adapter() 
    {}

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>::Opposite Adapter<C, Traits>::Native() const 
    {
        return Opposite(GetString()); 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>::size_type Adapter<C, Traits>::length() const 
    {
        return (size_type)GetLength(); 
    }

    template <typename C, typename Traits>
    inline C const* Adapter<C, Traits>::c_str() const 
    {
        return (C const*)*this; 
    }

    template <typename C, typename Traits>
    inline bool Adapter<C, Traits>::empty() const 
    {
        return IsEmpty(); 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>::const_iterator Adapter<C, Traits>::begin() const 
    {
        return const_iterator(buffer(), NULL); 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>::const_iterator Adapter<C, Traits>::end() const 
    {
        return const_iterator(buffer() + length(), NULL); 
    }

    template <typename C, typename Traits>
    inline void Adapter<C, Traits>::clear()
    { 
        Empty(); 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>::size_type Adapter<C, Traits>::rfind(C symbol) const 
    { 
        int rv = Super::ReverseFind(symbol); return -1 == rv ? npos : rv; 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits> Adapter<C, Traits>::substr(size_type start, size_type count = npos) const
    {
        return Super::Mid((int)start, npos == count ? GetLength() - (int)start : (int)count); 
    }

    template <typename C, typename Traits>
    inline void Adapter<C, Traits>::swap(Adapter& rhs) 
    {
        Adapter temp = rhs; rhs = *this; *this = rhs; 
    }

    template <typename C, typename Traits>
    inline void Adapter<C, Traits>::reserve(size_type size) 
    {
        Super::SetLength((int)size); 
    }

    template <typename C, typename Traits>
    inline void Adapter<C, Traits>::resize(size_type size) 
    {
        Super::SetLength(Super::GetLength() + (int)size); 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>& Adapter<C, Traits>::operator += (Opposite const& op)
    {
        Super::operator += (op.c_str()); return *this; 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>& Adapter<C, Traits>::operator += (Adapter const& op)
    {
        Super::operator += (op); return *this; 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>& Adapter<C, Traits>::operator += (C const* raw) 
    {
        Super::operator += (raw); return *this; 
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits>& Adapter<C, Traits>::operator += (C ch) 
    {
        Super::AppendChar(ch); return *this; 
    }

    template <typename C, typename Traits>
    inline bool Adapter<C, Traits>::LessThan(Adapter const& rhs) const 
    {
        return operator < (*((Super*)this), *((Super*)&rhs)); 
    }

    template <typename C, typename Traits>
    inline C& Adapter<C, Traits>::operator[] (size_t index) 
    { 
        int i = (int)index;
        if (i >= 0 && i < Super::GetLength())
        {
            C* buffer = GetBuffer();
            return buffer[index];
        }

        throw std::out_of_range();

        C dummy;
        return dummy; 
    }

    template <typename C, typename Traits>
    inline bool operator < (typename Adapter<C, Traits> const& left, typename Adapter<C, Traits> const& right)
    {
        return left.LessThan(right);
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits> operator + (typename Adapter<C, Traits> const& left, typename Adapter<C, Traits> const& right)
    {
        typename Adapter<C, Traits> result(left);
        result += right;
        return result;
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits> operator + (typename Adapter<C, Traits> const& left, C const* rawRight)
    {
        typename Adapter<C, Traits> result(left);
        result += rawRight;
        return result;
    }

    template <typename C, typename Traits>
    inline typename Adapter<C, Traits> operator + (C const* rawLeft, typename Adapter<C, Traits> const& right)
    {
        typename Adapter<C, Traits> result(rawLeft);
        result += right;
        return result;
    }

    template <typename C, typename Traits>
    inline std::basic_ostream<C>& operator + (std::basic_ostream<C>& stream, typename Adapter<C, Traits> const& rhs)
    {
        stream << rhs.c_str();
        return stream;
    }
}

#endif
#endif // _cf_string_utils_native_adopt_h__

