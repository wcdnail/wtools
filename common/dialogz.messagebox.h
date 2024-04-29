#pragma once

#include "wcdafx.api.h"
#include "dialogz.types.h"
#include "strint.h"

namespace CF
{
    struct UserDialog
    {
        WCDAFX_API static DialogResult Ask(HWND parent, WStrView what, WStrView title, unsigned flags);
    };
}
