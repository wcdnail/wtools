#pragma once 

#include "string.utils.error.code.h"
#include <stdexcept>
#include <sstream>
#include <ole2.h>

struct ScopedInitOLE
{
    ScopedInitOLE(); /* throw(std::runtime_error) */
    ~ScopedInitOLE(); /* throw() */
};

inline ScopedInitOLE::ScopedInitOLE() /* throw(std::runtime_error) */
{
    HRESULT hr = ::OleInitialize(nullptr);
    if (FAILED(hr))
    {
        std::stringstream message;
        message << "OleInitialize failed: " << std::hex << hr << " " 
                << Str::ErrorCode<char>::SystemMessage(hr);

        throw std::runtime_error(message.str());
    }
}

inline ScopedInitOLE::~ScopedInitOLE() /* throw() */
{
    ::OleUninitialize();
}

