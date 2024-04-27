#include "stdafx.h"
#include "windows.resource.data.h"
#include "windows.resource.custom.h"
#include "string.utils.error.code.h"

namespace Res
{
    Data::Data() 
        : global_(NULL)
        , data_(NULL)
    {}

    Data::Data(Custom& resource) 
        : global_(::GlobalAlloc(GMEM_MOVEABLE, resource.size_))
        , data_(::GlobalLock(global_))
    {
        ::CopyMemory(data_, resource.resource_, resource.size_);
    }

    Data::~Data() 
    {
        if (NULL != global_)
        {
            ::GlobalUnlock(global_);
            ::GlobalFree(global_);
        }
    }

    Data::Data(Data const& rhs)
        : global_(rhs.global_)
        , data_(rhs.data_)
    {
    }

    Data& Data::operator = (Data const& rhs)
    {
        Data temp(rhs);
        Swap(temp);
        return *this;
    }

    void Data::Swap(Data& rhs)
    {
        std::swap(global_, rhs.global_);
        std::swap(data_, rhs.data_);
    }

    CComPtr<IStream> Data::GetStream() const
    {
        CComPtr<IStream> stream;
        HRESULT hr = ::CreateStreamOnHGlobal(global_, FALSE, &stream);
// ##TODO: Handle error"))
        //if (S_OK != hr)
            //resource.message_.Format(_T("CreateStreamOnHGlobal failed: %s"), Str::ErrorCode<>::SystemMessage(hr));
        return stream;
    }
}
