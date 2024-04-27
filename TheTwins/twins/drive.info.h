#pragma once

#include "drive.enum.h"
#include <atlstr.h>

namespace Twins
{
    namespace Drive
    {
        extern bool IsReady(Enumerator::Item const& info);
        extern CString GetHint(Enumerator::Item const& info);
    }
}
