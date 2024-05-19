#include "stdafx.h"
#include "DDGDIStuff.h"
#include "CeXDib.h"
#include <cmath>

#include "exa/gt.color.dialog/GTDrawHelper.h"

//
// This code is refactored from: GTDrawHelper.cpp
// David Swigger's (dwswigger@yahoo.com) Photoshop's-like color pickers
// 15 Jul 2002
// codeproject.com/Articles/2577/Xguiplus-A-set-of-Photoshop-s-like-color-pickers
//

constexpr int IntMult = 20;

static void RGB_Line(DWORD* pDest, int nWidth, int vStart, int iR, int iG, int iB)
{
    int vR{vStart};
    int vG{vStart};
    int vB{vStart};
    for (int j = 0; j < nWidth; j++) {
        *pDest++ = RGB(vR >> IntMult, vG >> IntMult, vB >> IntMult);
        vR += iR;
        vG += iG;
        vB += iB;
    }
}

bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue)
{
    if (nWidth <= 0) {
        // TODO: report nHeight - INVAL
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
    case 0: RGB_Line(pDest, nWidth, vStart, inc1, inc3, 0); break;
    case 1: RGB_Line(pDest, nWidth, vStart, inc1, 0, inc2); break;
    case 2: RGB_Line(pDest, nWidth, vStart, inc3, 0, inc1); break;
    case 3: RGB_Line(pDest, nWidth, vStart, 0, inc2, inc1); break;
    case 4: RGB_Line(pDest, nWidth, vStart, 0, inc1, inc3); break;
    case 5: RGB_Line(pDest, nWidth, vStart, inc2, inc1, 0); break;
    default:
        // TODO: report nInt - INVAL
        return false;
    }
    return true;
}

bool DDraw_HSV_Hue(DWORD* pDest, int nWidth, int nHeight, int nLinePitch, double dHue)
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

void DDraw_HSV_HUE_Line(DWORD *buffer, int samples, double sat, double val_fp)
{
    // value, but as integer in [0, 255 << IntMult]
    int     val;

    // loop counter
    int     j;

    // coefficients and advances
    int     coef1, coef2, coef3;
    int     coef2_adv, coef3_adv;

    // current position and advance to the next one
    double  pos, pos_adv;

    //
    // hue increments in [0, 360); indirectly
    //  intp changes - 0, 1, 2, 3, 4, 5; indirectly (separate loops)
    //  frac increments in [0, 1) six times; indirectly (coefficients)
    // sat - const, in [0, 1]
    // val - const, in [0, (255 << IntMult)]
    //
    // coef1 => val * (1 - sat)              => const, = val * (1 - sat)
    // coef2 => val * (1 - sat * frac)       => changes from val to val * (1 - sat)
    // coef3 => val * (1 - sat * (1 - frac)) => changes from val * (1 - sat) to val
    //

    // constants
    val = (int) (val_fp * 255) << IntMult;
    coef1 = (int) (val * (1 - sat));

    // prepare
    pos = 0;
    pos_adv = (double) samples / 6.0;

    // hue in [0, 60)
    pos += pos_adv;
    j = (int) pos;
    coef3 = coef1, coef3_adv = (int) ((val - coef3) / ((j) - 1));
    while (j--) {
        *buffer++ = RGB((BYTE) (coef1 >> int_extend),(BYTE) (coef3 >> int_extend),(BYTE) (val >> int_extend));
        coef3 += coef3_adv;
    }

    pos += pos_adv;
    j = (int) pos - (int) (1 * pos_adv);
    coef2 = val, coef2_adv = (int) ((val * (1.0 - sat) - coef2) / ((j) - 1));
    while (j--) {
        *buffer++ = RGB ((BYTE) (coef1 >> int_extend),(BYTE) (val >> int_extend),(BYTE) (coef2 >> int_extend));
        coef2 += coef2_adv;
    }

    pos += pos_adv;
    j = (int) pos - (int) (2 * pos_adv);
    coef3 = coef1, coef3_adv = (int) ((val - coef3) / ((j) - 1));
    while (j--) {
        *buffer++ = RGB ((BYTE) (coef3 >> int_extend),(BYTE) (val >> int_extend),(BYTE) (coef1 >> int_extend));
        coef3 += coef3_adv;
    }

    pos += pos_adv;
    j = (int) pos - (int) (3 * pos_adv);
    coef2 = val, coef2_adv = (int) ((val * (1.0 - sat) - coef2) / ((j) - 1));
    while (j--) {
        *buffer++ = RGB((BYTE) (val >> int_extend),(BYTE) (coef2 >> int_extend),(BYTE) (coef1 >> int_extend));
        coef2 += coef2_adv;
    }

    pos += pos_adv;
    j = (int) pos - (int) (4 * pos_adv);
    coef3 = coef1, coef3_adv = (int) ((val - coef3) / ((j) - 1));
    while (j--) {
        *buffer++ = RGB ((BYTE) (val >> int_extend),(BYTE) (coef1 >> int_extend),(BYTE) (coef3 >> int_extend));
        coef3 += coef3_adv;
    }

    pos += (pos_adv + 0.1); // + 0.1 because of floating-point math's rounding errors
    j = (int) pos - (int) (5 * pos_adv);
    coef2 = val, coef2_adv = (int) ((val * (1.0 - sat) - coef2) / HSV_LOOP_STEPS (j));
    while (j--) {
        *buffer++ = RGB ( (BYTE) (coef2 >> int_extend),(BYTE) (coef1 >> int_extend),(BYTE) (val >> int_extend));
        coef2 += coef2_adv;
    }
}
