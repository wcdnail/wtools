#pragma once

#include <wcdafx.api.h>
#include <atlgdi.h>

class CRect;

struct CColorCell
{
    COLORREF crColor;
    int       nAlpha;

    DELETE_COPY_OF(CColorCell);
    ~CColorCell();
    CColorCell(COLORREF crClr, int nA);

    CColorCell(CColorCell&& rhs) noexcept;
    CColorCell& operator = (CColorCell&& rhs) noexcept;
    void Swap(CColorCell& rhs) noexcept;

    HDC GetDC(HDC dc, CRect const& rc);
    bool operator == (const CColorCell&) const;

    void Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack);

private:
    HBITMAP       m_hPrev;
    WTL::CDC         m_DC;
    WTL::CBitmap m_Bitmap;
};
