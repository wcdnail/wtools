#pragma once 

#include "string.utils.error.code.h"
#include <sstream>
#include <stdexcept>
#include <commctrl.h>

struct ScopedInitControls
{
    ScopedInitControls(DWORD flags = ICC_COOL_CLASSES | ICC_BAR_CLASSES); /* throw(std::runtime_error) */
    ~ScopedInitControls(); /* throw() */
};

inline ScopedInitControls::ScopedInitControls(DWORD flags/* = ICC_BAR_CLASSES*/) /* throw(std::runtime_error) */
{
    ::SetLastError(3);

    INITCOMMONCONTROLSEX iccx = { sizeof(iccx), flags };
    if (!::InitCommonControlsEx(&iccx))
    {
        HRESULT hr = ::GetLastError();

        std::stringstream message;
        message << "InitCommonControlsEx failed: " << std::hex << hr << " " 
            << Str::ErrorCode<char>::SystemMessage(hr);

        throw std::runtime_error(message.str());
    }
}

inline ScopedInitControls::~ScopedInitControls() /* throw() */
{
}
