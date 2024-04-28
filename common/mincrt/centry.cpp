#include "stdafx.h"
#include "cdefines.h"

namespace App { int EntryPoint(); }

static int _EntryPoint()
{
    return App::EntryPoint();
}

_extrnc int WinMainCRTStartup()
{
    return _EntryPoint();
}
