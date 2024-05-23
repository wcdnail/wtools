#pragma once

#include <wcdafx.api.h>
#include <utility>

constexpr TCHAR CSPECIMG_CLASS[] = _T("WCCF::CSpectrumImage");
constexpr TCHAR CSPECSLD_CLASS[] = _T("WCCF::CSpectrumSlider");

constexpr double  HSV_HUE_MAX{360.};
constexpr int HSV_HUE_MAX_INT{360};
constexpr double  HSV_HUE_MID{120.};
constexpr double HSV_HUE_AMID{270.};
constexpr double  HSV_SAT_MAX{100.};
constexpr int HSV_SAT_MAX_INT{100};
constexpr double  HSV_VAL_MAX{100.};
constexpr int HSV_VAL_MAX_INT{100};
constexpr double      RGB_MAX{255.};
constexpr int     RGB_MAX_INT{255};

void HSVtoRGB(double const dH, double const dS, double const dV, int& R, int& G, int& B);
void RGBtoHSV(int const R, int const G, int const B, double& dH, double& dS, double& dV);
void RGBtoHSL(int const R, int const G, int const B, double& dHl, double& dSl, double& dLl);

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
    SPEC_BITMAP_WDTH    = 64,
    SPECTRUM_BPP        = 32,
    SPECTRUM_CX         = SPEC_BITMAP_WDTH,
    SPECTRUM_CY         = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_CX  = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_MIN = 0,
    SPECTRUM_SLIDER_MAX = RGB_MAX_INT,
    SPECTRUM_CHANNEL_CX = 24,
    SPECTRUM_CHANNEL_CY = 32,

    NM_SPECTRUM_CLR_SEL = 1917,
    NM_SLIDER_CLR_SEL,
};

static_assert(SPECTRUM_BPP == 32, "SPECTRUM_BPP must always be 32!");

struct CSpectrumImage;
struct CSpectrumSlider;
struct CRGBSpecRect;

struct CColorUnion
{
    DECL_COPY_MOVE_OF(CColorUnion);

    bool m_bUpdated;
    int         m_H;
    int         m_S;
    int         m_V;
    int         m_R;
    int         m_G;
    int         m_B;
    int         m_A;
    int        m_Hl;
    int        m_Sl;
    int         m_L;

    ~CColorUnion();
    CColorUnion(double dH, double dS, double dV);
    CColorUnion(COLORREF crInitial);

    bool IsUpdated() const;
    void SetUpdated(bool bUpdated);

    void SetRGB(int R, int G, int B);
    void SetHSV(double dH, double dS, double dV);

    void HSVtoRGB();
    void RGBtoHSV();
    void RGBtoHSL();
    void FromHSL();

    int GetRed() const;
    int GetGreen() const;
    int GetBlue() const;
    int GetAlpha() const;

    int& GetRed();
    int& GetGreen();
    int& GetBlue();
    int& GetAlpha();

    COLORREF GetRedFirst() const;
    COLORREF GetRedLast() const;
    COLORREF GetGreenFirst() const;
    COLORREF GetGreenLast() const;
    COLORREF GetBlueFirst() const;
    COLORREF GetBlueLast() const;

    COLORREF GetColorRef() const;
    CRGBSpecRect GetRGBSpectrumRect(SpectrumKind nSpectrumKind) const;
    CPoint GetColorPoint(SpectrumKind nSpectrumKind, CRect const& rc);

private:
    void SetRGBPlain(int R, int G, int B);
};

inline bool CColorUnion::IsUpdated() const { return m_bUpdated; }
inline void CColorUnion::SetUpdated(bool bUpdated) { m_bUpdated = bUpdated; }

inline void CColorUnion::SetRGBPlain(int R, int G, int B)
{
    GetRed()   = std::min<int>(R, RGB_MAX_INT);
    GetGreen() = std::min<int>(G, RGB_MAX_INT);
    GetBlue()  = std::min<int>(B, RGB_MAX_INT);
    SetUpdated(true);
}

inline void CColorUnion::SetRGB(int R, int G, int B)
{
    SetRGBPlain(R, G, B);
    RGBtoHSV();
}

inline void CColorUnion::SetHSV(double dH, double dS, double dV)
{
    m_H = static_cast<int>(std::min<double>(dH, HSV_HUE_MAX));
    m_S = static_cast<int>(std::min<double>(dS, HSV_SAT_MAX));
    m_V = static_cast<int>(std::min<double>(dV, HSV_VAL_MAX));
    HSVtoRGB();
}

inline void CColorUnion::HSVtoRGB()
{
    ::HSVtoRGB(m_H, m_S / HSV_SAT_MAX, m_V / HSV_VAL_MAX, m_R, m_G, m_B);
    RGBtoHSL();
    SetUpdated(true);
}

inline void CColorUnion::RGBtoHSV()
{
    double dH, dS, dV;
    ::RGBtoHSV(m_R, m_G, m_B, dH, dS, dV);
    m_H = static_cast<int>(dH);
    m_S = static_cast<int>(dS * HSV_SAT_MAX);
    m_V = static_cast<int>(dV * HSV_VAL_MAX);
    RGBtoHSL();
    SetUpdated(true);
}

inline void CColorUnion::RGBtoHSL()
{
    double dH, dS, dL;
    ::RGBtoHSL(m_R, m_G, m_B, dH, dS, dL);
    m_Hl = static_cast<int>(dH * HSV_HUE_MAX);
    m_Sl = static_cast<int>(dS * HSV_SAT_MAX);
    m_L = static_cast<int>(dL * HSV_VAL_MAX);
}

inline int     CColorUnion::GetRed() const { return m_R; }
inline int   CColorUnion::GetGreen() const { return m_G; }
inline int    CColorUnion::GetBlue() const { return m_B; }
inline int   CColorUnion::GetAlpha() const { return m_A; }
inline int&    CColorUnion::GetRed() { return m_R; }
inline int&  CColorUnion::GetGreen() { return m_G; }
inline int&   CColorUnion::GetBlue() { return m_B; }
inline int&  CColorUnion::GetAlpha() { return m_A; }

inline COLORREF   CColorUnion::GetRedFirst() const { return RGB(       0, GetGreen(), GetBlue()); }
inline COLORREF    CColorUnion::GetRedLast() const { return RGB(     255, GetGreen(), GetBlue()); }
inline COLORREF CColorUnion::GetGreenFirst() const { return RGB(GetRed(),          0, GetBlue()); }
inline COLORREF  CColorUnion::GetGreenLast() const { return RGB(GetRed(),        255, GetBlue()); }
inline COLORREF  CColorUnion::GetBlueFirst() const { return RGB(GetRed(), GetGreen(),         0); }
inline COLORREF   CColorUnion::GetBlueLast() const { return RGB(GetRed(), GetGreen(),       255); }

inline COLORREF   CColorUnion::GetColorRef() const { return RGB(GetRed(), GetGreen(), GetBlue()); }
