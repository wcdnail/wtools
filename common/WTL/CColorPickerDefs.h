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
    SPECTRUM_CLR_RANGE  = 255,
    SPEC_BITMAP_WDTH    = 64,
    SPECTRUM_CX         = SPEC_BITMAP_WDTH,
    SPECTRUM_CY         = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_CX  = SPEC_BITMAP_WDTH,
    SPECTRUM_SLIDER_MIN = 0,
    SPECTRUM_SLIDER_MAX = SPECTRUM_CLR_RANGE,
    SPECTRUM_CHANNEL_CX = 24,
    SPECTRUM_CHANNEL_CY = 32,

    NM_SPECTRUM_CLR_SEL = 1917,
};

static_assert(SPECTRUM_BPP == 32, "SPECTRUM_BPP must always be 32!");

struct CSpectrumImage;
struct CSpectrumSlider;

struct CColorUnion
{
    bool m_bUpdated;
    double     m_dH;
    double     m_dS;
    double     m_dV;
    BYTE      m_Red;
    BYTE    m_Green;
    BYTE     m_Blue;
    BYTE    m_Alpha;

    ~CColorUnion();
    CColorUnion(double dH, double dS, double dV);
    CColorUnion(COLORREF crInitial);

    bool IsUpdated() const;
    void SetUpdated(bool bUpdated);

    void SetRGB(int R, int G, int B);
    void SetHSV(double dH, double dS, double dV);
    void UpdateRGB();
    void UpdateHSV();

    BYTE GetRed() const;
    BYTE GetGreen() const;
    BYTE GetBlue() const;
    BYTE GetAlpha() const;

    BYTE& GetRed();
    BYTE& GetGreen();
    BYTE& GetBlue();
    BYTE& GetAlpha();

    COLORREF GetRedFirst() const;
    COLORREF GetRedLast() const;
    COLORREF GetGreenFirst() const;
    COLORREF GetGreenLast() const;
    COLORREF GetBlueFirst() const;
    COLORREF GetBlueLast() const;

    COLORREF GetColorRef() const;

private:
    void SetRGBPlain(int R, int G, int B);
};

inline bool CColorUnion::IsUpdated() const { return m_bUpdated; }
inline void CColorUnion::SetUpdated(bool bUpdated) { m_bUpdated = bUpdated; }

inline void CColorUnion::SetRGBPlain(int R, int G, int B)
{
    GetRed()   = R;
    GetGreen() = G;
    GetBlue()  = B;
    SetUpdated(true);
}

inline void CColorUnion::SetRGB(int R, int G, int B)
{
    SetRGBPlain(R, G, B);
    UpdateHSV();
}

inline void CColorUnion::SetHSV(double dH, double dS, double dV)
{
    m_dH = std::min<double>(dH, 359.);
    m_dS = std::min<double>(dS, 100.);
    m_dV = std::min<double>(dV, 100.);
    UpdateRGB();
}

inline BYTE     CColorUnion::GetRed() const { return m_Red; }
inline BYTE   CColorUnion::GetGreen() const { return m_Green; }
inline BYTE    CColorUnion::GetBlue() const { return m_Blue; }
inline BYTE   CColorUnion::GetAlpha() const { return m_Alpha; }
inline BYTE&    CColorUnion::GetRed() { return m_Red; }
inline BYTE&  CColorUnion::GetGreen() { return m_Green; }
inline BYTE&   CColorUnion::GetBlue() { return m_Blue; }
inline BYTE&  CColorUnion::GetAlpha() { return m_Alpha; }

inline COLORREF   CColorUnion::GetRedFirst() const { return RGB(       0, GetGreen(), GetBlue()); }
inline COLORREF    CColorUnion::GetRedLast() const { return RGB(     255, GetGreen(), GetBlue()); }
inline COLORREF CColorUnion::GetGreenFirst() const { return RGB(GetRed(),          0, GetBlue()); }
inline COLORREF  CColorUnion::GetGreenLast() const { return RGB(GetRed(),        255, GetBlue()); }
inline COLORREF  CColorUnion::GetBlueFirst() const { return RGB(GetRed(), GetGreen(),         0); }
inline COLORREF   CColorUnion::GetBlueLast() const { return RGB(GetRed(), GetGreen(),       255); }

inline COLORREF   CColorUnion::GetColorRef() const { return RGB(GetRed(), GetGreen(), GetBlue()); }
