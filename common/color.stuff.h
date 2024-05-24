#pragma once

#include "wcdafx.api.h"
#include <windef.h>

constexpr double  HSV_HUE_MAX{360.};
constexpr int HSV_HUE_MAX_INT{360};
constexpr double  HSV_100PERC{100.};
constexpr int HSV_100PERC_INT{100};
constexpr double      RGB_MAX{255.};
constexpr int     RGB_MAX_INT{255};

constexpr double  HSV_HUE_MID{120.};
constexpr double HSV_HUE_AMID{270.};

namespace CF
{
    WCDAFX_API COLORREF ReduceColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B);
    WCDAFX_API COLORREF IncreaseColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B);
    WCDAFX_API COLORREF InvertColor(COLORREF color);
    WCDAFX_API COLORREF CalcContrastColor(COLORREF color, COLORREF tolerance = 0x10);
    WCDAFX_API COLORREF ComplementColor(COLORREF color);

    WCDAFX_API void HSVtoRGB(double const dH, double const dS, double const dV, int& R, int& G, int& B);
    WCDAFX_API void RGBtoHSV(int const R, int const G, int const B, double& dH, double& dS, double& dV);
    WCDAFX_API void RGBtoHSL1(int const R, int const G, int const B, double& dHl, double& dSl, double& dLl);
    WCDAFX_API void RGBtoHSL2(int const R, int const G, int const B, double& dHl, double& dSl, double& dLl);
    WCDAFX_API void HSVtoHSL(double const dH, double const dS, double const dV, double& dHl, double& dSl, double& dLl);
    WCDAFX_API void HSLtoRGB(double const dH, double const dS, double const dL, int& R, int& G, int& B);
}
