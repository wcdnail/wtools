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

CColorCell::CColorCell(COLORREF crColor, int nAlpha, HWND hHolder /*= nullptr*/)
    : m_crColor{crColor}
    ,  m_nAlpha{nAlpha}
    , m_hHolder{hHolder}
    ,   m_hPrev{nullptr}
{
}

CColorCell::CColorCell(CColorCell&& rhs) noexcept
    : m_crColor{rhs.m_crColor}
    ,  m_nAlpha{rhs.m_nAlpha}
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
    std::swap(m_crColor, rhs.m_crColor);
    std::swap(m_nAlpha, rhs.m_nAlpha);
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
        m_DC.FillSolidRect(rc, m_crColor);
    }
    return m_DC.m_hDC;
}

bool CColorCell::operator==(const CColorCell& lhs) const
{
    return m_nAlpha == lhs.m_nAlpha &&
          m_crColor == lhs.m_crColor;
}

void CColorCell::Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack)
{
    if (m_nAlpha >= RGB_MAX_INT) {
        dc.FillSolidRect(rc, m_crColor);
        return ;
    }
    CRect const        rcColor{0, 0, 1, 1};
    WTL::CDCHandle const dcClr{GetDC(dc, rcColor)};
    BLENDFUNCTION const  blend{AC_SRC_OVER, 0, static_cast<BYTE>(m_nAlpha), 0};
    dc.SelectBrush(brBack);
    dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
    dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(),
                  dcClr, 0, 0, rcColor.Width(), rcColor.Height(), blend);
}

void CColorCell::SetColor(COLORREF crColor, int nAlpha)
{
    m_crColor = crColor;
    m_nAlpha = nAlpha;
    if (m_hHolder) {
        InvalidateRect(m_hHolder, nullptr, FALSE);
    }
}

CColorCellEx::~CColorCellEx() = default;

CColorCellEx::CColorCellEx(COLORREF crColor, int nAlpha, HWND hHolder)
    : m_clCell{crColor, nAlpha, hHolder}
{
}

COLORREF CColorCellEx::GetColorRef() const
{
    return m_clCell.GetColorRef();
}

int CColorCellEx::GetAlpha() const
{
    return m_clCell.GetAlpha();
}

void CColorCellEx::SetColor(COLORREF crColor, int nAlpha)
{
    m_clCell.SetColor(crColor, nAlpha);
    NotifyObservers();
}

void CColorCellEx::OnColorUpdate(IColor const& clrSource)
{
    m_clCell.SetColor(clrSource.GetColorRef(), clrSource.GetAlpha());
}
