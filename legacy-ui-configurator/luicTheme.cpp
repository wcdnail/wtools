#include "stdafx.h"
#include "luicTheme.h"
#include "luicAppearance.h"
#include "luicMain.h"
#include "string.utils.format.h"
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

PCItemAssign CTheme::GetItemAssignment(int dex)
{
    // Assign the color and metric numbers to each element of the combo box
    //                              Name  Size 1                Size 2               Color 1                 Color 2                         Font            Fontcolor
    static const ItemAssignment gsl_assignment[EN_Count] =
    {
    /* 0*/ {                  L"Desktop", -1,                   -1,                  COLOR_DESKTOP,          -1,                             FONT_Desktop,   -1                          },
    /* 1*/ {   L"Application Background", -1,                   -1,                  COLOR_APPWORKSPACE,     -1,                             -1,             -1                          },
    /* 2*/ {                   L"Window", -1,                   -1,                  COLOR_WINDOW,           COLOR_WINDOWFRAME,              -1,             COLOR_WINDOWTEXT            },
    /* 3*/ {                     L"Menu", SIZE_MenuHeight,      SIZE_MenuWidth,      COLOR_MENU,             -1,                             FONT_Menu,      COLOR_MENUTEXT              },
    /* 4*/ {       L"Title Bar - Active", SIZE_CaptionHeight,   SIZE_CaptionWidth,   COLOR_ACTIVECAPTION,    COLOR_GRADIENTACTIVECAPTION,    FONT_Caption,   COLOR_CAPTIONTEXT           },
    /* 5*/ {     L"Title Bar - Inactive", SIZE_CaptionHeight,   SIZE_CaptionWidth,   COLOR_INACTIVECAPTION,  COLOR_GRADIENTINACTIVECAPTION,  FONT_Caption,   COLOR_INACTIVECAPTIONTEXT   },
    /* 6*/ {      L"Title Bar - Palette", SIZE_SMCaptionHeight, SIZE_SMCaptionWidth, -1,                     -1,                             FONT_SMCaption, -1                          },
    /* 7*/ {   L"Window Border - Active", SIZE_Border,          -1,                  COLOR_ACTIVEBORDER,     -1,                             -1,             -1                          },
    /* 8*/ { L"Window Border - Inactive", SIZE_Border,          -1,                  COLOR_INACTIVEBORDER,   -1,                             -1,             -1                          },
    /* 9*/ {                L"Scrollbar", SIZE_ScrollWidth,     SIZE_ScrollHeight,   COLOR_SCROLLBAR,        -1,                             -1,             -1                          },
    /*10*/ {                L"3D Object", -1,                   -1,                  COLOR_3DFACE,           -1,                             -1,             COLOR_BTNTEXT               },
    /*11*/ {                L"3D Shadow", -1,                   -1,                  COLOR_3DSHADOW,         COLOR_3DDKSHADOW,               -1,             -1                          },
    /*12*/ {                 L"3D Light", -1,                   -1,                  COLOR_3DHILIGHT,        COLOR_3DLIGHT,                  -1,             -1                          },
    /*13*/ {            L"Selected Item", -1,                   -1,                  COLOR_HIGHLIGHT,        -1,                             -1,             COLOR_HIGHLIGHTTEXT         },
    /*14*/ {            L"Disabled Item", -1,                   -1,                  -1,                     -1,                             -1,             COLOR_GRAYTEXT              },
    /*15*/ {                  L"ToolTip", -1,                   -1,                  COLOR_INFOBK,           -1,                             FONT_Tooltip,   COLOR_INFOTEXT              },
    /*16*/ {              L"Message Box", -1,                   -1,                  -1,                     -1,                             FONT_Message,   COLOR_WINDOWTEXT            },
    /*17*/ {                L"Hyperlink", -1,                   -1,                  -1,                     -1,                             -1,             COLOR_HOTLIGHT              },
    /*18*/ {          L"Menu Bar (Flat)", -1,                   -1,                  COLOR_MENUBAR,          COLOR_MENUHILIGHT,              -1,             -1                          },
    /*19*/ {     L"Window Padded Border", SIZE_PaddedBorder,    -1,                  -1,                     -1,                             -1,             -1                          },
    };
    if (dex < 0 || dex >= EN_Count) {
        return nullptr;
    }
    return &gsl_assignment[dex];
}

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
    :            m_nIndex{TI_Invalid}
    ,            m_MyName{"Native"}
    ,        m_lfIconFont{}
    , m_bGradientCaptions{true}
    ,        m_bFlatMenus{false}
{
    ZeroMemory(&m_ncMetrics, sizeof(m_ncMetrics));
    ZeroMemory(&m_Color, sizeof(m_Color));
    for (int i = 0; i < SIZES_Count; i++) {
        m_SizeRange[i] = g_DefaultSizeRange[i];
    }
    if (loadSystemTheme) {
        LoadSysTheme();
    }
}

_Ret_maybenull_
PCTSTR CTheme::SizeName(int size) // it for tooltips!
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
        TEXT("Caption Font"),           // 0 = FONT_Caption
        TEXT("Small Caption Font"),     // 1 = FONT_SMCaption
        TEXT("Menu Font"),              // 2 = FONT_Menu
        TEXT("Status Font"),            // 3 = FONT_Tooltip
        TEXT("Message Font"),           // 4 = FONT_Message
        TEXT("Desktop Font"),           // 5 = FONT_Desktop
#if WINVER >= WINVER_2K
        TEXT("Hyperlink Font"),         // 8 = FONT_Hyperlink
#endif
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

int CTheme::GetNcMetricSize(NONCLIENTMETRICS const* ncMetrics, int size)
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

LOGFONT const* CTheme::GetNcMetricFont(CTheme const& theme, int font)
{
    switch (font) {
    case FONT_Caption:   return &theme.m_ncMetrics.lfCaptionFont;
    case FONT_SMCaption: return &theme.m_ncMetrics.lfSmCaptionFont;
    case FONT_Menu:      return &theme.m_ncMetrics.lfMenuFont;
    case FONT_Tooltip:   return &theme.m_ncMetrics.lfStatusFont;
    case FONT_Message:   return &theme.m_ncMetrics.lfMessageFont;
    case FONT_Desktop:
    default:
        break;
    }
    return &theme.m_lfIconFont;
}

bool CTheme::RefreshBrushes()
{
    WTL::CBrush tmpBrush[CLR_Count];
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
    WTL::CFont tmpFont[FONTS_Count];
    for (int iFont = 0; iFont < FONTS_Count; iFont++) {
        LOGFONT const* lf = GetNcMetricFont(*this, iFont);
        if (!lf) {
            // ##TODO: придумать решение
            continue;
        }
        WTL::CLogFont lfObj(*lf);
#if WINVER >= WINVER_2K
        if (FONT_Hyperlink == iFont) {
            lfObj.lfUnderline = TRUE;
        }
#endif
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
    const BOOL ret = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncMetrics, 0);
    if (!ret) {
        return false;
    }
    CopyMemory(&m_ncMetrics, &ncMetrics, sizeof(ncMetrics));
    ATLASSUME(memcmp(&m_ncMetrics, &ncMetrics, sizeof(ncMetrics)) == 0);
    SizeRange sizeRanges[SIZES_Count] = { 0 };
    for (int iSize = 0; iSize < SIZES_Count; iSize++) {
        m_SizeRange[iSize].current = GetNcMetricSize(&ncMetrics, iSize);
    }
    RefreshFonts();
    return true;
}

bool CTheme::LoadSysIconFont()
{
    WTL::CLogFont lfIconFont;
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
    m_MyName = L"(Current)";
    return ret;
}

COLORREF CTheme::GetColor(int color) const
{
    if (color < 0 || color >= CLR_Count) {
        return CLR_INVALID;
    }
    return m_Color[color];
}

bool CTheme::SetColor(int iColor, COLORREF clrNew)
{
    if (iColor < 0 || (iColor > CLR_Count - 1)) {
        return false;
    }
    if (clrNew == m_Color[iColor]) {
        // Same color...
        return false;
    }
    // TODO: implement history...
    m_Color[iColor] = clrNew;
    WTL::CBrush tmpBrush;
    tmpBrush.CreateSolidBrush(m_Color[iColor]);
    m_Brush[iColor].Attach(tmpBrush.Detach());
    return true;
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

LOGFONT const* CTheme::GetLogFont(int font) const
{
    return GetNcMetricFont(*this, font);
}

SizeRange const* CTheme::GetSizeRange(int metric) const
{
    if ((metric < 0) || (metric > SIZES_Count - 1)) {
        return nullptr;
    }
    return &m_SizeRange[metric];
}

void CTheme::ThemesStaticInit(CPageAppearance& uiPage, CTheme& initialTheme)
{
    WTL::CComboBox& lbCtl = uiPage.m_cbTheme;
    lbCtl.ResetContent();
    int item = lbCtl.AddString(initialTheme.m_MyName);
    if (item < 0) {
        auto code = static_cast<HRESULT>(GetLastError());
        ReportError(
            Str::ElipsisW::Format(L"Append listbox [w:%08x] item '%s' failed!", 
                lbCtl.m_hWnd, initialTheme.m_MyName.GetString()
            ), code);
        return ;
    }
    initialTheme.m_nIndex = item;
    lbCtl.SetItemData(item, static_cast<DWORD_PTR>(item));

    uiPage.m_cbThemeSize.AddString(L"Normal");
    uiPage.m_cbThemeSize.SetCurSel(0);
}

void CTheme::ItemStaticInit(CPageAppearance& uiPage)
{
    WTL::CComboBox& lbCtl = uiPage.m_cbItem;
    lbCtl.ResetContent();
    for (int iElement = 0; iElement < EN_Count; iElement++) {
        const auto* assignment = GetItemAssignment(iElement);
        if (!assignment) {
            auto code = static_cast<HRESULT>(ERROR_ACCESS_DENIED);
            ReportError(
                Str::ElipsisW::Format(L"Append listbox [w:%08x] item [%d] '%s' failed!", 
                    lbCtl.m_hWnd, iElement, L"Element::Assignment == NULL"
                ), code);
            continue;
        }
        int item = lbCtl.AddString(assignment->name);
        if (item < 0) {
            auto code = static_cast<HRESULT>(GetLastError());
            ReportError(
                Str::ElipsisW::Format(L"Append listbox [w:%08x] item [%d] '%s' failed!", 
                    lbCtl.m_hWnd, iElement, assignment->name
                ), code);
            continue;
        }
        lbCtl.SetItemData(item, static_cast<DWORD_PTR>(iElement));
        if (EN_Desktop == iElement) {
            lbCtl.SetCurSel(iElement);
        }
    }
}

void CTheme::FontsSizeStaticInit(CPageAppearance& uiPage)
{
    WTL::CComboBox& lbCtl = uiPage.m_cbFontSize;
    lbCtl.ResetContent();
    for (int size = MIN_FONT_SIZE; size <= MAX_FONT_SIZE; size++) {
        auto str = std::to_wstring(size);
        int item = lbCtl.AddString(str.c_str());
        if (item < 0) {
            auto code = static_cast<HRESULT>(GetLastError());
            ReportError(
                Str::ElipsisW::Format(L"Append listbox [w:%08x] item '%s' failed!", 
                    lbCtl.m_hWnd, str.c_str()
                ), code);
            continue;
        }
        lbCtl.SetItemData(item, static_cast<DWORD_PTR>(size));
    }
    lbCtl.SetCurSel(0);
}

void CTheme::FontsButtonsStaticInit(CPageAppearance& uiPage)
{
    static WTL::CFont gs_fntBold;
    static WTL::CFont gs_fntItalic;
    static WTL::CFont gs_fntUnderline;

    if (!gs_fntBold.m_hFont) {
        HFONT hFont = uiPage.m_bnFontBold.GetFont();
        LOGFONTW lf;
        GetObjectW(hFont, sizeof(lf), &lf);
        lf.lfWeight = FW_BOLD;
        lf.lfItalic = FALSE;
        lf.lfUnderline = FALSE;
        gs_fntBold.CreateFontIndirectW(&lf);
        lf.lfWeight = FW_NORMAL;
        lf.lfItalic = TRUE;
        gs_fntItalic.CreateFontIndirectW(&lf);
        lf.lfItalic = FALSE;
        lf.lfUnderline = TRUE;
        gs_fntUnderline.CreateFontIndirectW(&lf);
    }

    uiPage.m_bnFontBold.SetFont(gs_fntBold);
    uiPage.m_bnFontItalic.SetFont(gs_fntItalic);
    uiPage.m_bnFontUndrln.SetFont(gs_fntUnderline);
}

void CTheme::FontsSmoothStaticInit(CPageAppearance& uiPage)
{
    struct FontSmoothDef
    {
        PCWSTR szName;
        int     nType;
    };
    static const FontSmoothDef fsdData[] = {
        { L"Default", DEFAULT_QUALITY }, 
        { L"Disabled", NONANTIALIASED_QUALITY }, 
        { L"ClearType", CLEARTYPE_QUALITY }, 
        { L"Natural ClearType", CLEARTYPE_NATURAL_QUALITY }, 
    };

    WTL::CComboBox& lbCtl = uiPage.m_cbFontSmooth;
    lbCtl.ResetContent();
    for (auto const& it: fsdData) {
        int item = lbCtl.AddString(it.szName);
        if (item < 0) {
            auto code = static_cast<HRESULT>(GetLastError());
            ReportError(
                Str::ElipsisW::Format(L"Append listbox [w:%08x] item '%s' failed!", 
                    lbCtl.m_hWnd, it.szName
                ), code);
            continue;
        }
        lbCtl.SetItemData(item, it.nType);
    }
    lbCtl.SetCurSel(0);
}

void CTheme::FontsStaticInit(CPageAppearance& uiPage, FontMap const& mapFont)
{
    WTL::CComboBox& lbCtl = uiPage.m_cbFont;
    lbCtl.ResetContent();
    for (auto const& it: mapFont) {
        if (L'@' == it.first[0]) {
            // ##TODO: skip @'at'ed font?
            continue;
        }
        int item = lbCtl.AddString(it.first.c_str());
        if (item < 0) {
            auto code = static_cast<HRESULT>(GetLastError());
            ReportError(
                Str::ElipsisW::Format(L"Append listbox [w:%08x] item '%s' failed!", 
                    lbCtl.m_hWnd, it.first.c_str()
                ), code);
            continue;
        }
    }
    lbCtl.SetCurSel(0);
    FontsSizeStaticInit(uiPage);
    FontsSmoothStaticInit(uiPage);
    FontsButtonsStaticInit(uiPage);
    uiPage.m_udFontWidth.SetRange32(0, 24);
    uiPage.m_udFontAngle.SetRange32(0, 359);
}

void CTheme::PerformStaticInit(CPageAppearance& uiPage, CLUIApp const* pApp)
{
    CTheme& theme = pApp->GetTheme(TI_Current);
    ThemesStaticInit(uiPage, theme);
    ItemStaticInit(uiPage);
    FontsStaticInit(uiPage, pApp->GetFontMap());
    uiPage.OnThemeSelect(theme.m_nIndex);
}
