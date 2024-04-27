#include "stdafx.h"
#include "crt.defs.h"

extern "C" int _MCDECL atexit(void (_MCDECL *somestuff)())
{
    return MinCrt::Crt().AddGlobal(somestuff);
}
