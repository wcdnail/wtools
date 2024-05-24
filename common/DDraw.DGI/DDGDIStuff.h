#pragma once

#include <minwindef.h>

struct CDibEx;

struct CRGBSpecRect
{
    COLORREF crLT;
    COLORREF crRT;
    COLORREF crLB;
    COLORREF crRB;
};

using PLineDrawer = bool(*)(DWORD*, int, double, double);
using PSpecDrawer = bool(*)(DWORD*, int, int, DWORD, double);
using PGradDrawer = bool(*)(DWORD*, int, int, DWORD, double, double);

bool DDraw_RGBGrad_Line(DWORD* pDest, int nWidth, COLORREF crBegin, COLORREF crEnd);
bool DDraw_RGB(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, CRGBSpecRect&& color);
bool DDraw_RGB(CDibEx const& dibDest, CRGBSpecRect&& color);
bool DDraw_RGB_Grad(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, COLORREF crBegin, COLORREF crEnd);
bool DDraw_RGB_Grad(CDibEx const& dibDest, COLORREF crBegin, COLORREF crEnd);

bool DDraw_HSV_HUE_Line(DWORD* pDest, int nWidth, double dSat, double dVal);
bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue);
bool DDraw_HSV_VAL_Line(DWORD* pDest, int nWidth, double dHue, double dSat);

bool DDraw_Spectrum(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dParam, PLineDrawer pLineDraw);
bool DDraw_Spectrum(CDibEx const& dibDest, double dParam, PSpecDrawer pSpecDrawer);
bool DDraw_Gradient(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dA, double dB, PLineDrawer pLineDrawer);
bool DDraw_Gradient(CDibEx const& dibDest, double dA, double dB, PGradDrawer pGradDrawer);

inline bool DDraw_HSV_H(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSat, double dVal)
{
    return DDraw_Gradient(pDest, nWidth, nHeight, nLinePitch, dSat, dVal, DDraw_HSV_HUE_Line);
}

inline bool DDraw_HSV_H(CDibEx const& dibDest, double dSat, double dVal)
{
    return DDraw_Gradient(dibDest, dSat, dVal, DDraw_HSV_H);
}

inline bool DDraw_HSV_S(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue, double dVal)
{
    return DDraw_Gradient(pDest, nWidth, nHeight, nLinePitch, dHue, dVal, DDraw_HSV_SAT_Line);
}

inline bool DDraw_HSV_S(CDibEx const& dibDest, double dHue, double dVal)
{
    return DDraw_Gradient(dibDest, dHue, dVal, DDraw_HSV_S);
}

inline bool DDraw_HSV_V(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue, double dSat)
{
    return DDraw_Gradient(pDest, nWidth, nHeight, nLinePitch, dHue, dSat, DDraw_HSV_VAL_Line);
}

inline bool DDraw_HSV_V(CDibEx const& dibDest, double dHue, double dSat)
{
    return DDraw_Gradient(dibDest, dHue, dSat, DDraw_HSV_V);
}

inline bool DDraw_HSV_Hue(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue)
{
    return DDraw_Spectrum(pDest, nWidth, nHeight, nLinePitch, dHue, DDraw_HSV_SAT_Line);
}

inline bool DDraw_HSV_Hue(CDibEx const& dibDest, double dHue)
{
    return DDraw_Spectrum(dibDest, dHue, DDraw_HSV_Hue);
}

inline bool DDraw_HSV_Sat(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSat)
{
    return DDraw_Spectrum(pDest, nWidth, nHeight, nLinePitch, dSat, DDraw_HSV_HUE_Line);
}

inline bool DDraw_HSV_Sat(CDibEx const& dibDest, double dSat)
{
    return DDraw_Spectrum(dibDest, dSat, DDraw_HSV_Sat);
}

inline bool DDraw_HSV_HUE_Line_2(DWORD* pDest, int nWidth, double dSat, double dVal)
{
    return DDraw_HSV_HUE_Line(pDest, nWidth, dVal, dSat);
}

inline bool DDraw_HSV_Val(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dVal)
{
    return DDraw_Spectrum(pDest, nWidth, nHeight, nLinePitch, dVal, DDraw_HSV_HUE_Line_2);
}

inline bool DDraw_HSV_Val(CDibEx const& dibDest, double dVal)
{
    return DDraw_Spectrum(dibDest, dVal, DDraw_HSV_Val);
}

void DDraw_Checkers(PDWORD pDest, int nWidth, int nHeight, DWORD nLinePitch, COLORREF crA, COLORREF crB);
void DDraw_Checkers(CDibEx const& dibDest, COLORREF crA, COLORREF crB);
