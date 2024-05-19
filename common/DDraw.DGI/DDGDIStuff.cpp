#include "stdafx.h"
#include "DDGDIStuff.h"
#include "CeXDib.h"
#include <cmath>

//
// This code is refactored from: GTDrawHelper.cpp
// David Swigger's (dwswigger@yahoo.com) Photoshop's-like color pickers
// 15 Jul 2002
// codeproject.com/Articles/2577/Xguiplus-A-set-of-Photoshop-s-like-color-pickers
//

constexpr int IntMult = 20;

constexpr void RGB_Line3(DWORD*& pDest, int nWidth, int vR, int vG, int vB, int iR, int iG, int iB)
{
    for (int j = 0; j < nWidth; j++) {
        *pDest++ = RGB(vR >> IntMult, vG >> IntMult, vB >> IntMult);
        vR += iR;
        vG += iG;
        vB += iB;
    }
}

constexpr void RGB_Line1(DWORD*& pDest, int nWidth, int bRGB, int iR, int iG, int iB)
{
    const int vR{bRGB};
    const int vG{bRGB};
    const int vB{bRGB};
    RGB_Line3(pDest, nWidth, vR, vG, vB, iR, iG, iB);
}

bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue)
{
    if (nWidth <= 0) {
        // TODO: report nWidth - INVAL
        return false;
    }
    //
    // dHue - const, in [0, 360)
    //  intp - const in 0, 1, 2, 3, 4, 5
    //  frac - const in [0, 1)
    // sat - increments, in [0, 1]; indirectly (coefficients)
    // val - const, in [0, (255 << IntMult)]
    //
    // inc1 => val * (1 - sat)              => changes from val to 0
    // inc2 => val * (1 - sat * frac)       => changes from val to val * (1 - frac)
    // inc3 => val * (1 - sat * (1 - frac)) => changes from val to val * frac
    //
    const int   vStart{static_cast<int>(dValue * 255) << IntMult};
    double        dInt{0};
    const double dFrac{modf(dHue / 60.0, &dInt)};
    auto const    nInt{static_cast<int>(dInt)};
    const int     inc1{-vStart / nWidth};
    const int     inc2{static_cast<int>((1 - dFrac) * vStart - vStart) / nWidth};
    const int     inc3{static_cast<int>(dFrac * vStart - vStart) / nWidth};
    switch (nInt) {
    case 0: RGB_Line1(pDest, nWidth, vStart, inc1, inc3, 0); break;
    case 1: RGB_Line1(pDest, nWidth, vStart, inc1, 0, inc2); break;
    case 2: RGB_Line1(pDest, nWidth, vStart, inc3, 0, inc1); break;
    case 3: RGB_Line1(pDest, nWidth, vStart, 0, inc2, inc1); break;
    case 4: RGB_Line1(pDest, nWidth, vStart, 0, inc1, inc3); break;
    case 5: RGB_Line1(pDest, nWidth, vStart, inc2, inc1, 0); break;
    default:
        // TODO: report nInt - INVAL
        return false;
    }
    return true;
}

bool DDraw_HSV_Hue(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue)
{
    if (nHeight <= 0) {
        // TODO: report nHeight - INVAL
        return false;
    }
    // initial and change
    double     dValue{0.0};
    const double dAdv{1.0 / nHeight};
    for (int i = 0; i<nHeight; i++) {
        if (!DDraw_HSV_SAT_Line(pDest, nWidth, dHue, dValue)) {
            return false;
        }
        dValue += dAdv;
        pDest += nLinePitch;
    }
    return true;
}

bool DDraw_HSV_Hue(CDibEx const& dibDest, double dHue)
{
    ATLASSUME(dibDest.GetBitCount() == 32);
    return DDraw_HSV_Hue(reinterpret_cast<DWORD*>(dibDest.GetData()),
        dibDest.GetWidth(),
        dibDest.GetHeight(),
        dibDest.GetStride() / sizeof(DWORD),
        dHue);
}

bool DDraw_HSV_HUE(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSaturation, double dVal)
{
    if (nHeight <= 0) {
        // TODO: report nHeight - INVAL
        return false;
    }
    DDraw_HSV_HUE_Line(pDest, nWidth, dSaturation, dVal);
    for (int i = 1; i<nHeight; i++) {
        memcpy(pDest + nLinePitch, pDest, nWidth * sizeof(DWORD));
        pDest += nLinePitch;
    }
    return true;
}

bool DDraw_HSV_HUE(CDibEx const& dibDest, double dSaturation, double dVal)
{
    ATLASSUME(dibDest.GetBitCount() == 32);
    return DDraw_HSV_HUE(reinterpret_cast<DWORD*>(dibDest.GetData()),
        dibDest.GetWidth(),
        dibDest.GetHeight(),
        dibDest.GetStride() / sizeof(DWORD),
        dSaturation,
        dVal
    );
}

void DDraw_HSV_HUE_Line(DWORD* pDest, int nWidth, double dSaturation, double dVal)
{
    //
    //  dFrac increments in [0, 1) six times; indirectly (coefficients)
    // dSaturation - const, in [0, 1]
    // nVal - const, in [0, (255 << IntMult)]
    //
    // coef1 => val * (1 - dSaturation)               => const, = val * (1 - dSaturation)
    // coef2 => val * (1 - dSaturation * dFrac)       => changes from val to val * (1 - dSaturation)
    // nCoef3 => val * (1 - dSaturation * (1 - dFrac)) => changes from val * (1 - dSaturation) to val
    //

    // current position and advance to the next one
    double           dFrac{0};
    double const dFracStep{static_cast<double>(nWidth) / 6.0};
    // value, but as integer in [0, 255 << IntMult]
    int               nVal{static_cast<int>(dVal * 255.) << IntMult};
    int             nCoef1{static_cast<int>(nVal * (1. - dSaturation))};
    int             nCoef2{nCoef1};
    int             nCoef3{nCoef1};
    int             nRange{0};
    int              nStep{0};

    dFrac += dFracStep;
    nRange = static_cast<int>(dFrac);
    nStep  = (nVal - nCoef3) / (nRange - 1);
    RGB_Line3(pDest, nRange, nCoef1, nCoef3, nVal, 0, nStep, 0);

    dFrac += dFracStep;
    nRange = static_cast<int>(dFrac - (1 * dFracStep));
    nStep  = static_cast<int>((nVal * (1.0 - dSaturation) - nCoef2) / (nRange - 1));
    RGB_Line3(pDest, nRange, nCoef1, nVal, nCoef2, 0, 0, nStep);

    dFrac += dFracStep;
    nRange = static_cast<int>(dFrac - (2 * dFracStep));
    nCoef3 = nCoef1;
    nStep  = (nVal - nCoef3) / (nRange - 1);
    RGB_Line3(pDest, nRange, nCoef3, nVal, nCoef1, nStep, 0, 0);

    dFrac += dFracStep;
    nRange = static_cast<int>(dFrac - (3 * dFracStep));
    nCoef2 = nVal;
    nStep  = static_cast<int>((nVal * (1.0 - dSaturation) - nCoef2) / ((nRange) - 1));
    RGB_Line3(pDest, nRange, nVal, nCoef2, nCoef1, 0, nStep, 0);

    dFrac += dFracStep;
    nRange = static_cast<int>(dFrac - (4 * dFracStep));
    nCoef3 = nCoef1;
    nStep = (nVal - nCoef3) / ((nRange) - 1);
    RGB_Line3(pDest, nRange, nVal, nCoef1, nCoef3, 0, 0, nStep);

    dFrac += (dFracStep + 0.1); // + 0.1 because of floating-point math's rounding errors
    nRange = static_cast<int>(dFrac - (5 * dFracStep));
    nCoef2 = nVal;
    nStep  = static_cast<int>((nVal * (1.0 - dSaturation) - nCoef2) / ((nRange) - 1));
    RGB_Line3(pDest, nRange, nCoef2, nCoef1, nVal, nStep, 0, 0);
}
