#pragma once 

#include "string.utils.error.code.h"
#include <stdexcept>
#include <sstream>
#include <atlctrls.h>
#include <boost/noncopyable.hpp>

namespace Initialize
{
    struct RichEdit: boost::noncopyable
    {
        RichEdit(); /* throw(std::runtime_error) */
        ~RichEdit(); /* throw() */

    private:
        HMODULE Lib;
    };

    inline RichEdit::RichEdit() /* throw(std::runtime_error) */
        : Lib(NULL)
    {
        Lib = ::LoadLibrary(WTL::CRichEditCtrl::GetLibraryName());
        if (!Lib)
        {
            HRESULT hr = ::GetLastError();

            std::ostringstream message;
            message << "RichEdit initialize failed: " << std::hex << hr << " " 
                    << Str::ErrorCode<char>::SystemMessage(hr);

            throw std::runtime_error(message.str());
        }
    }

    inline RichEdit::~RichEdit() /* throw() */
    {
        if (Lib)
            ::FreeLibrary(Lib);
    }
}
