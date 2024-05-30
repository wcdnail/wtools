#pragma once

#include "IColor.h"
#include <wcdafx.api.h>
#include <atlgdi.h>

class CRect;

struct CColorCell: public IColor
{
    DELETE_COPY_OF(CColorCell);

    ~CColorCell() override;
    CColorCell(COLORREF crColor, int nAlpha, HWND hHolder = nullptr);

    CColorCell(CColorCell&& rhs) noexcept;
    CColorCell& operator = (CColorCell&& rhs) noexcept;
    void Swap(CColorCell& rhs) noexcept;

    HDC GetDC(HDC dc, CRect const& rc);
    bool operator == (const CColorCell&) const;

    void Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack);

    COLORREF GetColorRef() const override;
    int GetAlpha() const override;
    void SetColor(COLORREF crColor, int nAlpha) override;

    void SetHolder(HWND hHolder);

private:
    COLORREF      m_crColor;
    int            m_nAlpha;
    HWND          m_hHolder;
    HBITMAP         m_hPrev;
    WTL::CDC           m_DC;
    WTL::CBitmap   m_Bitmap;
};

inline void CColorCell::SetHolder(HWND hHolder)
{
    m_hHolder = hHolder;
}
