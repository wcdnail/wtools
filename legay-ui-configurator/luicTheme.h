#pragma once

#include <SDKDDKVER.h>
#include <WinUser.h>
#include <atluser.h>
#include <atlgdi.h>

struct CTheme
{
    enum SizeName: int
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

    static const SizeRange g_DefaultSizeRange[CTheme::SIZES_Count];

#if WINVER < WINVER_2K
    static const int COLORS_COUNT = 25;
#elif WINVER < WINVER_XP
    static const int COLORS_COUNT = 29;
#else
    static const int COLORS_COUNT = 31;
#endif

    enum ColorName : int
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

    static_assert(CLR_Count == COLORS_COUNT, "Colors COUNT is NOT match!");

    ATL::CString          m_SchemeName;
    CLogFont              m_lfIconFont;
    bool           m_bGradientCaptions;
    bool                  m_bFlatMenus;
    NONCLIENTMETRICS       m_ncMetrics;
    COLORREF     m_Color[COLORS_COUNT];
    CBrush       m_Brush[COLORS_COUNT];
    SizeRange m_SizeRange[SIZES_Count];

    ~CTheme();
    CTheme(bool loadSystemTheme);

    static PCTSTR SizeName(int size);
    static PCTSTR ColorName(int color);

    bool LoadSysTheme();

private:
    bool RefreshBrushes();
    bool LoadSysColors();
    bool LoadSysNcMetrics();
    bool LoadSysIconFont();
    bool LoadSysGradientCaptionsSetting();
    bool LoadSysFlatMenusSetting();
};
