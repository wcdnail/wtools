#include "stdafx.h"
#include "DDGDIStuff.h"
#include "CeXDib.h"
#include <cmath>

//
//#include "exa/wtl.color.picker/GTDrawHelper.h"
//
// This code is refactored from: GTDrawHelper.cpp
// David Swigger's (dwswigger@yahoo.com) Photoshop's-like color pickers
// 15 Jul 2002
// https://codeproject.com/Articles/2577/Xguiplus-A-set-of-Photoshop-s-like-color-pickers
//

constexpr int RGB_INT_SCALE{20};

constexpr int   ScaleRed(COLORREF c) { return (GetRValue(c) << RGB_INT_SCALE); }
constexpr int ScaleGreen(COLORREF c) { return (GetGValue(c) << RGB_INT_SCALE); }
constexpr int  ScaleBlue(COLORREF c) { return (GetBValue(c) << RGB_INT_SCALE); }

constexpr void RGB_Line3(DWORD*& pDest, int nCount, int vR, int vG, int vB, int iR, int iG, int iB)
{
    for (int j = 0; j < nCount; j++) {
        *pDest++ = RGB(vR >> RGB_INT_SCALE, vG >> RGB_INT_SCALE, vB >> RGB_INT_SCALE);
        vR += iR;
        vG += iG;
        vB += iB;
    }
}

constexpr void RGB_Line1(DWORD*& pDest, int nCount, int bRGB, int iR, int iG, int iB)
{
    const int vR{bRGB};
    const int vG{bRGB};
    const int vB{bRGB};
    RGB_Line3(pDest, nCount, vR, vG, vB, iR, iG, iB);
}

bool DDraw_RGBGrad_Line(DWORD* pDest, int nWidth, COLORREF crBegin, COLORREF crEnd)
{
    if (nWidth <= 0) {
        // TODO: report nWidth - INVAL
        return false;
    }
    int const    R{ScaleRed(crBegin)};
    int const    G{ScaleGreen(crBegin)};
    int const    B{ScaleBlue(crBegin)};
    int const rInc{(ScaleRed(crEnd) - R) / (nWidth - 1)};
    int const gInc{(ScaleGreen(crEnd) - G) / (nWidth - 1)};
    int const bInc{(ScaleBlue(crEnd) - B) / (nWidth - 1)};
    // set current pixel (in DIB bitmap format is BGR, not RGB!)
    RGB_Line3(pDest, nWidth, B, G, R, bInc, gInc, rInc);
    return true;
}

bool DDraw_RGB(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, CRGBSpecRect&& clrRect)
{
    int            lbRed{ScaleRed(clrRect.crLB)};
    int          lbGreen{ScaleGreen(clrRect.crLB)};
    int           lbBlue{ScaleBlue(clrRect.crLB)};
    int const   ltRedInc{(ScaleRed(clrRect.crLT) - lbRed) / nHeight};
    int const ltGreenInc{(ScaleGreen(clrRect.crLT) - lbGreen) / nHeight};
    int const  ltBlueInc{(ScaleBlue(clrRect.crLT) - lbBlue) / nHeight};

    int            rbRed{ScaleRed(clrRect.crRB)};
    int          rbGreen{ScaleGreen(clrRect.crRB)};
    int           rbBlue{ScaleBlue(clrRect.crRB)};
    int const   rtRedInc{(ScaleRed(clrRect.crRT) - rbRed) / nHeight};
    int const rtGreenInc{(ScaleGreen(clrRect.crRT) - rbGreen) / nHeight};
    int const  rtBlueInc{(ScaleBlue(clrRect.crRT) - rbBlue) / nHeight};

    int const      nSkip{static_cast<int>(nLinePitch) - nWidth};

    for (int i = 0; i < nHeight; i++) {
        int                R{lbRed};
        int                G{lbGreen};
        int                B{lbBlue};
        int const    nRedInc{(rbRed - R) / nWidth};
        int const  nGreenInc{(rbGreen - G) / nWidth};
        int const   nBlueInc{(rbBlue - B) / nWidth};
        for (int j = 0; j < nWidth; j++) {
            *pDest++ = RGB(B >> RGB_INT_SCALE, G >> RGB_INT_SCALE, R >> RGB_INT_SCALE);
            R += nRedInc;
            G += nGreenInc;
            B += nBlueInc;
        }
        lbRed += ltRedInc;
        lbGreen += ltGreenInc;
        lbBlue += ltBlueInc;
        rbRed += rtRedInc;
        rbGreen += rtGreenInc;
        rbBlue += rtBlueInc;
        pDest += nSkip;
    }
    return true;
}

bool DDraw_RGB(CDibEx const& dibDest, CRGBSpecRect&& color)
{
    ATLASSUME(dibDest.GetBitCount() == 32);
    return DDraw_RGB(reinterpret_cast<DWORD*>(dibDest.GetData()),
                     dibDest.GetWidth(),
                     dibDest.GetHeight(),
                     dibDest.GetStride() / sizeof(DWORD),
                     std::move(color));
}

bool DDraw_RGB_Grad(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, COLORREF crBegin, COLORREF crEnd)
{
    if (nHeight <= 0) {
        // TODO: report nHeight - INVAL
        return false;
    }
    DDraw_RGBGrad_Line(pDest, nWidth, crBegin, crEnd);
    for (int i = 1; i<nHeight; i++) {
        memcpy(pDest + nLinePitch, pDest, nWidth * sizeof(DWORD));
        pDest += nLinePitch;
    }
    return true;
}

bool DDraw_RGB_Grad(CDibEx const& dibDest, COLORREF crBegin, COLORREF crEnd)
{
    ATLASSUME(dibDest.GetBitCount() == 32);
    return DDraw_RGB_Grad(reinterpret_cast<DWORD*>(dibDest.GetData()),
                          dibDest.GetWidth(),
                          dibDest.GetHeight(),
                          dibDest.GetStride() / sizeof(DWORD),
                          crBegin, crEnd);
}

constexpr void HSV_HUE_Red_To_Yellow(DWORD*& pDest, int nRange, int nVal, int nCoef, double dSat)
{
    UNREFERENCED_PARAMETER(dSat);
    int const nStep{(nVal - nCoef) / (nRange - 1)};
    RGB_Line3(pDest, nRange, nCoef, nCoef, nVal, 0, nStep, 0);
}

constexpr void HSV_HUE_Yellow_To_Green(DWORD*& pDest, int nRange, int nVal, int nCoef, double dSat)
{
    int const nStep{static_cast<int>((nVal * (1.0 - dSat) - nVal) / (nRange - 1))};
    RGB_Line3(pDest, nRange, nCoef, nVal, nVal, 0, 0, nStep);
}

constexpr void HSV_HUE_Green_To_Cyan(DWORD*& pDest, int nRange, int nVal, int nCoef, double dSat)
{
    UNREFERENCED_PARAMETER(dSat);
    int const nStep{(nVal - nCoef) / (nRange - 1)};
    RGB_Line3(pDest, nRange, nCoef, nVal, nCoef, nStep, 0, 0);
}

constexpr void HSV_HUE_Cyan_To_Blue(DWORD*& pDest, int nRange, int nVal, int nCoef, double dSat)
{
    int const nStep{static_cast<int>((nVal * (1.0 - dSat) - nVal) / (nRange - 1))};
    RGB_Line3(pDest, nRange, nVal, nVal, nCoef, 0, nStep, 0);
}

constexpr void HSV_HUE_Blue_To_Violet(DWORD*& pDest, int nRange, int nVal, int nCoef, double dSat)
{
    UNREFERENCED_PARAMETER(dSat);
    int const nStep{(nVal - nCoef) / (nRange - 1)};
    RGB_Line3(pDest, nRange, nVal, nCoef, nCoef, 0, 0, nStep);
}

constexpr void HSV_HUE_Violet_To_Crimson(DWORD*& pDest, int nRange, int nVal, int nCoef, double dSat)
{
    int const nStep{static_cast<int>((nVal * (1.0 - dSat) - nVal) / (nRange - 1))};
    RGB_Line3(pDest, nRange, nVal, nCoef, nVal, nStep, 0, 0);
}

bool DDraw_HSV_HUE_Line(DWORD* pDest, int nWidth, double dSat, double dVal)
{
    //GTDrawHelper dh;
    //dh.HSV_HUE(pDest, nWidth, dSat, dVal);
    //return true;
    if (nWidth <= 0) {
        // TODO: report nWidth - INVAL
        return false;
    }
    using PLineRoutine = void(*)(DWORD*&, int, int, int, double);
    static constexpr PLineRoutine gsLineRoutine[]{
        HSV_HUE_Red_To_Yellow,
        HSV_HUE_Yellow_To_Green,
        HSV_HUE_Green_To_Cyan,
        HSV_HUE_Cyan_To_Blue,
        HSV_HUE_Blue_To_Violet,
        HSV_HUE_Violet_To_Crimson,
    };
    static constexpr int giIterationCount{_countof(gsLineRoutine)};

    int const     nVal{static_cast<int>(dVal * 255.) << RGB_INT_SCALE};
    int const    nCoef{static_cast<int>(nVal * (1 - dSat))};
    double const dOffs{static_cast<double>(nWidth) / static_cast<double>(giIterationCount)};
    double        dPos{0};
    for (int i = 0; i < giIterationCount; i++) {
        dPos += dOffs;
        auto nRange{static_cast<int>(dPos) - static_cast<int>(i * dOffs)};
        nWidth -= nRange;
        if (i >= giIterationCount - 1) {
            nRange += nWidth;
        }
        gsLineRoutine[i](pDest, nRange, nVal, nCoef, dSat);
    }
    return true;
}

bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue)
{
    //GTDrawHelper dh;
    //dh.HSV_SAT(pDest, nWidth, dHue, dValue);
    //return true;
    if (nWidth <= 0) {
        // TODO: report nWidth - INVAL
        return false;
    }
    const int   vStart{static_cast<int>(dValue * 255) << RGB_INT_SCALE};
    double        dInt{0};
    const double dFrac{std::modf(dHue / 60.0, &dInt)};
    auto const    nInt{static_cast<int>(dInt)};
    const int     inc1{-vStart / nWidth};
    const int     inc2{static_cast<int>((1 - dFrac) * vStart - vStart) / nWidth};
    const int     inc3{static_cast<int>(dFrac * vStart - vStart) / nWidth};
    switch (nInt) {
    default:
    case 0: RGB_Line1(pDest, nWidth, vStart, inc1, inc3, 0); break;
    case 1: RGB_Line1(pDest, nWidth, vStart, inc1, 0, inc2); break;
    case 2: RGB_Line1(pDest, nWidth, vStart, inc3, 0, inc1); break;
    case 3: RGB_Line1(pDest, nWidth, vStart, 0, inc2, inc1); break;
    case 4: RGB_Line1(pDest, nWidth, vStart, 0, inc1, inc3); break;
    case 5: RGB_Line1(pDest, nWidth, vStart, inc2, inc1, 0); break;
    }
    return true;
}

bool DDraw_HSV_VAL_Line(DWORD* pDest, int nWidth, double dHue, double dSat)
{
    //GTDrawHelper dh;
    //dh.HSV_VAL(pDest, nWidth, dHue, dSat);
    //return true;
    if (nWidth <= 0) {
        // TODO: report nWidth - INVAL
        return false;
    }
    const int   vStart{0};
    const int     vMax{255 << RGB_INT_SCALE};
    const int    vStep{vMax / nWidth};
    double        dInt{0};
    const double dFrac{std::modf(dHue / 60.0, &dInt)};
    auto const    nInt{static_cast<int>(dInt)};
    const int     inc1{static_cast<int>((vMax * (1. - dSat)) / nWidth)};
    const int     inc2{static_cast<int>((vMax * (1. - dSat * dFrac)) / nWidth)};
    const int     inc3{static_cast<int>((vMax * (1. - dSat * (1 - dFrac))) / nWidth)};
    switch (nInt) {
    default:
    case 0: RGB_Line1(pDest, nWidth, vStart, inc1, inc3, vStep); break;
    case 1: RGB_Line1(pDest, nWidth, vStart, inc1, vStep, inc2); break;
    case 2: RGB_Line1(pDest, nWidth, vStart, inc3, vStep, inc1); break;
    case 3: RGB_Line1(pDest, nWidth, vStart, vStep, inc2, inc1); break;
    case 4: RGB_Line1(pDest, nWidth, vStart, vStep, inc1, inc3); break;
    case 5: RGB_Line1(pDest, nWidth, vStart, inc2, inc1, vStep); break;
    }
    return true;
}

bool DDraw_Spectrum(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dParam, PLineDrawer pLineDraw)
{
    if ((nHeight <= 0) || !pLineDraw) {
        // TODO: report INVARG
        return false;
    }
    double     dValue{0.0};
    const double dAdv{1.0 / nHeight};
    for (int i = 0; i<nHeight; i++) {
        if (!pLineDraw(pDest, nWidth, dParam, dValue)) {
            return false;
        }
        dValue += dAdv;
        pDest += nLinePitch;
    }
    return true;
}

bool DDraw_Spectrum(CDibEx const& dibDest, double dParam, PSpecDrawer pSpecDrawer)
{
    ATLASSUME(pSpecDrawer != nullptr);
    ATLASSUME(dibDest.GetBitCount() == 32);
    return pSpecDrawer(reinterpret_cast<DWORD*>(dibDest.GetData()),
        dibDest.GetWidth(),
        dibDest.GetHeight(),
        dibDest.GetStride() / sizeof(DWORD),
        dParam);
}

bool DDraw_Gradient(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dA, double dB, PLineDrawer pLineDrawer)
{
    if ((nHeight <= 0) || !pLineDrawer) {
        // TODO: report INVARG
        return false;
    }
    pLineDrawer(pDest, nWidth, dA, dB);
    for (int i = 1; i<nHeight; i++) {
        memcpy(pDest + nLinePitch, pDest, nWidth * sizeof(DWORD));
        pDest += nLinePitch;
    }
    return true;
}

bool DDraw_Gradient(CDibEx const& dibDest, double dA, double dB, PGradDrawer pGradDrawer)
{
    ATLASSUME(pGradDrawer != nullptr);
    ATLASSUME(dibDest.GetBitCount() == 32);
    return pGradDrawer(reinterpret_cast<DWORD*>(dibDest.GetData()),
                       dibDest.GetWidth(),
                       dibDest.GetHeight(),
                       dibDest.GetStride() / sizeof(DWORD),
                       dA, dB);
}
