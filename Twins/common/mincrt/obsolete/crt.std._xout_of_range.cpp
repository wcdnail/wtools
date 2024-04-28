#include "stdafx.h"
#include "crt.report.h"
#include <stdexcept>

namespace std
{
    void __cdecl _Xout_of_range(char const* expression)
    {
        ::TraceA("MinCrt: `%s` is out of range\n", expression);
        throw out_of_range(expression);
    }
}
