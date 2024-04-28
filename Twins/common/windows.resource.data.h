#pragma once

#include <boost/noncopyable.hpp>
#include <objidl.h>
#include <atlcomcli.h>

namespace Res
{
    class Custom;

    class Data: boost::noncopyable
    {
    public:
        Data();
        Data(Data const& rhs);
        Data(Custom& resource);
        ~Data();

        Data& operator = (Data const& rhs);
        void Swap(Data& rhs);

        CComPtr<IStream> GetStream() const;

    private:
        HGLOBAL global_;
        void *data_;
    };
}
