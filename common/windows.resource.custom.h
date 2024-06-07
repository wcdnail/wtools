#pragma once

#include <wcdafx.api.h>
#include <wtypes.h>
#include <atlstr.h>
#include <atlapp.h>

namespace Res
{
    class Custom
    {
    public:
        DELETE_COPY_MOVE_OF(Custom);

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
