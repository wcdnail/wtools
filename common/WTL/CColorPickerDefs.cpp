#include "stdafx.h"
#include "CColorPickerDefs.h"
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
    auto const base{static_cast<int>(255. * (1.0 - dSat) * dVal)};
    auto const inth{static_cast<int>(m_dH / 60.)};
    auto const frac{static_cast<int>(m_dH) % 60};
    switch (inth) {
    case 0:
        R = static_cast<int>(255.0 * dVal);
        G = static_cast<int>((255.0 * dVal - base) * (m_dH / 60.) + base);
        B = base;
        break;
    case 1:
        R = static_cast<int>((255. * dVal - base) * (1. - (frac / 60.)) + base);
        G = static_cast<int>(255. * dVal);
        B = base;
        break;
    case 2:
        R = base;
        G = static_cast<int>(255. * dVal);
        B = static_cast<int>((255. * dVal - base) * (frac / 60.) + base);
        break;
    case 3:
        R = base;
        G = static_cast<int>((255. * dVal - base) * (1. - (frac / 60.)) + base);
        B = static_cast<int>(255. * dVal);
        break;
    case 4:
        R = static_cast<int>((255. * dVal - base) * (frac / 60.) + base);
        G = base;
        B = static_cast<int>(255. * dVal);
        break;
    case 5:
        R = static_cast<int>(255. * dVal);
        G = base;
        B = static_cast<int>((255. * dVal - base) * (1. - (frac / 60.)) + base);
        break;
    default:
        DH::TPrintf(L"%s: NO valid case for %d\n", __FUNCTIONW__, inth);
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
    auto const btMin{Min3(R, G, B)};
    auto const delta{static_cast<double>(btMax - btMin)};
    double      temp{.0};
    if (0 == btMax) {
        m_dH = m_dS = m_dV = 0;
        return;
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
    SetUpdated(true);
}
