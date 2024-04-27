#pragma once

#include <boost/noncopyable.hpp>
#include <wtypes.h>
#include <atlstr.h>
#include <atlapp.h>

namespace Res
{
    class Custom: boost::noncopyable
    {
    public:
        Custom();
        Custom(PCTSTR name, PCTSTR type, HMODULE module = ModuleHelper::GetResourceInstance());
        ~Custom();

        CString const& GetMessage() const;

        HRESULT Load(PCTSTR name, PCTSTR type, HMODULE module = ModuleHelper::GetResourceInstance());

    private:
        friend class Data;

        void const* resource_;
        DWORD size_;
        CString message_;
    };
}
