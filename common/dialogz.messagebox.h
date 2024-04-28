#pragma once

#include "wcdafx.api.h"
#include "dialogz.types.h"

namespace CF
{
    struct UserDialog
    {
        WCDAFX_API static DialogResult Ask(HWND parent, wchar_t const* what, wchar_t const* title, unsigned flags);
    };
}
