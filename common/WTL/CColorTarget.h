#pragma once

class IColorTarget
{
protected:
    virtual ~IColorTarget();

public:
    virtual COLORREF GetColorRef() const = 0;
    virtual int GetAlpha() const = 0;
    virtual void SetColor(COLORREF crColor, int nAlpha) = 0;
};

struct CColorTarget
{
    IColorTarget* m_pTarget{nullptr};
    IColorTarget*   m_pHost{nullptr};

    void OnUpdateColor(COLORREF crColor, int nAlpha) const;
    void UpdateHostColor() const;
};
