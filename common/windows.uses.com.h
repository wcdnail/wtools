#pragma once 

#include "string.utils.error.code.h"
#include <stdexcept>
#include <sstream>
#include <objbase.h>

namespace Initialize
{
    struct COM
    {
        static const bool Multithreaded = true; 
        static const bool Singlethreaded = false;

        COM(bool mt = Multithreaded, DWORD method = COINIT_MULTITHREADED); /* throw(std::runtime_error) */
        ~COM(); /* throw() */
    };

    inline COM::COM(bool mt/* = Multithreaded*/, DWORD method/* = COINIT_MULTITHREADED*/) /* throw(std::runtime_error) */
    {
        HRESULT hr;

        if (mt)
            hr = ::CoInitializeEx(NULL, method);
        else
            hr = ::CoInitialize(NULL);

        if (FAILED(hr))
        {
            std::stringstream message;
            message << (mt ? "CoInitializeEx" : "CoInitialize") << " failed: "
                    << std::hex << hr << " " 
                    << Str::ErrorCode<char>::SystemMessage(hr);

            throw std::runtime_error(message.str());
        }
    }

    inline COM::~COM() /* throw() */
    {
        ::CoUninitialize();
    }
}
