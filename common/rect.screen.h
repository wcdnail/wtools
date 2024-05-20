#pragma once

#include <winuser.h>
#include <atltypes.h>

namespace Rc
{
    const CRect Screen{0, 0,
        GetSystemMetrics(SM_CXMAXIMIZED) - (GetSystemMetrics(SM_CXSIZEFRAME) * 2 + GetSystemMetrics(SM_CXFRAME) * 2),
        GetSystemMetrics(SM_CYMAXIMIZED) - (GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYFRAME) * 2)
    };
}
