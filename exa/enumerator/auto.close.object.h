#ifndef _cf_auto_close_object_h__
#define _cf_auto_close_object_h__

#ifdef _WIN32
#  define DCF_STDCALL __stdcall
#else
#  define DCF_STDCALL
#endif

namespace Cf
{
    template <typename T = void*, T NullVal = 0>
    struct AutoClosable
    {
        typedef void (DCF_STDCALL *CloseFunction)(T);

        AutoClosable()
            : Object(NullVal)
            , Closer(0) 
        {}

        template <class P>
        AutoClosable(T object, P const& closeFunction)
            : Object(object)
            , Closer((CloseFunction)closeFunction) 
        {} 

        ~AutoClosable() 
        { 
            if ((0 != Closer) && (NullVal != Object))
                Closer(Object); 
        }

        AutoClosable(AutoClosable const& rhs)
            : Object(rhs.Object)
            , Closer(rhs.Closer)
        {
            const_cast<AutoClosable*>(&rhs)->Closer = 0;
        }

        AutoClosable& operator = (AutoClosable const& rhs)
        {
            AutoClosable temp(rhs);
            temp.Swap(*this);
            return *this;
        }

        void Swap(AutoClosable& rhs)
        {
            std::swap(Object, rhs.Object);
            std::swap(Closer, rhs.Closer);
        }

        operator T() const 
        { 
            return Object; 
        }

    private:
        T Object;
        CloseFunction Closer;
    };
}

#undef DCF_STDCALL

#endif // _cf_auto_close_object_h__
