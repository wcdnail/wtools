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

void RGBtoHSL(int const R, int const G, int const B, double& dHl, double& dSl, double& dLl)
{
    double const        dR{R / 255.};
    double const        dG{G / 255.};
    double const        dB{B / 255.};
    double const      dMin{Min3(dR, dG, dB)};
    double const      dMax{Max3(dR, dG, dB)};
    double const dMaxRange{dMax - dMin};
    double const        dL{(dMax + dMin) / 2.0};
    double              dH{0.};
    double              dS{0.};
    if (dMaxRange == 0) {       // This is a gray, no chroma...
        dH = 0.;                // HSL results = 0 ÷ 1
        dS = 0.;
    }
    else {                      // Chromatic data...
        if (dL < 0.5) {
            dS = dMaxRange / (dMax + dMin);
        }
        else {
            dS = dMaxRange / (2.0 - dMax - dMin);
        }

        double const dRRange{((dMax - dR) / 6.0) + (dMaxRange / 2.0) / dMaxRange};
        double const dGRange{((dMax - dG) / 6.0) + (dMaxRange / 2.0) / dMaxRange};
        double const dBRange{((dMax - dB) / 6.0) + (dMaxRange / 2.0) / dMaxRange};
        if (dR == dMax) {
            dH = (dBRange - dGRange);
        }
        else if (dG == dMax) {
            dH = (1.0 / 3.0) + dRRange - dBRange;
        }
        else if (dB == dMax) {
            dH = (2.0 / 3.0) + dGRange - dRRange;
        }

        if (dH < 0) {
            dH += 1;
        }
        if (dH > 1) {
            dH -= 1;
        }
    }
    dHl = dH;
    dSl = dS;
    dLl = dL;
}

IMPL_DEFAULT_COPY_MOVE_OF(CColorUnion);

CColorUnion::~CColorUnion() = default;

CColorUnion::CColorUnion(double dH, double dS, double dV)
    : m_bUpdated{false}
    ,        m_H{static_cast<int>(dH)}
    ,        m_S{static_cast<int>(dS)}
    ,        m_V{static_cast<int>(dV)}
    ,        m_R{0}
    ,        m_G{0}
    ,        m_B{0}
    ,        m_A{0xff}
    ,       m_Hl{0}
    ,       m_Sl{0}
    ,        m_L{0}
{
    HSVtoRGB();
}

CColorUnion::CColorUnion(COLORREF crInitial)
    : m_bUpdated{false}
    ,        m_H{0}
    ,        m_S{0}
    ,        m_V{0}
    ,        m_R{GetRValue(crInitial)}
    ,        m_G{GetGValue(crInitial)}
    ,        m_B{GetBValue(crInitial)}
    ,        m_A{0xff}
    ,       m_Hl{0}
    ,       m_Sl{0}
    ,        m_L{0}
{
    RGBtoHSV();
}

void CColorUnion::FromHSL()
{
    // TODO: IMPL!
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
    double xScale{1.};
    double yScale{1.};
    CPoint     rv{0, 0};
    switch (nSpectrumKind) {
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue:
        xScale = static_cast<double>(rc.Width()) / RGB_MAX;
        yScale = static_cast<double>(rc.Height()) / RGB_MAX;
        switch (nSpectrumKind) {
        case SPEC_RGB_Red:   rv.SetPoint(static_cast<int>(GetGreen() * xScale), static_cast<int>(GetBlue() * yScale)); break;
        case SPEC_RGB_Green: rv.SetPoint(  static_cast<int>(GetRed() * xScale), static_cast<int>(GetBlue() * yScale)); break;
        case SPEC_RGB_Blue:  rv.SetPoint(static_cast<int>(GetGreen() * xScale),  static_cast<int>(GetRed() * yScale)); break;
        default:
            ATLASSERT(FALSE);
            break;
        }
        break;
    case SPEC_HSV_Hue:
        xScale = rc.Width() / HSV_SAT_MAX;
        yScale = rc.Height() / HSV_VAL_MAX;
        rv.SetPoint(static_cast<int>(m_S * xScale), static_cast<int>(rc.Height() - (m_V * yScale)));
        break;
    case SPEC_HSV_Saturation:
        xScale = rc.Width() / HSV_HUE_MAX;
        yScale = rc.Height() / HSV_VAL_MAX;
        rv.SetPoint(static_cast<int>(m_H * xScale), static_cast<int>(rc.Height() - (m_V * yScale)));
        break;
    case SPEC_HSV_Brightness:
        xScale = rc.Width() / HSV_HUE_MAX;
        yScale = rc.Height() / HSV_SAT_MAX;
        rv.SetPoint(static_cast<int>(m_H * xScale), static_cast<int>(rc.Height() - (m_S * yScale)));
        break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    return rv;
}
