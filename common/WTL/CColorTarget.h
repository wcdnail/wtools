#pragma once

#include <wcdafx.api.h>

class IColorTarget
{
protected:
    WCDAFX_API virtual ~IColorTarget();
    WCDAFX_API IColorTarget();

    WCDAFX_API virtual void SourceSetColor(IColorTarget const& trgSource);
    WCDAFX_API virtual void TargetColorUpdate(COLORREF crColor, int nAlpha);

public:
    WCDAFX_API virtual COLORREF GetColorRef() const = 0;
    WCDAFX_API virtual int GetAlpha() const = 0;
    WCDAFX_API virtual void SetColor(COLORREF crColor, int nAlpha) = 0;

    WCDAFX_API virtual void SetColorTarget(IColorTarget& rTarget);

    DELETE_COPY_MOVE_OF(IColorTarget);
};
