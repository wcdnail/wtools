#include "stdafx.h"
#include "windows.resource.custom.h"
#include "string.utils.error.code.h"

namespace Res
{
    Custom::Custom()
        : resource_(NULL)
        , size_(0)
        , message_()
    {}

    Custom::Custom(PCTSTR name, PCTSTR type, HMODULE module/* = NULL*/)
        : resource_(NULL)
        , size_(0)
        , message_()
    {
        Load(name, type, module);
    }

    Custom::~Custom()
    {
    }

    CString const& Custom::GetMessage() const 
    {
        return message_; 
    }

    HRESULT Custom::Load(PCTSTR name, PCTSTR type, HMODULE module/* = NULL*/)
    {
        message_.Empty();

        HRESULT hr = S_OK;
        HRSRC res = ::FindResource(module, name, type);
        if (!res)
        {
            hr = ::GetLastError();
            message_.Format(_T("FindResource(%x, %x, %x) failed: %s")
                , module, name, type, Str::ErrorCode<>::SystemMessage(hr));
        }
        else
        {
            DWORD size = ::SizeofResource(module, res);
            if (!size)
            {
                hr = ::GetLastError();
                message_.Format(_T("SizeofResource(%x, %x) failed: %s")
                    , module, res, Str::ErrorCode<>::SystemMessage(hr));
            }
            else
            {
                void const* temp = ::LockResource(::LoadResource(module, res));
                if (!temp)
                {
                    hr = ::GetLastError();
                    message_.Format(_T("LoadResource(%x, %x) failed: %s")
                        , module, res, Str::ErrorCode<>::SystemMessage(hr));
                }
                else
                {
                    resource_ = temp;
                    size_ = size;
                }
            }
        }

        return hr;
    }
}
