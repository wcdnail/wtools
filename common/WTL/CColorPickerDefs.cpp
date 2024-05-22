#include "stdafx.h"
#include "CColorPickerDefs.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <dh.tracing.h>

template <typename T>
constexpr T Max3(T a, T b, T c)
{
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

template <typename T>
constexpr T Min3(T a, T b, T c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

CColorUnion::~CColorUnion() = default;

CColorUnion::CColorUnion(double dH, double dS, double dV)
    : m_bUpdated{false}
    ,       m_dH{dH}
    ,       m_dS{dS}
    ,       m_dV{dV}
    ,      m_Red{0}
    ,    m_Green{0}
    ,     m_Blue{0}
    ,    m_Alpha{0}
{
    UpdateRGB();
}

CColorUnion::CColorUnion(COLORREF crInitial)
    : m_bUpdated{false}
    ,       m_dH{0.}
    ,       m_dS{0.}
    ,       m_dV{0.}
    ,      m_Red{GetRValue(crInitial)}
    ,    m_Green{GetGValue(crInitial)}
    ,     m_Blue{GetBValue(crInitial)}
    ,    m_Alpha{0}
{
    UpdateHSV();
}

void CColorUnion::UpdateRGB()
{
    int             R{0xff};
    int             G{0xff};
    int             B{0xff};
    double const dSat{m_dS / 100.};
    double const dVal{m_dV / 100.};
    if (0 == m_dS) {
        R = B = G = static_cast<int>(255. * dVal);
        SetRGBPlain(R, G, B);
        return;
    }
    auto const nBase{static_cast<int>(255. * (1.0 - dSat) * dVal)};
    auto const  nHue{static_cast<int>(m_dH / 60.)};
    auto const nFrac{static_cast<int>(m_dH) % 60};
    switch (nHue) {
    case 0:
        R = static_cast<int>(255.0 * dVal);
        G = static_cast<int>((255.0 * dVal - nBase) * (m_dH / 60.) + nBase);
        B = nBase;
        break;
    case 1:
        R = static_cast<int>((255. * dVal - nBase) * (1. - (nFrac / 60.)) + nBase);
        G = static_cast<int>(255. * dVal);
        B = nBase;
        break;
    case 2:
        R = nBase;
        G = static_cast<int>(255. * dVal);
        B = static_cast<int>((255. * dVal - nBase) * (nFrac / 60.) + nBase);
        break;
    case 3:
        R = nBase;
        G = static_cast<int>((255. * dVal - nBase) * (1. - (nFrac / 60.)) + nBase);
        B = static_cast<int>(255. * dVal);
        break;
    case 4:
        R = static_cast<int>((255. * dVal - nBase) * (nFrac / 60.) + nBase);
        G = nBase;
        B = static_cast<int>(255. * dVal);
        break;
    case 5:
        R = static_cast<int>(255. * dVal);
        G = nBase;
        B = static_cast<int>((255. * dVal - nBase) * (1. - (nFrac / 60.)) + nBase);
        break;
    default:
        DH::TPrintf(L"%s: NO valid case for %d\n", __FUNCTIONW__, nHue);
        ATLASSERT(false);
        return;
    }
    SetRGBPlain(R, G, B);
}

void CColorUnion::UpdateHSV()
{
    int const      R{m_Red};
    int const      G{m_Green};
    int const      B{m_Blue};
    auto const btMax{Max3(R, G, B)};
    if (0 == btMax) {
        m_dH = m_dS = m_dV = 0;
        return;
    }
    auto const btMin{Min3(R, G, B)};
    auto       delta{static_cast<double>(btMax - btMin)};
    double      temp{.0};
    if (delta == 0) {
        delta = 1.;
    }

    m_dV = btMax / (255.0 * 100.0);
    m_dS = (delta / static_cast<double>(btMax)) * 100.0;
    if (btMax == R) {
        temp = (60 * ((G - B) / delta));
    }
    else if (btMax == G) {
        temp = (60 * (2.0 + (B - R) / delta));
    }
    else {
        temp = (60 * (4.0 + (R - G) / delta));
    }
    if (temp < 0) {
        m_dH = temp + 360;
    }
    else {
        m_dH = temp;
    }
    if (m_dV > HSV_VAL_MAX) {
        m_dV = HSV_VAL_MAX;
    }
    if (m_dS > HSV_SAT_MAX) {
        m_dS = HSV_SAT_MAX;
    }
    if (m_dH > HSV_HUE_MAX) {
        m_dH = HSV_HUE_MAX;
    }
    SetUpdated(true);
}

CRGBSpecRect CColorUnion::GetRGBSpectrumRect(SpectrumKind nSpectrumKind) const
{
    CRGBSpecRect rv{};
    switch (nSpectrumKind) {
    case SPEC_RGB_Red: {
        auto const R{GetRed()};
        rv.crLT = RGB(R, 0, 0);
        rv.crRT = RGB(R, 255, 0);
        rv.crLB = RGB(R, 0, 255);
        rv.crRB = RGB(R, 255, 255);
        break;
    }
    case SPEC_RGB_Green:{
        auto const G{GetGreen()};
        rv.crLT = RGB(0, G, 0);
        rv.crRT = RGB(255, G, 0);
        rv.crLB = RGB(0, G, 255);
        rv.crRB = RGB(255, G, 255);
        break;
    }
    case SPEC_RGB_Blue:{
        auto const B{GetBlue()};
        rv.crLT = RGB(0, 0, B);
        rv.crRT = RGB(0, 255, B);
        rv.crLB = RGB(255, 0, B);
        rv.crRB = RGB(255, 255, B);
        break;
    }
    default:
        ATLASSERT(FALSE);
        break;
    }
    return rv;
}

CPoint CColorUnion::GetColorPoint(SpectrumKind nSpectrumKind, CRect const& rc)
{
    double xScale = 1.;
    double yScale = 1.;

    switch (nSpectrumKind) {
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue:
        xScale = static_cast<double>(rc.Width()) / RGB_MAX;
        yScale = static_cast<double>(rc.Height()) / RGB_MAX;
        break;
    case SPEC_HSV_Hue:
        xScale = rc.Width() / HSV_SAT_MAX;
        yScale = rc.Height() / HSV_VAL_MAX;
        break;
    case SPEC_HSV_Saturation:
        xScale = rc.Width() / HSV_HUE_MAX;
        yScale = rc.Height() / HSV_VAL_MAX;
        break;
    case SPEC_HSV_Brightness:
        xScale = rc.Width() / HSV_HUE_MAX;
        yScale = rc.Height() / HSV_SAT_MAX;
        break;
    default:
        break;
    }
    CPoint rv{0, 0};
    switch (nSpectrumKind) {
    case SPEC_RGB_Red:          rv.SetPoint(static_cast<int>(GetGreen() * xScale), static_cast<int>(GetBlue() * yScale)); break;
    case SPEC_RGB_Green:        rv.SetPoint(static_cast<int>(GetRed() * xScale), static_cast<int>(GetBlue() * yScale)); break;
    case SPEC_RGB_Blue:         rv.SetPoint(static_cast<int>(GetGreen() * xScale), static_cast<int>(GetRed() * yScale)); break;
    case SPEC_HSV_Hue:          rv.SetPoint(static_cast<int>(m_dS * xScale), static_cast<int>(rc.Height() - (m_dV * yScale))); break;
    case SPEC_HSV_Saturation:   rv.SetPoint(static_cast<int>(m_dH * xScale), static_cast<int>(rc.Height() - (m_dV * yScale))); break;
    case SPEC_HSV_Brightness:   rv.SetPoint(static_cast<int>(m_dH * xScale), static_cast<int>(rc.Height() - (m_dS * yScale))); break;
    default:
        break;
    }
    return rv;
}
