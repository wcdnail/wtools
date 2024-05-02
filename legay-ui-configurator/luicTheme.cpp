﻿#include "stdafx.h"
#include "luicTheme.h"
#include "resz/resource.h"

namespace
{
#if WINVER < WINVER_2K
    static const int COLORS_COUNT = 25;
#elif WINVER < WINVER_XP
    static const int COLORS_COUNT = 29;
#else
    static const int COLORS_COUNT = 31;
#endif
    static_assert(CLR_Count == COLORS_COUNT, "CLR_Count COUNT is NOT match COLORS_COUNT!");

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

int CTheme::g_DPI = GetCurrentDPI<int>();

const SizeRange CTheme::g_DefaultSizeRange[SIZES_Count] =
{
    // Index                     Min  Max  Current Orig.Max
    /* SIZE_BORDER          */  {  1,  15,  1 },   //  50
    /* SIZE_SCROLLWIDTH     */  {  8,  70, 16 },   // 100
    /* SIZE_SCROLLHEIGHT    */  {  8,  70, 16 },   // 100
    /* SIZE_CAPTIONWIDTH    */  { 12,  70, 18 },   // 100
    /* SIZE_CAPTIONHEIGHT   */  { 12,  70, 18 },   // 100
    /* SIZE_SMCAPTIONWIDTH  */  { 12,  70, 12 },   // 100
    /* SIZE_SMCAPTIONHEIGHT */  { 12,  70, 15 },   // 100
    /* SIZE_MENUWIDTH       */  { 12,  70, 18 },   // 100
    /* SIZE_MENUHEIGHT      */  { 12,  70, 18 },   // 100
#if WINVER >= WINVER_VISTA
    /* SIZE_PADDEDBORDER    */  {  0,  15,  0 }    // 100
#endif
};

CTheme::~CTheme()
{
}

CTheme::CTheme(bool loadSystemTheme)
    :        m_SchemeName("Native")
    ,        m_lfIconFont()
    , m_bGradientCaptions(true)
    ,        m_bFlatMenus(false)
{
    ZeroMemory(&m_ncMetrics, sizeof(m_ncMetrics));
    ZeroMemory(&m_Color, sizeof(m_Color));

    CopyMemory(&m_SizeRange, &g_DefaultSizeRange, sizeof(m_SizeRange));

    if (loadSystemTheme) {
        LoadSysTheme();
    }
}

_Ret_maybenull_
PCTSTR CTheme::SizeName(int size)
{
    static const PCTSTR gsl_sizeNames[SIZES_Count] = {
        TEXT("BorderWidth"),            // 0 = SIZE_BORDER
        TEXT("ScrollWidth"),            // 1 = SIZE_SCROLLWIDTH
        TEXT("ScrollHeight"),           // 2 = SIZE_SCROLLHEIGHT
        TEXT("CaptionWidth"),           // 3 = SIZE_CAPTIONWIDTH
        TEXT("CaptionHeight"),          // 4 = SIZE_CAPTIONHEIGHT
        TEXT("SmCaptionWidth"),         // 5 = SIZE_SMCAPTIONWIDTH
        TEXT("SmCaptionHeight"),        // 6 = SIZE_SMCAPTIONHEIGHT
        TEXT("MenuWidth"),              // 7 = SIZE_MENUWIDTH
        TEXT("MenuHeight"),             // 8 = SIZE_MENUHEIGHT
#if WINVER >= WINVER_VISTA
        TEXT("PaddedBorderWidth")       // 9 = SIZE_PADDEDBORDER
#endif
    };
    if (size < 0 || size >= SIZES_Count) {
        return nullptr;
    }
    return gsl_sizeNames[size];
}

PCTSTR CTheme::FontName(int font)
{
    static const PCTSTR gsl_fontNames[FONTS_Count] = {
        TEXT("CaptionFont"),            // 0 = FONT_CAPTION
        TEXT("SmCaptionFont"),          // 1 = FONT_SMCAPTION
        TEXT("MenuFont"),               // 2 = FONT_MENU
        TEXT("StatusFont"),             // 3 = FONT_TOOLTIP
        TEXT("MessageFont"),            // 4 = FONT_MESSAGE
        TEXT("IconFont")                // 5 = FONT_DESKTOP
    };
    if (font < 0 || font >= FONTS_Count) {
        return nullptr;
    }
    return gsl_fontNames[font];
}

PCTSTR CTheme::ColorName(int color)
{
    static const PCTSTR gsl_ColorName[CLR_Count] = {
        TEXT("Scrollbar"),              // 00 = COLOR_SCROLLBAR
        TEXT("Background"),             // 01 = COLOR_BACKGROUND
        // COLOR_DESKTOP -------------------------------------------------
        TEXT("ActiveTitle"),            // 02 = COLOR_ACTIVECAPTION
        TEXT("InactiveTitle"),          // 03 = COLOR_INACTIVECAPTION
        TEXT("Menu"),                   // 04 = COLOR_MENU
        TEXT("Window"),                 // 05 = COLOR_WINDOW
        TEXT("WindowFrame"),            // 06 = COLOR_WINDOWFRAME
        TEXT("MenuText"),               // 07 = COLOR_MENUTEXT
        TEXT("WindowText"),             // 08 = COLOR_WINDOWTEXT
        TEXT("TitleText"),              // 09 = COLOR_CAPTIONTEXT
        TEXT("ActiveBorder"),           // 10 = COLOR_ACTIVEBORDER
        TEXT("InactiveBorder"),         // 11 = COLOR_INACTIVEBORDER
        TEXT("AppWorkSpace"),           // 12 = COLOR_APPWORKSPACE
        TEXT("Hilight"),                // 13 = COLOR_HIGHLIGHT
        TEXT("HilightText"),            // 14 = COLOR_HIGHLIGHTTEXT
        TEXT("ButtonFace"),             // 15 = COLOR_BTNFACE
        // COLOR_3DFACE --------------------------------------------------
        TEXT("ButtonShadow"),           // 16 = COLOR_BTNSHADOW
        // COLOR_3DSHADOW ------------------------------------------------
        TEXT("GrayText"),               // 17 = COLOR_GRAYTEXT
        // (Disabled menu item selection) --------------------------------
        TEXT("ButtonText"),             // 18 = COLOR_BTNTEXT
        TEXT("InactiveTitleText"),      // 19 = COLOR_INACTIVECAPTIONTEXT
        TEXT("ButtonHilight"),          // 20 = COLOR_BTNHIGHLIGHT
        // COLOR_BTNHILIGHT
        // COLOR_3DHIGHLIGHT
        // COLOR_3DHILIGHT
        TEXT("ButtonDkShadow"),         // 21 = COLOR_3DDKSHADOW
        TEXT("ButtonLight"),            // 22 = COLOR_3DLIGHT
        TEXT("InfoText"),               // 23 = COLOR_INFOTEXT
        TEXT("InfoWindow"),             // 24 = COLOR_INFOBK
#if WINVER >= WINVER_2K
        TEXT("ButtonAlternateFace"),    // 25 = COLOR_ALTERNATEBTNFACE
        // (unused, undefined by the SDK) --------------------------------
        TEXT("HotTrackingColor"),       // 26 = COLOR_HOTLIGHT (Hyperlink)
        TEXT("GradientActiveTitle"),    // 27 = COLOR_GRADIENTACTIVECAPTION
        TEXT("GradientInactiveTitle"),  // 28 = COLOR_GRADIENTINACTIVECAPTION
#endif
#if WINVER >= WINVER_XP
        TEXT("MenuHilight"),            // 29 = COLOR_MENUHILIGHT
        TEXT("MenuBar")                 // 30 = COLOR_MENUBAR
#endif
    };
    if (color < 0 || color >= CLR_Count) {
        return nullptr;
    }
    return gsl_ColorName[color];
}

static int GetNcMetricSize(NONCLIENTMETRICS const* ncMetrics, int size)
{
    switch (size) {
    case SIZE_Border:           return ncMetrics->iBorderWidth;
    case SIZE_ScrollWidth:      return ncMetrics->iScrollWidth;
    case SIZE_ScrollHeight:     return ncMetrics->iScrollHeight;
    case SIZE_CaptionWidth:     return ncMetrics->iCaptionWidth;
    case SIZE_CaptionHeight:    return ncMetrics->iCaptionHeight;
    case SIZE_SMCaptionWidth:   return ncMetrics->iSmCaptionWidth;
    case SIZE_SMCaptionHeight:  return ncMetrics->iSmCaptionHeight;
    case SIZE_MenuWidth:        return ncMetrics->iMenuWidth;
    case SIZE_MenuHeight:       return ncMetrics->iMenuHeight;
#if WINVER >= WINVER_VISTA
    case SIZE_PaddedBorder:     return ncMetrics->iPaddedBorderWidth;
#endif
    }
    return -1;
}

_Ret_maybenull_
LOGFONT* CTheme::GetNcMetricFont(CTheme& theme, int font)
{
    switch (font) {
    case FONT_Caption:   return &theme.m_ncMetrics.lfCaptionFont;
    case FONT_SMCaption: return &theme.m_ncMetrics.lfSmCaptionFont;
    case FONT_Menu:      return &theme.m_ncMetrics.lfMenuFont;
    case FONT_Tooltip:   return &theme.m_ncMetrics.lfStatusFont;
    case FONT_Message:   return &theme.m_ncMetrics.lfMessageFont;
    case FONT_Desktop:   return &theme.m_lfIconFont;
    }
    return nullptr;
}

bool CTheme::RefreshBrushes()
{
    CBrush tmpBrush[CLR_Count];
    for (int iColor = 0; iColor < CLR_Count; iColor++) {
        if (CLR_INVALID == m_Color[iColor]) {
            // ##TODO: придумать решение
            continue;
        }
        tmpBrush[iColor].CreateSolidBrush(m_Color[iColor]);
    }
    for (int iBrush = 0; iBrush < CLR_Count; iBrush++) {
        m_Brush[iBrush].Attach(tmpBrush[iBrush].Detach());
    }
    return true;
}

bool CTheme::RefreshFonts()
{
    CFont tmpFont[FONTS_Count];
    for (int iFont = 0; iFont < FONTS_Count; iFont++) {
        LOGFONT* lf = GetNcMetricFont(*this, iFont);
        if (!lf) {
            // ##TODO: придумать решение
            continue;
        }
        CLogFont lfObj(*lf);
        tmpFont[iFont] = lfObj.CreateFontIndirectW();
    }
    for (int iFont = 0; iFont < FONTS_Count; iFont++) {
        m_Font[iFont].Attach(tmpFont[iFont].Detach());
    }
    return true;
}

bool CTheme::LoadSysColors()
{
    COLORREF colors[CLR_Count] = { 0 };
    for (int iColor = 0; iColor < CLR_Count; iColor++) {
        const auto color = static_cast<COLORREF>(GetSysColor(iColor));
        if (color == CLR_INVALID) {
            // ##TODO: придумать решение
        }
        colors[iColor] = color;
    }
    CopyMemory(&m_Color, &colors, sizeof(m_Color));
    return RefreshBrushes();
}

bool CTheme::LoadSysNcMetrics()
{
    NONCLIENTMETRICS ncMetrics;
    static_assert(sizeof(m_ncMetrics) == sizeof(ncMetrics), "NONCLIENTMETRICS did not MATCH!");

    ZeroMemory(&ncMetrics, sizeof(ncMetrics));
    ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);

    BOOL ret = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncMetrics, 0);
    if (!ret) {
        return false;
    }
    SizeRange sizeRanges[SIZES_Count] = { 0 };
    for (int iSize = 0; iSize < SIZES_Count; iSize++) {
        sizeRanges[iSize].current = GetNcMetricSize(&ncMetrics, iSize);
    }
    CopyMemory(&m_ncMetrics, &ncMetrics, sizeof(ncMetrics));
    CopyMemory(&m_SizeRange, &sizeRanges, sizeof(sizeRanges));
    RefreshFonts();
    return true;
}

bool CTheme::LoadSysIconFont()
{
    CLogFont lfIconFont;
    if (!SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIconFont), &lfIconFont, 0)) {
        return false;
    }
    m_lfIconFont = lfIconFont;
    return true;
}

bool CTheme::LoadSysGradientCaptionsSetting()
{
#if WINVER >= WINVER_2K
    BOOL bGradientCaptions = FALSE;
    if (!SystemParametersInfo(SPI_GETGRADIENTCAPTIONS, 0, &bGradientCaptions, 0)) {
        return false;
    }
    m_bGradientCaptions = bGradientCaptions != FALSE;
#endif
    return true;

}

bool CTheme::LoadSysFlatMenusSetting()
{
#if WINVER >= WINVER_XP
    BOOL bFlatMenus = FALSE;
    if (!SystemParametersInfo(SPI_GETFLATMENU, 0, &bFlatMenus, 0)) {
        return false;
    }
    m_bFlatMenus = bFlatMenus != FALSE;
#endif
    return true;
}

bool CTheme::LoadSysTheme()
{
    bool ret = true;
    ret &= LoadSysColors();
    ret &= LoadSysIconFont();
    ret &= LoadSysNcMetrics();
    ret &= LoadSysGradientCaptionsSetting();
    ret &= LoadSysFlatMenusSetting();
    return ret;
}

COLORREF CTheme::GetColor(int color) const
{
    if (color < 0 || color >= CLR_Count) {
        return CLR_INVALID;
    }
    return m_Color[color];
}

HBRUSH CTheme::GetBrush(int color) const
{
    if (color < 0 || color >= CLR_Count) {
        return nullptr;
    }
    return m_Brush[color];
}

HFONT CTheme::GetFont(int font) const
{
    if (font < 0 || font >= FONTS_Count) {
        return nullptr;
    }
    return m_Font[font];
}
