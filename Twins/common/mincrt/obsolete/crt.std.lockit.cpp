#include "stdafx.h"
#include <yvals.h>

namespace std
{
    _Lockit::_Lockit(int kind)
        : _Locktype(0)
    {
        _Lockit_ctor(this, kind);
    }

    _Lockit::~_Lockit()
    {
        _Lockit_dtor(this);
    }

    void _Lockit::_Lockit_ctor(_Lockit* lk, int kind)
    {
        // TODO: lock
    }

    void _Lockit::_Lockit_dtor(_Lockit* lk)
    {
        // TODO: unlock
    }
}
