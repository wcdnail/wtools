#include "stdafx.h"
#include "file.operation.generic.h"

namespace Twins
{
    SpecFlags& GetGlobalOperationFlags()
    {
        static SpecFlags flags(SpecFlags::AskUser | SpecFlags::SkipError);
        return flags;
    }
}
