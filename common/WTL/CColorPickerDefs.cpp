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

//
// Refactored from https://github.com/jakebesworth/Simple-Color-Conversions/blob/master/color.c
// by Jake Besworth
// 
void HSVtoRGB(double const dH, double const dS, double const dV, int& R, int& G, int& B)
{
    double const       dHex{dH / 60.};
    int const      nPrimary{static_cast<int>(dHex)};
    double const nSecondary{dHex - nPrimary};
    double const         dX{(1. - dS) * dV};
    double const         dY{(1. - (dS * nSecondary)) * dV};
    double const         dZ{(1. - (dS * (1. - nSecondary))) * dV};
    double               dR{255.};
    double               dG{255.};
    double               dB{255.};
    switch (nPrimary) {
    default:
    case 0:
        dR = (dV * 255.) + 0.5;
        dG = (dZ * 255.) + 0.5;
        dB = (dX * 255.) + 0.5;
        break;
    case 1:
        dR = (dY * 255.) + 0.5;
        dG = (dV * 255.) + 0.5;
        dB = (dX * 255.) + 0.5;
        break;
    case 2:
        dR = (dX * 255.) + 0.5;
        dG = (dV * 255.) + 0.5;
        dB = (dZ * 255.) + 0.5;
        break;
    case 3:
        dR = (dX * 255.) + 0.5;
        dG = (dY * 255.) + 0.5;
        dB = (dV * 255.) + 0.5;
        break;
    case 4:
        dR = (dZ * 255.) + 0.5;
        dG = (dX * 255.) + 0.5;
        dB = (dV * 255.) + 0.5;
        break;
    case 5:
        dR = (dV * 255.) + 0.5;
        dG = (dX * 255.) + 0.5;
        dB = (dY * 255.) + 0.5;
        break;
    }
    R = static_cast<int>(dR);
    G = static_cast<int>(dG);
    B = static_cast<int>(dB);
}

//
// Refactored from https://github.com/jakebesworth/Simple-Color-Conversions/blob/master/color.c
// Copyright 2016 Jake Besworth
// 
void RGBtoHSV(int const R, int const G, int const B, double& dH, double& dS, double& dV)
{
    int const    nMax{Max3(R, G, B)};
    int const    nMin{Min3(R, G, B)};
    double const dMax{nMax / 255.0};
    double const dMin{nMin / 255.0};

    dS = (dMax < 0.0001) ? 0 : (dMax - dMin) / dMax;
    dV = dMax;

    /* Saturation is 0 */
    if(dS < 0.1) {
        /* Hue is undefined, monochrome */
        dH = 0;
        return;
    }
    if(R == nMax) {
        if(G == nMin) {
            /* H = 5 + B' */
            dH = 5 + ((dMax - (B / 255.)) / (dMax - dMin));
        }
        else {
            /* H = 1 - G' */
            dH = 1 - ((dMax - (G / 255.)) / (dMax - dMin));
        }
    }
    else if(G == nMax) {
        if(B == nMin) {
            /* H = R' + 1 */
            dH = ((dMax - (R / 255.)) / (dMax - dMin)) + 1;
        }
        else {
            /* H = 3 - B' */
            dH = 3 - ((dMax - (B / 255.)) / (dMax - dMin));
        }
    }
    /* This is actually a problem with the original paper, I've fixed it here, should email them... */
    else if(B == nMax && R == nMin) {
        /* H = 3 + G' */
        dH = 3 + ((dMax - (G / 255.)) / (dMax - dMin));
    }
    else {
        /* H = 5 - R' */
        dH = 5 - ((dMax - (R / 255.)) / (dMax - dMin));
    }

    /* Hue is then converted to degrees by multiplying by 60 */
    dH = std::min<double>(dH * 60., HSV_HUE_MAX);
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
    ,    m_Alpha{0xff}
{
    HSVtoRGB();
}

CColorUnion::CColorUnion(COLORREF crInitial)
    : m_bUpdated{false}
    ,       m_dH{0.}
    ,       m_dS{0.}
    ,       m_dV{0.}
    ,      m_Red{GetRValue(crInitial)}
    ,    m_Green{GetGValue(crInitial)}
    ,     m_Blue{GetBValue(crInitial)}
    ,    m_Alpha{0xff}
{
    RGBtoHSV();
}

#if 0
void CColorUnion::RGBtoHSV()
{
    auto const    R{m_Red / RGB_MAX};
    auto const    G{m_Green / RGB_MAX};
    auto const    B{m_Blue / RGB_MAX};
    auto const dMax{Max3(R, G, B)};
    if (0 == dMax) {
        m_dH = m_dS = m_dV = 0;
        return;
    }
    auto const   dMin{Min3(R, G, B)};
    auto const dDelta{dMax - dMin};

    if (dDelta == 0.) {
        m_dH = 0.;
    }
    else if(R == dMax) {
        m_dH = std::fmod((60. * ((G - B) / dDelta)) + HSV_HUE_MAX, HSV_HUE_MAX);
    }
    else if (G == dMax) {
        m_dH = std::fmod((60. * ((B - R) / dDelta)) + HSV_HUE_MID, HSV_HUE_MAX);
    }
    else if (B == dMax) {
        m_dH = std::fmod((60. * ((R - G) / dDelta)) + HSV_HUE_AMID, HSV_HUE_MAX);
    }
    if (dMax == 0.) {
        m_dS = 0.;
    }
    else {
        m_dS = HSV_SAT_MAX * (dDelta / dMax);
    }
    m_dV = dMax * HSV_VAL_MAX;

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
#endif


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
