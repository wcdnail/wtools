#pragma once

#include <minwindef.h>

struct CDibEx;

bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue);
bool DDraw_HSV_Hue(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dHue);
bool DDraw_HSV_Hue(CDibEx const& dibDest, double dHue);

void DDraw_HSV_HUE_Line(DWORD* pDest, int nWidth, double dSaturation, double dVal);
bool DDraw_HSV_HUE(DWORD* pDest, int nWidth, int nHeight, DWORD nLinePitch, double dSaturation, double dVal);
bool DDraw_HSV_HUE(CDibEx const& dibDest, double dSaturation, double dVal);
