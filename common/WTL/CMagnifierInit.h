#pragma once

#include <wcdafx.api.h>

struct CMagnifierInit
{
    WCDAFX_API static CMagnifierInit& Instance();

private:
    CMagnifierInit();
    ~CMagnifierInit();
};
