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
    SPECTRUM_BPP        = 32,
    SPEC_BITMAP_WDTH    = 64,
    SPECTRUM_CX         = SPEC_BITMAP_WDTH,
    SPECTRUM_CY         = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_CX  = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_MIN = 0,
    SPECTRUM_SLIDER_MAX = 255,
    SPECTRUM_CHANNEL_CX = 24,
    SPECTRUM_CHANNEL_CY = 32,
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

union CUNRGB
{
    COLORREF Color;
    CSTRGB     RGB;
};

struct CColorUnion
{
    double       m_dH;
    double       m_dS;
    double       m_dV;
    CUNRGB     m_Comp;
    bool   m_bUpdated;

    ~CColorUnion();
    CColorUnion(double dH, double dS, double dV);
    CColorUnion(COLORREF crInitial);

    bool IsUpdated() const;
    void SetUpdated(bool bUpdated);

    void SetRGB(int R, int G, int B);
    void UpdateRGB();
    void UpdateHSV();

    void SetHue(double dHue);
};

inline bool CColorUnion::IsUpdated() const
{
    return m_bUpdated;
}

inline void CColorUnion::SetUpdated(bool bUpdated)
{
    m_bUpdated = bUpdated;
}

inline void CColorUnion::SetRGB(int R, int G, int B)
{
    m_Comp.Color = RGB(R, G, B);
    SetUpdated(true);
}

static_assert(SPECTRUM_BPP == 32, "SPECTRUM_BPP must always be 32!");
