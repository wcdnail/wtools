#pragma once

#include "luicFontDef.h"
#include <SDKDDKVER.h>
#include <WinUser.h>
#include <atluser.h>
#include <atlgdi.h>

struct CPageAppearance;
class CLUIApp;

enum ThemeIndex : int
{
    TI_Invalid = -1,
    TI_Current,
};

enum EItemIndex : int
{
    EN_Desktop,
    EN_AppBackground,
    EN_Window,
    EN_Menu,             // The width affects the icon and caption buttons of MDI children
    EN_ActiveCaption,    // Title bar of active windows
    EN_InactiveCaption,  // Title bar of inactive windows
    EN_SMCaption,        // Title bar of palette (small) windows
    EN_ActiveBorder,     // Border of active resizable windows
    EN_InactiveBorder,   // Border of inactive resizable windows
    EN_Scrollbar,
    EN_3DObject,
    EN_3DShadow,         // Not in official applets
    EN_3DLight,          // Not in official applets
    EN_SelectedItem,     // Also used for text selection
    EN_DisabledItem,
    EN_Tooltip,
    EN_MsgBox,
    EN_Hyperlink,
    EN_FlatmenuBar,
    /**
      * Border of windows, including property sheets for some reason.
      * If SIZE_PADDEDBORDER is > 0, the windows with fixed borders affected also
      * include SIZE_BORDER
      */
    EN_PaddedBorder,
    EN_Count
};

enum ESizeIndex : int
{
    SIZE_Border,
    SIZE_ScrollWidth,
    SIZE_ScrollHeight,
    SIZE_CaptionWidth,
    SIZE_CaptionHeight,
    SIZE_SMCaptionWidth,
    SIZE_SMCaptionHeight,
    SIZE_MenuWidth,
    SIZE_MenuHeight,
#if WINVER >= WINVER_VISTA
    SIZE_PaddedBorder,
#endif
    SIZES_Count
};

struct ItemAssignment
{
    PCWSTR   name;
    int     size1;
    int     size2;
    int    color1;
    int    color2;
    int      font;
    int fontColor;
};

using  PItemAssign = ItemAssignment*;
using PCItemAssign = ItemAssignment const*;

struct SizeRange
{
    int     min;
    int     max;
    int current;
};

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
#if WINVER >= WINVER_2K
    FONT_Hyperlink,
#endif
    FONTS_Count
};

enum EColorIndex : int
{
    CLR_Scrollbar = 0,          // 00 = COLOR_SCROLLBAR
    CLR_Background,             // 01 = COLOR_BACKGROUND
    // COLOR_DESKTOP -------------------------------------------------
    CLR_ActiveTitle,            // 02 = COLOR_ACTIVECAPTION
    CLR_InactiveTitle,          // 03 = COLOR_INACTIVECAPTION
    CLR_Menu,                   // 04 = COLOR_MENU
    CLR_Window,                 // 05 = COLOR_WINDOW
    CLR_WindowFrame,            // 06 = COLOR_WINDOWFRAME
    CLR_MenuText,               // 07 = COLOR_MENUTEXT
    CLR_WindowText,             // 08 = COLOR_WINDOWTEXT
    CLR_TitleText,              // 09 = COLOR_CAPTIONTEXT
    CLR_ActiveBorder,           // 10 = COLOR_ACTIVEBORDER
    CLR_InactiveBorder,         // 11 = COLOR_INACTIVEBORDER
    CLR_AppWorkSpace,           // 12 = COLOR_APPWORKSPACE
    CLR_Hilight,                // 13 = COLOR_HIGHLIGHT
    CLR_HilightText,            // 14 = COLOR_HIGHLIGHTTEXT
    CLR_ButtonFace,             // 15 = COLOR_BTNFACE
    // COLOR_3DFACE --------------------------------------------------
    CLR_ButtonShadow,           // 16 = COLOR_BTNSHADOW
    // COLOR_3DSHADOW ------------------------------------------------
    CLR_GrayText,               // 17 = COLOR_GRAYTEXT
    // (Disabled menu item selection) --------------------------------
    CLR_ButtonText,             // 18 = COLOR_BTNTEXT
    CLR_InactiveTitleText,      // 19 = COLOR_INACTIVECAPTIONTEXT
    CLR_ButtonHilight,          // 20 = COLOR_BTNHIGHLIGHT
    // COLOR_BTNHILIGHT
    // COLOR_3DHIGHLIGHT
    // COLOR_3DHILIGHT
    CLR_ButtonDkShadow,         // 21 = COLOR_3DDKSHADOW
    CLR_ButtonLight,            // 22 = COLOR_3DLIGHT
    CLR_InfoText,               // 23 = COLOR_INFOTEXT
    CLR_InfoWindow,             // 24 = COLOR_INFOBK
#if WINVER >= WINVER_2K
    CLR_ButtonAlternateFace,    // 25 = COLOR_ALTERNATEBTNFACE
    // (unused, undefined by the SDK) --------------------------------
    CLR_HotTrackingColor,       // 26 = COLOR_HOTLIGHT (Hyperlink)
    CLR_GradientActiveTitle,    // 27 = COLOR_GRADIENTACTIVECAPTION
    CLR_GradientInactiveTitle,  // 28 = COLOR_GRADIENTINACTIVECAPTION
#endif
#if WINVER >= WINVER_XP
    CLR_MenuHilight,            // 29 = COLOR_MENUHILIGHT
    CLR_MenuBar,                // 30 = COLOR_MENUBAR
#endif
    CLR_Count
};

struct CTheme
{
    static int g_DPI;

    ~CTheme();
    CTheme(bool loadSystemTheme);

    static PCItemAssign GetItemAssignment(int dex);
    static PCTSTR SizeName(int size);
    static PCTSTR FontName(int font);
    static PCTSTR ColorName(int color);
    static int GetNcMetricSize(NONCLIENTMETRICS const* ncMetrics, int size);

    bool LoadSysTheme();
    COLORREF GetColor(int color) const;
    HBRUSH GetBrush(int color) const;
    HFONT GetFont(int font) const;
    LOGFONT const* GetLogFont(int font) const;
    SizeRange const* GetSizeRange(int metric) const;
    bool IsGradientCaptions() const;
    bool IsFlatMenus() const;
    NONCLIENTMETRICS const& GetNcMetrcs() const;

    static void PerformStaticInit(CPageAppearance& uiPage, CLUIApp const* pApp);

private:
    static const SizeRange g_DefaultSizeRange[SIZES_Count];

    int                       m_nIndex;
    ATL::CString              m_MyName;
    CLogFont              m_lfIconFont;
    bool           m_bGradientCaptions;
    bool                  m_bFlatMenus;
    NONCLIENTMETRICS       m_ncMetrics;
    COLORREF        m_Color[CLR_Count];
    CBrush          m_Brush[CLR_Count];
    CFont          m_Font[FONTS_Count];
    SizeRange m_SizeRange[SIZES_Count];

    static LOGFONT const* GetNcMetricFont(CTheme const& theme, int font);

    bool RefreshBrushes();
    bool RefreshFonts();
    bool LoadSysColors();
    bool LoadSysNcMetrics();
    bool LoadSysIconFont();
    bool LoadSysGradientCaptionsSetting();
    bool LoadSysFlatMenusSetting();

    static void ThemesStaticInit(CPageAppearance& uiPage, CTheme& initialTheme);
    static void ElementsStaticInit(CPageAppearance& uiPage);
    static void FontsSizeStaticInit(CPageAppearance& uiPage);
    static void FontsButtonsStaticInit(CPageAppearance& uiPage);
    static void FontsSmoothStaticInit(CPageAppearance& uiPage);
    static void FontsStaticInit(CPageAppearance& uiPage, FontMap const&);
};

inline bool             CTheme::IsGradientCaptions() const { return m_bGradientCaptions; }
inline bool                    CTheme::IsFlatMenus() const { return m_bFlatMenus; }
inline NONCLIENTMETRICS const& CTheme::GetNcMetrcs() const { return m_ncMetrics; }

template <typename Res>
inline Res ScaleForDpi(Res n)
{
    return static_cast<Res>(MulDiv(static_cast<int>(n), CTheme::g_DPI, USER_DEFAULT_SCREEN_DPI));
}

static constexpr long DEFAULT_FONT_DPI = 72;

template <typename Res>
inline Res FontLogToPt(long n)
{
    return -static_cast<Res>(MulDiv(n, DEFAULT_FONT_DPI, CTheme::g_DPI));
}
