#pragma once

#include "luicFontDef.h"
#include <SDKDDKVER.h>
#include <WinUser.h>
#include <atluser.h>
#include <atlgdi.h>

struct CPageAppearance;
class CLUIApp;

enum SIZE_NAMES : int
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

struct SizeRange
{
    int     min;
    int     max;
    int current;
};

enum FONT_NAMES : int
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

enum COLOR_NAMES : int
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

    static PCTSTR SizeName(int size);
    static PCTSTR FontName(int font);
    static PCTSTR ColorName(int color);

    bool LoadSysTheme();
    COLORREF GetColor(int color) const;
    HBRUSH GetBrush(int color) const;
    HFONT GetFont(int font) const;
    LOGFONT const* GetLogFont(int font) const;
    bool IsGradientCaptions() const;
    bool IsFlatMenus() const;
    NONCLIENTMETRICS const& GetNcMetrcs() const;

    static void PerformStaticInit(CPageAppearance& uiPage, CLUIApp const* pApp);

private:
    static const SizeRange g_DefaultSizeRange[SIZES_Count];

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

    static void LoadThemes(CPageAppearance& uiPage, CTheme& initialTheme);
    static void LoadElements(CPageAppearance& uiPage);
    static void LoadFonts(CPageAppearance& uiPage, FontMap const&);
};

inline bool             CTheme::IsGradientCaptions() const { return m_bGradientCaptions; }
inline bool                    CTheme::IsFlatMenus() const { return m_bFlatMenus; }
inline NONCLIENTMETRICS const& CTheme::GetNcMetrcs() const { return m_ncMetrics; }

template <typename Res>
inline Res ScaleForDpi(Res n)
{
    return MulDiv(static_cast<int>(n), CTheme::g_DPI, USER_DEFAULT_SCREEN_DPI);
}
