#pragma once

#if 0

#include "luicFontDef.h"
#include "luicThemeFonts.h"
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
    IT_Invalid = -1,
    EN_Desktop = 0,
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
    EN_PaddedBorder,    // Border of windows, including property sheets for some reason.
    EN_Count            // If SIZE_PaddedBorder is > 0, the windows with fixed borders affected also
};                      // include SIZE_Border

enum EItemColors: int
{
    IT_Color1 = 0,
    IT_Color2,
    IT_FontColor1,
    IT_ColorCount
};

enum EItemSize: int 
{
    IT_Size1 = 0,
    IT_Size2,
    IT_FontWidth,
    IT_FontAngle,
    IT_SizeCount
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
    SIZE_PaddedBorder,
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

struct CTheme
{
    ~CTheme();
    CTheme(bool initFromSystem);

    static PCItemAssign GetItemAssignment(int dex);
    static PCTSTR SizeName(int size);
    static PCTSTR ColorName(int color);
    static int& GetNcMetricSize(PNONCLIENTMETRICS pncMetrics, int size);

    COLORREF GetColor(int color) const;
    bool SetColor(int iColor, COLORREF dword);
    WTL::CBrush const& GetBrush(int color) const;
    SizeRange const* GetSizeRange(int metric) const;

    bool IsGradientCaptions() const;
    bool& IsGradientCaptions();
    bool IsFlatMenus() const;
    bool& IsFlatMenus();
    NONCLIENTMETRICS const& GetNcMetrcs() const;
    NONCLIENTMETRICS& GetNcMetrcs();

    static void PerformStaticInit(CPageAppearance& uiPage, CLUIApp const* pApp);

private:
    static const SizeRange g_DefaultSizeRange[SIZES_Count];

    int                       m_nIndex;
    ATL::CString               m_sName;
    bool           m_bGradientCaptions;
    bool                  m_bFlatMenus;
    NONCLIENTMETRICS       m_ncMetrics;
    WTL::CBrush           m_brReserved;
    COLORREF        m_Color[CLR_Count];
    WTL::CBrush     m_Brush[CLR_Count];
    SizeRange m_SizeRange[SIZES_Count];

    bool LoadCurrent();
    bool LoadCurrentColors();
    bool LoadCurrentNcMetrics();
    bool LoadCurrentGradientCaptionsSetting();
    bool LoadCurrentFlatMenusSetting();

    static void ThemesStaticInit(CPageAppearance& uiPage, CTheme& initialTheme);
    static void ItemStaticInit(CPageAppearance& uiPage);
    static void FontsSizeStaticInit(CPageAppearance& uiPage);
    static void FontsButtonsStaticInit(CPageAppearance& uiPage);
    static void FontsSmoothStaticInit(CPageAppearance& uiPage);
    static void FontsStaticInit(CPageAppearance& uiPage, FontMap const&);
};

inline bool&            CTheme::IsGradientCaptions() { return m_bGradientCaptions; }
inline bool&                   CTheme::IsFlatMenus() { return m_bFlatMenus; }
inline NONCLIENTMETRICS&       CTheme::GetNcMetrcs() { return m_ncMetrics; }

inline bool             CTheme::IsGradientCaptions() const { return m_bGradientCaptions; }
inline bool                    CTheme::IsFlatMenus() const { return m_bFlatMenus; }
inline NONCLIENTMETRICS const& CTheme::GetNcMetrcs() const { return m_ncMetrics; }

#endif
