#ifndef cf_com_iunknown_impl_h__
#define cf_com_iunknown_impl_h__

#include <objbase.h>

template <class T>
class IUnknownImp: public T
{
protected:
    IUnknownImp()
        : RefCounter(0)
    {}

    virtual ~IUnknownImp() {}

public:
    STDMETHOD_(ULONG, AddRef)()
    {
        return ::InterlockedIncrement(&RefCounter);
    }

    STDMETHOD_(ULONG, Release)()
    {
        ULONG rv = ::InterlockedDecrement(&RefCounter);
        if (0 == rv) {
            delete this;
        }
        return rv;
    }

    STDMETHOD(QueryInterface)(REFIID rid, LPVOID* ppvObj)
    {
        if ((rid == IID_IUnknown) || (rid == __uuidof(T))) {
            AddRef();
            *ppvObj = static_cast<void*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    };

private:
    volatile long RefCounter;
};

#endif // cf_com_iunknown_impl_h__
