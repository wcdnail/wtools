#include "stdafx.h"

#if 0
#include "luicTheme.h"
#include "luicAppearance.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "resz/resource.h"

COLORREF CTheme::GetColor(int color) const
{
    if (color < 0 || color >= CLR_Count) {
        return CLR_INVALID;
    }
    return m_Color[color];
}

bool CTheme::SetColor(int iColor, COLORREF clrNew)
{
    if (iColor < 0 || (iColor > CLR_Count - 1)) {
        return false;
    }
    if (clrNew == m_Color[iColor]) {
        // Same color...
        return false;
    }
    // TODO: implement history...
    m_Color[iColor] = clrNew;
    WTL::CBrush tmpBrush;
    tmpBrush.CreateSolidBrush(m_Color[iColor]);
    m_Brush[iColor].Attach(tmpBrush.Detach());
    return true;
}

void CTheme::SetFont(int iFont, const WTL::CLogFont& lfNew, HFONT fnNew)
{
    GetLogFont(iFont) = lfNew;
    GetFont(iFont) = fnNew;
}

WTL::CBrush const& CTheme::GetBrush(int color) const
{
    if (color < 0 || color > CLR_Count - 1) {
        return m_brReserved;
    }
    return m_Brush[color];
}

WTL::CFont const& CTheme::GetFont(int font) const
{
    return GetNcMetricHFontT<WTL::CFont const&, CTheme const&>(*this, font);
}

WTL::CFont& CTheme::GetFont(int font)
{
    return GetNcMetricHFontT<WTL::CFont&, CTheme&>(*this, font);
}

SizeRange const* CTheme::GetSizeRange(int metric) const
{
    if ((metric < 0) || (metric > SIZES_Count - 1)) {
        return nullptr;
    }
    return &m_SizeRange[metric];
}

#endif
