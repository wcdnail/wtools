#include "stdafx.h"
#include "luicTheme.h"
#include "resz/resource.h"

const CTheme::SizeRange CTheme::g_DefaultSizeRange[CTheme::SIZES_Count] =
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


PCTSTR CTheme::ColorName(int color)
{
    static const PCTSTR gsl_ColorName[COLORS_COUNT] = {
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
    if (color < 0 || color >= COLORS_COUNT) {
        return nullptr;
    }
    return gsl_ColorName[color];
}

static int GetSizeNcMetric(NONCLIENTMETRICS const* ncMetrics, int size)
{
    switch (size) {
    case CTheme::SIZE_Border:           return ncMetrics->iBorderWidth;
    case CTheme::SIZE_ScrollWidth:      return ncMetrics->iScrollWidth;
    case CTheme::SIZE_ScrollHeight:     return ncMetrics->iScrollHeight;
    case CTheme::SIZE_CaptionWidth:     return ncMetrics->iCaptionWidth;
    case CTheme::SIZE_CaptionHeight:    return ncMetrics->iCaptionHeight;
    case CTheme::SIZE_SMCaptionWidth:   return ncMetrics->iSmCaptionWidth;
    case CTheme::SIZE_SMCaptionHeight:  return ncMetrics->iSmCaptionHeight;
    case CTheme::SIZE_MenuWidth:        return ncMetrics->iMenuWidth;
    case CTheme::SIZE_MenuHeight:       return ncMetrics->iMenuHeight;
#if WINVER >= WINVER_VISTA
    case CTheme::SIZE_PaddedBorder:     return ncMetrics->iPaddedBorderWidth;
#endif
    }
    return -1;
}

bool CTheme::RefreshBrushes()
{
    CBrush tmpBrush[COLORS_COUNT];
    for (int iColor = 0; iColor < COLORS_COUNT; iColor++) {
        if (CLR_INVALID != m_Color[iColor]) {
            tmpBrush[iColor].CreateSolidBrush(m_Color[iColor]);
        }
    }
    for (int iBrush = 0; iBrush < COLORS_COUNT; iBrush++) {
        m_Brush[iBrush].Attach(tmpBrush[iBrush].Detach());
    }
    return true;
}

bool CTheme::LoadSysColors()
{
    COLORREF colors[COLORS_COUNT] = { 0 };
    for (int iColor = 0; iColor < COLORS_COUNT; iColor++) {
        const auto color = static_cast<COLORREF>(GetSysColor(iColor));
        if (color == CLR_INVALID) {
            // ##TODO: придумай решение
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
        sizeRanges[iSize].current = GetSizeNcMetric(&ncMetrics, iSize);
    }
    CopyMemory(&m_ncMetrics, &ncMetrics, sizeof(ncMetrics));
    CopyMemory(&m_SizeRange, &sizeRanges, sizeof(sizeRanges));
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
    ret &= LoadSysNcMetrics();
    ret &= LoadSysIconFont();
    ret &= LoadSysGradientCaptionsSetting();
    ret &= LoadSysFlatMenusSetting();

    return ret;
}
