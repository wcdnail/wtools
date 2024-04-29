#pragma once

#include "wcdafx.api.h"

namespace CF
{
}

#ifdef _DEBUG
#  define DEBUG_FIELD_DECL(Type, Name)           Type Name
#  define DEBUG_FIELD_CTOR_INIT0(Name)           , Name()
#  define DEBUG_FIELD_CTOR_INIT1(Name, Value)    , Name(Value)
#else
#  define DEBUG_FIELD_DECL(Type, Name)
#  define DEBUG_FIELD_CTOR_INIT0(Name)
#  define DEBUG_FIELD_CTOR_INIT1(Name, Value)
#endif
