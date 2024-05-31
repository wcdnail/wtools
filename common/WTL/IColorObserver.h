#pragma once

#include <wcdafx.api.h>

class IColor;

class IColorObserver
{
protected:
    WCDAFX_API virtual ~IColorObserver();
    WCDAFX_API IColorObserver();

public:
    WCDAFX_API virtual void OnColorUpdate(IColor const& clrSource) = 0;

    DELETE_COPY_MOVE_OF(IColorObserver);
};
