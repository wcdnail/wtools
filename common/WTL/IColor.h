#pragma once

#include <wcdafx.api.h>

class IColor
{
protected:
    WCDAFX_API virtual ~IColor();

public:
    WCDAFX_API virtual COLORREF GetColorRef() const = 0;
    WCDAFX_API virtual int GetAlpha() const = 0;
    WCDAFX_API virtual void SetColor(COLORREF crColor, int nAlpha) = 0;

    WCDAFX_API virtual void SetColor(IColor const* pColor);
};
