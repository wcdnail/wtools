#pragma once 

#include "string.utils.error.code.h"
#include <stdexcept>
#include <sstream>
#include <ole2.h>

struct OLE
{
    OLE(); /* throw(std::runtime_error) */
    ~OLE(); /* throw() */
};

inline OLE::OLE() /* throw(std::runtime_error) */
{
    HRESULT hr = ::OleInitialize(NULL);
    if (FAILED(hr))
    {
        std::stringstream message;
        message << "OleInitialize failed: " << std::hex << hr << " " 
                << Str::ErrorCode<char>::SystemMessage(hr);

        throw std::runtime_error(message.str());
    }
}

inline OLE::~OLE() /* throw() */
{
    ::OleUninitialize();
}

