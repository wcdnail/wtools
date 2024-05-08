#include "stdafx.h"
#include "luicThemeFonts.h"
#include "string.utils.format.h"
#include "resz/resource.h"

CThemeFonts::~CThemeFonts() = default;

CThemeFonts::CThemeFonts()
{
}

namespace
{
template <typename Res>
static inline Res GetCurrentDPI()
{
    int temp = USER_DEFAULT_SCREEN_DPI;
    HDC screenDc{ GetDC(nullptr) };
    if (screenDc) {
        temp = GetDeviceCaps(screenDc, LOGPIXELSY);
        ReleaseDC(nullptr, screenDc);
    }
    return static_cast<Res>(temp);
}
}

int CThemeFonts::g_DPI()
{
    static int gs_DPI = GetCurrentDPI<int>();
    return gs_DPI;
}

PCTSTR CThemeFonts::FontName(int font)
{
    static const PCTSTR gsl_fontNames[FONTS_Count] = {
        TEXT("Caption Font"),           // 0 = FONT_Caption
        TEXT("Small Caption Font"),     // 1 = FONT_SMCaption
        TEXT("Menu Font"),              // 2 = FONT_Menu
        TEXT("Status Font"),            // 3 = FONT_Tooltip
        TEXT("Message Font"),           // 4 = FONT_Message
        TEXT("Desktop Font"),           // 5 = FONT_Desktop
        TEXT("Hyperlink Font"),         // 8 = FONT_Hyperlink
    };
    if (font < 0 || font >= FONTS_Count) {
        return nullptr;
    }
    return gsl_fontNames[font];
}

template <typename RetType, typename SelfType>
RetType CThemeFonts::GetHFontT(SelfType& self, int font)
{
    switch (font) {
    case FONT_Caption:
    case FONT_SMCaption:
    case FONT_Menu:
    case FONT_Tooltip:
    case FONT_Message:
    case FONT_Desktop:
    case FONT_Hyperlink:
        return self.m_hFont[font];
    default:
        break;
    }
    return self.m_hFontReserved;
}

template <typename RetType, typename SelfType>
RetType CThemeFonts::GetLogFontT(SelfType& self, int font)
{
    switch (font) {
    case FONT_Caption:
    case FONT_SMCaption:
    case FONT_Menu:
    case FONT_Tooltip:
    case FONT_Message:
    case FONT_Desktop:
    case FONT_Hyperlink:
        return self.m_logFont[font];
    default:
        break;
    }
    return self.m_logFontReserved;
}

WTL::CFont const& CThemeFonts::GetFont(int font) const { return GetHFontT<WTL::CFont const&>(*this, font); }
WTL::CFont&       CThemeFonts::GetFont(int font)       { return GetHFontT<WTL::CFont&>(*this, font); }
LOGFONT const& CThemeFonts::GetLogFont(int font) const { return GetLogFontT<WTL::CLogFont const&>(*this, font); }
LOGFONT&       CThemeFonts::GetLogFont(int font)       { return GetLogFontT<WTL::CLogFont&>(*this, font); }

//static bool DoFontRefresh(WTL::CFont& fntTarget, LOGFONT const& logFont)
//{
//    WTL::CFont temp{CreateFontIndirectW(&logFont)};
//    if (!temp.m_hFont) {
//        // TODO: report CreateFontIndirectW
//        return false;
//    }
//    fntTarget.Attach(temp.Detach());
//    return true;
//}
//
//bool CThemeFonts::RefreshHFont(int font, LOGFONT const& logFont)
//{
//    WTL::CFont& fntTarget = GetNcMetricHFontT<WTL::CFont&, CThemeFonts&>(*this, font);
//    return DoFontRefresh(fntTarget, logFont);
//}
//
//bool CThemeFonts::RefreshHFonts()
//{
//    WTL::CFont tmpFont[FONTS_Count];
//    for (int iFont = 0; iFont < FONTS_Count; iFont++) {
//        auto& logFont = static_cast<WTL::CLogFont&>(GetNcMetricFont(*this, iFont));
//        tmpFont[iFont] = logFont.CreateFontIndirectW();
//    }
//    for (int iFont = 0; iFont < FONTS_Count; iFont++) {
//        m_Font[iFont].Attach(tmpFont[iFont].Detach());
//    }
//    return true;
//}

void CThemeFonts::SetFont(int iFont, const WTL::CLogFont& lfNew, HFONT fnNew)
{
    GetLogFont(iFont) = lfNew;
    GetFont(iFont) = fnNew;
}

