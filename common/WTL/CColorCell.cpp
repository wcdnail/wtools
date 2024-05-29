#include "stdafx.h"
#include "CColorCell.h"
#include <color.stuff.h>
#include <atltypes.h>

CColorCell::~CColorCell()
{
    if (m_hPrev) {
        m_DC.SelectBitmap(m_hPrev);
    }
}

CColorCell::CColorCell(COLORREF crClr, int nA)
    : crColor{crClr}
    ,  nAlpha{nA}
    , m_hPrev{nullptr}
{
}

CColorCell::CColorCell(CColorCell&& rhs) noexcept
    : crColor{rhs.crColor}
    ,  nAlpha{rhs.nAlpha}
    , m_hPrev{nullptr}
{
}

CColorCell& CColorCell::operator=(CColorCell&& rhs) noexcept
{
    rhs.Swap(*this);
    return *this;
}

void CColorCell::Swap(CColorCell& rhs) noexcept
{
    std::swap(crColor, rhs.crColor);
    std::swap(nAlpha, rhs.nAlpha);
    std::swap(m_hPrev, rhs.m_hPrev);
    std::swap(m_DC.m_hDC, rhs.m_DC.m_hDC);
    std::swap(m_Bitmap.m_hBitmap, rhs.m_Bitmap.m_hBitmap);
}

HDC CColorCell::GetDC(HDC dc, CRect const& rc)
{
    if (!m_DC) {
        if (!m_Bitmap) {
            m_Bitmap = CreateCompatibleBitmap(dc, rc.Width(), rc.Height());
        }
        m_DC.CreateCompatibleDC(dc);
        m_hPrev = m_DC.SelectBitmap(m_Bitmap);
        m_DC.FillSolidRect(rc, crColor);
    }
    return m_DC.m_hDC;
}

bool CColorCell::operator==(const CColorCell& lhs) const
{
    return nAlpha == lhs.nAlpha &&
          crColor == lhs.crColor;
}

void CColorCell::Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack)
{
    if (nAlpha >= RGB_MAX_INT) {
        dc.FillSolidRect(rc, crColor);
        return ;
    }
    CRect const        rcColor{0, 0, 1, 1};
    WTL::CDCHandle const dcClr{GetDC(dc, rcColor)};
    BLENDFUNCTION const  blend{AC_SRC_OVER, 0, static_cast<BYTE>(nAlpha), 0};
    dc.SelectBrush(brBack);
    dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
    dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(),
                  dcClr, 0, 0, rcColor.Width(), rcColor.Height(), blend);
}
