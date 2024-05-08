#include "stdafx.h"

#if 0

    static constexpr long DEFAULT_FONT_DPI = 72;
    static int g_DPI();

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

int CFontsArray::g_DPI()
{
    static int gs_DPI = GetCurrentDPI<int>();
    return gs_DPI;
}

template <typename Res>
inline Res ScaleForDpi(Res n)
{
    return static_cast<Res>(MulDiv(static_cast<int>(n), CFontsArray::g_DPI(), USER_DEFAULT_SCREEN_DPI));
}

template <typename Res>
inline Res FontLogToPt(Res n)
{
    return -static_cast<Res>(MulDiv(static_cast<int>(n), CFontsArray::DEFAULT_FONT_DPI, CFontsArray::g_DPI()));
}

template <typename Res>
inline Res FontPtToLog(Res n)
{
    return -static_cast<Res>(MulDiv(static_cast<int>(n), CFontsArray::g_DPI(), CFontsArray::DEFAULT_FONT_DPI));
}



#include "luicTheme.h"
#include "luicAppearance.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "resz/resource.h"

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

CTheme::~CTheme()
{
}

CTheme::CTheme(bool initFromSystem)
    :            m_nIndex{IT_Invalid}
    ,             m_sName{"Native"}
    , m_bGradientCaptions{true}
    ,        m_bFlatMenus{false}
    ,        m_brReserved{}
{
    ZeroMemory(&m_ncMetrics, sizeof(m_ncMetrics));
    ZeroMemory(&m_Color, sizeof(m_Color));
    for (int i = 0; i < SIZES_Count; i++) {
        m_SizeRange[i] = g_DefaultSizeRange[i];
    }
    if (initFromSystem) {
        LoadCurrent();
    }
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

bool CTheme::LoadCurrentColors()
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

bool CTheme::LoadCurrentNcMetrics()
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
    RefreshHFonts();
    return true;
}

bool CTheme::LoadCurrentIconFont()
{
    WTL::CLogFont lfIconFont;
    if (!SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIconFont), &lfIconFont, 0)) {
        return false;
    }
    m_lfIconFont = lfIconFont;
    m_lfHyperlink = lfIconFont;
    m_lfHyperlink.lfUnderline = TRUE;
    return true;
}

bool CTheme::LoadCurrentGradientCaptionsSetting()
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

bool CTheme::LoadCurrentFlatMenusSetting()
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

bool CTheme::LoadCurrent()
{
    bool ret = true;
    ret &= LoadCurrentColors();
    ret &= LoadCurrentIconFont();
    ret &= LoadCurrentNcMetrics();
    ret &= LoadCurrentGradientCaptionsSetting();
    ret &= LoadCurrentFlatMenusSetting();
    m_sName = L"(Current)";
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

void CTheme::SetFont(int iFont, const WTL::CLogFont& lfNew, HFONT fnNew)
{
    GetLogFont(iFont) = lfNew;
    GetFont(iFont) = fnNew;
}

WTL::CBrush const& CTheme::GetBrush(int color) const
{
    if (color < 0 || color > CLR_Count - 1) {
        return m_brReserved;
    }
    return m_Brush[color];
}

WTL::CFont const& CTheme::GetFont(int font) const
{
    return GetNcMetricHFontT<WTL::CFont const&, CTheme const&>(*this, font);
}

WTL::CFont& CTheme::GetFont(int font)
{
    return GetNcMetricHFontT<WTL::CFont&, CTheme&>(*this, font);
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
    int item = lbCtl.AddString(initialTheme.m_sName);
    if (item < 0) {
        auto code = static_cast<HRESULT>(GetLastError());
        ReportError(
            Str::ElipsisW::Format(L"Append listbox [w:%08x] item '%s' failed!", 
                lbCtl.m_hWnd, initialTheme.m_sName.GetString()
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
    uiPage.m_udItemSize[IT_FontWidth].SetRange32(0, 24);
    uiPage.m_udItemSize[IT_FontAngle].SetRange32(0, 359);
}

void CTheme::PerformStaticInit(CPageAppearance& uiPage, CLUIApp const* pApp)
{
    CTheme& theme = pApp->GetTheme(TI_Current);
    ThemesStaticInit(uiPage, theme);
    ItemStaticInit(uiPage);
    FontsStaticInit(uiPage, pApp->GetFontMap());
    uiPage.OnThemeSelect(theme.m_nIndex);
}

#endif
