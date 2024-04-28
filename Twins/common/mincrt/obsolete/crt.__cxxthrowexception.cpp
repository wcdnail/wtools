#include "stdafx.h"
#include "crt.defs.h"

extern "C" 
{
    void _MSTDCALL _CxxThrowException(void* object, _ThrowInfo* info)
    {
        ULONG_PTR params[4] = {0};

        params[0] = MinCrt::CppExceptionMagic;
        params[1] = (ULONG_PTR)object;
        params[2] = (ULONG_PTR)info;
        params[3] = (ULONG_PTR)::GetLastError();

        ::RaiseException(MinCrt::CppExceptionCode, 0, _countof(params), params);
    }
}
