#pragma once

#include <wcdafx.api.h>
#include <list>

class IColorObserver;

class IColor
{
protected:
    WCDAFX_API virtual ~IColor();
    WCDAFX_API IColor();

    WCDAFX_API void NotifyObservers() const;

public:
    WCDAFX_API virtual COLORREF GetColorRef() const = 0;
    WCDAFX_API virtual int GetAlpha() const = 0;

    WCDAFX_API virtual void SetColor(COLORREF crColor, int nAlpha) = 0;
    WCDAFX_API virtual void SetColor(IColor const* pColor);

    WCDAFX_API virtual void AddObserver(IColorObserver& rObserver);
    WCDAFX_API virtual void AddObservers(IColor& rColor);

private:
    using ObserverList = std::list<IColorObserver*>;

    ObserverList m_lstObservers;

    bool IsAlreadyObserved(IColorObserver const* pObserver) const;

    DELETE_COPY_MOVE_OF(IColor);
};
