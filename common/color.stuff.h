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
    struct ColorTabItem
    {
        COLORREF clrColor;
        LPCTSTR   pszName;
    };

    constexpr ColorTabItem CLR_PALETTE0[] =
    {
        { RGB(0x00, 0x00, 0x00), _T("Black") },
        { RGB(0xA5, 0x2A, 0x00), _T("Brown") },
        { RGB(0x00, 0x40, 0x40), _T("Dark Olive Green") },
        { RGB(0x00, 0x55, 0x00), _T("Dark Green") },
        { RGB(0x00, 0x00, 0x5E), _T("Dark Teal") },
        { RGB(0x00, 0x00, 0x8B), _T("Dark blue") },
        { RGB(0x4B, 0x00, 0x82), _T("Indigo") },
        { RGB(0x28, 0x28, 0x28), _T("Dark grey") },

        { RGB(0x8B, 0x00, 0x00), _T("Dark red") },
        { RGB(0xFF, 0x68, 0x20), _T("Orange") },
        { RGB(0x8B, 0x8B, 0x00), _T("Dark yellow") },
        { RGB(0x00, 0x93, 0x00), _T("Green") },
        { RGB(0x38, 0x8E, 0x8E), _T("Teal") },
        { RGB(0x00, 0x00, 0xFF), _T("Blue") },
        { RGB(0x7B, 0x7B, 0xC0), _T("Blue-grey") },
        { RGB(0x66, 0x66, 0x66), _T("Grey - 40") },

        { RGB(0xFF, 0x00, 0x00), _T("Red") },
        { RGB(0xFF, 0xAD, 0x5B), _T("Light orange") },
        { RGB(0x32, 0xCD, 0x32), _T("Lime") },
        { RGB(0x3C, 0xB3, 0x71), _T("Sea green") },
        { RGB(0x7F, 0xFF, 0xD4), _T("Aqua") },
        { RGB(0x7D, 0x9E, 0xC0), _T("Light blue") },
        { RGB(0x80, 0x00, 0x80), _T("Violet") },
        { RGB(0x7F, 0x7F, 0x7F), _T("Grey - 50") },

        { RGB(0xFF, 0xC0, 0xCB), _T("Pink") },
        { RGB(0xFF, 0xD7, 0x00), _T("Gold") },
        { RGB(0xFF, 0xFF, 0x00), _T("Yellow") },
        { RGB(0x00, 0xFF, 0x00), _T("Bright green") },
        { RGB(0x40, 0xE0, 0xD0), _T("Turquoise") },
        { RGB(0xC0, 0xFF, 0xFF), _T("Skyblue") },
        { RGB(0x48, 0x00, 0x48), _T("Plum") },
        { RGB(0xC0, 0xC0, 0xC0), _T("Light grey") },

        { RGB(0xFF, 0xE4, 0xE1), _T("Rose") },
        { RGB(0xD2, 0xB4, 0x8C), _T("Tan") },
        { RGB(0xFF, 0xFF, 0xE0), _T("Light yellow") },
        { RGB(0x98, 0xFB, 0x98), _T("Pale green ") },
        { RGB(0xAF, 0xEE, 0xEE), _T("Pale turquoise") },
        { RGB(0x68, 0x83, 0x8B), _T("Pale blue") },
        { RGB(0xE6, 0xE6, 0xFA), _T("Lavender") },
        { RGB(0xFF, 0xFF, 0xFF), _T("White" )}
    };
    constexpr size_t CLR_PALETTE0_COUNT{std::size(CLR_PALETTE0)};

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
