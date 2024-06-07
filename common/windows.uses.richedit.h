#pragma once 

#include <wcdafx.api.h>
#include <string.utils.error.code.h>
#include <stdexcept>
#include <sstream>
#include <atlctrls.h>

struct ScopedInitRichEdit
{
    DELETE_COPY_MOVE_OF(ScopedInitRichEdit);

    ScopedInitRichEdit(); /* throw(std::runtime_error) */
    ~ScopedInitRichEdit(); /* throw() */

private:
    HMODULE Lib;
};

inline ScopedInitRichEdit::ScopedInitRichEdit() /* throw(std::runtime_error) */
    : Lib(nullptr)
{
    Lib = ::LoadLibrary(WTL::CRichEditCtrl::GetLibraryName());
    if (!Lib)
    {
        HRESULT hr = ::GetLastError();

        std::ostringstream message;
        message << "ScopedInitRichEdit initialize failed: " << std::hex << hr << " " 
                << Str::ErrorCode<char>::SystemMessage(hr);

        throw std::runtime_error(message.str());
    }
}

inline ScopedInitRichEdit::~ScopedInitRichEdit() /* throw() */
{
    if (Lib) {
        FreeLibrary(Lib);
    }
}
