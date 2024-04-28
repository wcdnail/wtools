#include "stdafx.h"
#include "crt.report.h"
#include <stdexcept>

namespace std
{
    void __cdecl _Xlength_error(char const* expression)
    {
        ::TraceA("MinCrt: `%s` is length error\n", expression);
        throw length_error(expression);
    }
}
