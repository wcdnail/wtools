#include "stdafx.h"
#include "color.stuff.h"
#include <wingdi.h>
#include <cstdint>
#include <cmath>

namespace CF
{
    COLORREF ReduceColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B)
    {
        return RGB(GetRValue(color) < R ? 0 : GetRValue(color) - R,
                   GetGValue(color) < G ? 0 : GetGValue(color) - G,
                   GetBValue(color) < B ? 0 : GetBValue(color) - B);
    }

    COLORREF IncreaseColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B)
    {
        return RGB(std::min<uint32_t>(GetRValue(color) + R, 255),
                   std::min<uint32_t>(GetGValue(color) + G, 255),
                   std::min<uint32_t>(GetBValue(color) + B, 255));
    }

    COLORREF InvertColor(COLORREF color)
    {
        return RGB(255 - GetRValue(color),
                   255 - GetGValue(color),
                   255 - GetBValue(color));
    }

    COLORREF CalcContrastColor(COLORREF color, COLORREF tolerance /*= 0x10*/)
    {
        if (   (abs(static_cast<int>((color) & 0xff) - 0x80) <= static_cast<int>(tolerance))
            && (abs(static_cast<int>((color >> 8) & 0xff) - 0x80) <= static_cast<int>(tolerance))
            && (abs(static_cast<int>((color >> 16) & 0xff) - 0x80) <= static_cast<int>(tolerance))
            ) {
            return (0x7f7f7f + color) & 0xffffff;
        }
        return color ^ 0xffffff;
    }

    COLORREF ComplementColor(COLORREF color)
    { 
        return (0xff - (color & 0xff))
            | ((0xff - ((color >> 8) & 0xff)) << 8)
            | ((0xff - ((color >> 16) & 0xff)) << 16)
            ;
    }

    //
    // Refactored from https://github.com/jakebesworth/Simple-Color-Conversions/blob/master/color.c
    // by Jake Besworth
    // 
    void HSVtoRGB(double const dH, double const dS, double const dV, int& R, int& G, int& B)
    {
        double const   dHex{dH / 60.};
        int const      nSeg{static_cast<int>(dHex)};
        double const  nPrev{dHex - nSeg};
        double const     dX{(1. - dS) * dV};
        double const     dY{(1. - (dS * nPrev)) * dV};
        double const     dZ{(1. - (dS * (1. - nPrev))) * dV};
        double           dR{0.};
        double           dG{0.};
        double           dB{0.};
        switch (nSeg) {
        default: // HUE MAX == 360, nPrimary may be 6
        case 0: dR = dV; dG = dZ; dB = dX; break;
        case 1: dR = dY; dG = dV; dB = dX; break;
        case 2: dR = dX; dG = dV; dB = dZ; break;
        case 3: dR = dX; dG = dY; dB = dV; break;
        case 4: dR = dZ; dG = dX; dB = dV; break;
        case 5: dR = dV; dG = dX; dB = dY; break;
        }
        R = static_cast<int>((dR * RGB_MAX) + 0.5);
        G = static_cast<int>((dG * RGB_MAX) + 0.5);
        B = static_cast<int>((dB * RGB_MAX) + 0.5);
    }

    //
    // https://en.wikipedia.org/wiki/HSL_and_HSV#Color_conversion_formulae
    //
    void HSLtoRGB(double const dH, double const dS, double const dL, int& R, int& G, int& B)
    {
        double const dHex{dH / 60.};
        double const   dC{(1 - std::abs(2. * dL - 1.)) * dS};
        double const   dX{dC * (1 - std::abs(std::fmod(dHex, 2) - 1))};
        double const   dM{dL - (dC / 2.)};
        int const    nSeg{static_cast<int>(dHex)};
        double         dR{0.};
        double         dG{0.};
        double         dB{0.};
        switch (nSeg) {
        default: // HUE MAX == 360, nSeg may be 6
        case 0: dR = dC; dG = dX; dB = 0.; break;
        case 1: dR = dX; dG = dC; dB = 0.; break;
        case 2: dR = 0.; dG = dC; dB = dX; break;
        case 3: dR = 0.; dG = dX; dB = dC; break;
        case 4: dR = dX; dG = 0.; dB = dC; break;
        case 5: dR = dC; dG = 0.; dB = dX; break;
        }
        R = static_cast<int>((dR + dM) * RGB_MAX);
        G = static_cast<int>((dG + dM) * RGB_MAX);
        B = static_cast<int>((dB + dM) * RGB_MAX);
    }

    //
    // Refactored from https://github.com/jakebesworth/Simple-Color-Conversions/blob/master/color.c
    // by Jake Besworth
    // 
    void RGBtoHSV(int const R, int const G, int const B, double& dH, double& dS, double& dV)
    {
        int const    nMax{std::max<int>(R, std::max<int>(G, B))};
        int const    nMin{std::min<int>(R, std::min<int>(G, B))};
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

    void RGBtoHSL1(int const R, int const G, int const B, double& dHl, double& dSl, double& dLl)
    {
        double const        dR{R / 255.};
        double const        dG{G / 255.};
        double const        dB{B / 255.};
        double const      dMin{std::min<double>(dR, std::min<double>(dG, dB))};
        double const      dMax{std::max<double>(dR, std::max<double>(dG, dB))};
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
        dHl = dH * HSV_HUE_MAX;
        dSl = dS * HSV_100PERC;
        dLl = dL * HSV_100PERC;
    }

    void RGBtoHSL2(int const R, int const G, int const B, double& dHl, double& dSl, double& dLl)
    {
        double const        dR{R / 255.};
        double const        dG{G / 255.};
        double const        dB{B / 255.};
        double const      dMin{std::min<double>(dR, std::min<double>(dG, dB))};
        double const      dMax{std::max<double>(dR, std::max<double>(dG, dB))};
        double const    dRange{dMax - dMin};
        double const        dL{50 * (dMin + dMax)};
        double              dS{0.};
        double              dH{0.};
        if (dRange != 0.) {
            if (dL < 50.) {
                dS = 100. * dRange / (dMax + dMin);
            }
            else {
                dS = 100. * dRange / (2. - dRange);
            }
            if (dMax == dR) {
                dH = 60. * (dG - dB) / dRange;
            }
            if (dMax == dG) {
                dH = 60. * (dB - dR) / dRange + 120.;
            }
            if (dMax == dB) {
                dH = 60. * (dR - dG) / dRange + 240.;
            }
            if (dH < 0.) {
                dH = dH + 360.;
            }
        }
        dHl = dH;
        dSl = dS;
        dLl = dL;
    }

    void HSVtoHSL(double const dH, double const dS, double const dV, double& dHl, double& dSl, double& dLl)
    {
        dHl = dH;
        dLl = dV * (1 - (dS / 2.));
        if (0. == dLl || 1. == dLl) {
            dSl = 0.;
        }
        else {
            dSl = (dV - dLl) / std::min<double>(dLl, 1. - dLl);
        }
    }
}
