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

bool DDrawHSVHueLine(DWORD* pDest, int nWidth, double dHue, double dValue);
bool DDrawHSVHueSpectrum(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue);
bool DDrawHSVHueSpectrum(CDibEx const& dibDest, double dHue);

void DDrawHSVSatLine(DWORD* pDest, int nWidth, double dSaturation, double dVal);
bool DDrawHSVSatGradient(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSaturation, double dVal);
bool DDrawHSVSatGradient(CDibEx const& dibDest, double dSaturation, double dVal);

void DDrawHSVSatSpectrum(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSaturation);
void DDrawHSVSatSpectrum(CDibEx const& dibDest, double dSaturation);

void DDrawRGBSpectrum(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, CRGBSpecRect&& color);
void DDrawRGBSpectrum(CDibEx const& dibDest, CRGBSpecRect&& color);
