#pragma once

#include <atlstr.h>

namespace Str
{
    template <class Ct = wchar_t>
    struct ErrorCode
    {
        typedef typename Ct* RawStringType;
        typedef typename CStringT< Ct, StrTraitATL< Ct, ChTraitsCRT< Ct > > > StringType;

        template <class CodeType>
        static StringType SystemMessage(CodeType code)
        {
            StringType result;

            HLOCAL local = NULL;
            DWORD len = ATL::ChTraitsOS<Ct>::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL, code, LANG_USER_DEFAULT, (RawStringType)&local, 0, NULL);

            if (len > 2) { // CRLF
                RawStringType message = static_cast<RawStringType>(::LocalLock(local));
                if (message) {
                    message[len-2] = 0; // del CRLF
                    result = message;
                    ::LocalUnlock(local);
                }
                ::LocalFree(local);
            }

            return result;
        }

        template <class CodeType>
        static StringType SystemMessage2(CodeType code)
        {
            static const Ct OK[] = { (Ct)'O', (Ct)'k', 0 };
            return 0 == code ? StringType(OK) : SystemMessage(code);
        }
    };
}
