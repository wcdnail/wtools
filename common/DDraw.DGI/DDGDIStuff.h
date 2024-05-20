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

void DDraw_RGBGrad_Line(DWORD* pDest, int nWidth, COLORREF crBegin, COLORREF crEnd);
void DDraw_RGB(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, CRGBSpecRect&& color);
void DDraw_RGB(CDibEx const& dibDest, CRGBSpecRect&& color);
void DDraw_RGB_Grad(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, COLORREF crBegin, COLORREF crEnd);
void DDraw_RGB_Grad(CDibEx const& dibDest, COLORREF crBegin, COLORREF crEnd);

void DDraw_HSV_HUE_Line(DWORD* pDest, int nWidth, double dSaturation, double dVal);
bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue);
bool DDraw_HSV_VAL_Line(DWORD* pDest, int nWidth, double dHue, double dSaturation);

bool DDraw_HSV_Hue(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue);
bool DDraw_HSV_Hue(CDibEx const& dibDest, double dHue);

void DDraw_HSV_Sat(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSaturation);
void DDraw_HSV_Sat(CDibEx const& dibDest, double dSaturation);

bool DDraw_HSV_H(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSaturation, double dVal);
bool DDraw_HSV_H(CDibEx const& dibDest, double dSaturation, double dVal);

bool DDraw_HSV_S(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue, double dVal);
bool DDraw_HSV_S(CDibEx const& dibDest, double dHue, double dVal);

bool DDraw_HSV_V(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue, double dSaturation);
bool DDraw_HSV_V(CDibEx const& dibDest, double dHue, double dSaturation);
