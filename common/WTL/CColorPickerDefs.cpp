#include "stdafx.h"
#include "CColorPickerDefs.h"

CColorUnion::~CColorUnion() = default;

CColorUnion::CColorUnion(double dH, double dS, double dV)
    :       m_dH{dH}
    ,       m_dS{dS}
    ,       m_dV{dV}
    , m_bUpdated{false}
{
    UpdateRGB();
}

CColorUnion::CColorUnion(COLORREF crInitial)
    : CColorUnion{0., 0., 100.}
{
    m_Comp.Color = crInitial;
    UpdateHSV();
}

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

void CColorUnion::UpdateRGB()
{
    int             R{0xff};
    int             G{0xff};
    int             B{0xff};
    double const dSat{m_dS / 100.};
    double const dVal{m_dV / 100.};
    if (0 == m_dS) {
        R = B = G = static_cast<int>(255. * dVal);
        SetRGB(R, G, B);
        return;
    }
    auto const base{static_cast<int>(255. * (1.0 - dSat) * dVal)};
    auto const frac{static_cast<int>(m_dH) % 60};
    switch (static_cast<int>(m_dH / 60.)) {
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
    }
    SetRGB(R, G, B);
}

void CColorUnion::UpdateHSV()
{
    int const      R{m_Comp.RGB.m_btRed};
    int const      G{m_Comp.RGB.m_btGreen};
    int const      B{m_Comp.RGB.m_btBlue};
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

void CColorUnion::SetHue(double dHue)
{
    m_dH = std::min<double>(dHue, 359.);
    UpdateRGB();
}
