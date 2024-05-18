#pragma once

#include <minwindef.h>

struct CDibEx;

bool DDraw_HSV_SAT_Line(DWORD* pDest, int nWidth, double dHue, double dValue);
bool DDraw_HSV_Hue(DWORD* pDest, int nWidth, int nHeight, int nLinePitch, double dHue);
bool DDraw_HSV_Hue(CDibEx const& dibDest, double dHue);

