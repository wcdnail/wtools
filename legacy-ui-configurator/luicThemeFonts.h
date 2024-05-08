#pragma once

#include <atlstr.h>
#include <atlgdi.h>
#include <atluser.h>

enum EFontSizes : long
{
    MIN_FONT_SIZE = 6,
    MAX_FONT_SIZE = 24,
};

enum EFontIndex : int
{
    FONT_Caption,
    FONT_SMCaption,
    FONT_Menu,
    FONT_Tooltip,
    FONT_Message,
    FONT_Desktop,
    FONT_Hyperlink,
    FONTS_Count
};

struct CThemeFonts
{
    static constexpr long DEFAULT_FONT_DPI = 72;

    static int g_DPI();
    static PCTSTR FontName(int font);

    ~CThemeFonts();
    CThemeFonts();

    WTL::CFont const& GetFont(int font) const;
    WTL::CFont& GetFont(int font);

    LOGFONT const& GetLogFont(int font) const;
    LOGFONT& GetLogFont(int font);

    void SetFont(int iFont, const WTL::CLogFont& lfNew, HFONT fnNew);

private:
    WTL::CFont           m_hFontReserved;
    WTL::CLogFont      m_logFontReserved;
    WTL::CFont      m_hFont[FONTS_Count];
    WTL::CLogFont m_logFont[FONTS_Count];

    template <typename RetType, typename SelfType>
    static RetType GetHFontT(SelfType& self, int font);

    template <typename RetType, typename SelfType>
    static RetType GetLogFontT(SelfType& self, int font);

    bool LoadCurrent();
};

template <typename Res>
inline Res ScaleForDpi(Res n)
{
    return static_cast<Res>(MulDiv(static_cast<int>(n), CThemeFonts::g_DPI(), USER_DEFAULT_SCREEN_DPI));
}

template <typename Res>
inline Res FontLogToPt(Res n)
{
    return -static_cast<Res>(MulDiv(static_cast<int>(n), CThemeFonts::DEFAULT_FONT_DPI, CThemeFonts::g_DPI()));
}

template <typename Res>
inline Res FontPtToLog(Res n)
{
    return -static_cast<Res>(MulDiv(static_cast<int>(n), CThemeFonts::g_DPI(), CThemeFonts::DEFAULT_FONT_DPI));
}
