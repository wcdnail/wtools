#pragma once

constexpr TCHAR CSPECIMG_CLASS[] = _T("WCCF::CSpectrumImage");
constexpr TCHAR CSPECSLD_CLASS[] = _T("WCCF::CSpectrumSlider");

enum SpectrumKind: int
{
    SPEC_Begin = 0,
    SPEC_RGB_Red = SPEC_Begin,
    SPEC_RGB_Green,
    SPEC_RGB_Blue,
    SPEC_HSV_Hue,
    SPEC_HSV_Saturation,
    SPEC_HSV_Brightness,
    SPEC_End = SPEC_HSV_Brightness
};

enum CCPMiscConsts: int
{
    SPEC_BITMAP_WDTH   = 64,
    SPECTRUM_CX        = SPEC_BITMAP_WDTH,
    SPECTRUM_CY        = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_CX = SPEC_BITMAP_WDTH,
};

struct CSpectrumImage;
struct CSpectrumSlider;

struct CSTRGB
{
    BYTE   m_btRed;
    BYTE m_btGreen;
    BYTE  m_btBlue;
    BYTE m_btAlpha;
};

struct CColorUnion
{
    union
    {
        COLORREF Color;
        CSTRGB     RGB;
    }
    m_Comp;
};
