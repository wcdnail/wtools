#pragma once

#include "luicNCMetric.h"
#include "luicColors.h"
#include "luicFonts.h"
#include <wcdafx.api.h>
#include <memory>

enum EItemIndex : int
{
    IT_Invalid = -1,
    IT_Desktop = 0,
    IT_AppBackground,
    IT_Window,
    IT_Menu,             // The width affects the icon and caption buttons of MDI children
    IT_ActiveCaption,    // Title bar of active windows
    IT_InactiveCaption,  // Title bar of inactive windows
    IT_SMCaption,        // Title bar of palette (small) windows
    IT_ActiveBorder,     // Border of active resizable windows
    IT_InactiveBorder,   // Border of inactive resizable windows
    IT_Scrollbar,
    IT_3DObject,
    IT_3DShadow,         // Not in official applets
    IT_3DLight,          // Not in official applets
    IT_SelectedItem,     // Also used for text selection
    IT_DisabledItem,
    IT_Tooltip,
    IT_MsgBox,
    IT_Hyperlink,
    IT_FlatmenuBar,
    IT_PaddedBorder,    // Border of windows, including property sheets for some reason.
    IT_Count            // If SIZE_PaddedBorder is > 0, the windows with fixed borders affected also
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

struct CScheme;

using CSchemePtr = std::shared_ptr<CScheme>;

struct CScheme
{
    DELETE_COPY_MOVE_OF(CScheme);

    using String = std::wstring;

    struct Item
    {
        PCWSTR   name;
        int     size1;
        int     size2;
        int    color1;
        int    color2;
        int      font;
        int fontColor;
    };

    String                            m_Name;
    CColors                          m_Color;
    CNCMetrics                    m_NCMetric;
    CFonts                            m_Font;
    bool                 m_bGradientCaptions;
    bool                        m_bFlatMenus;
    CNCMetrics::Range m_SizeRange[NCM_Count];

    ~CScheme();
    CScheme(String const& name);

    static Item const& ItemDef(int index);

    bool LoadDefaults();
    void CopyTo(CScheme& target) const;

    COLORREF               GetColor(int index) const { return m_Color[index].m_Color; }
    WTL::CBrush const&     GetBrush(int index) const { return m_Color[index].m_Brush; }
    CNCMetrics const&           GetNCMetrics() const { return m_NCMetric; }
    CNCMetrics&                 GetNCMetrics()       { return m_NCMetric; }
    WTL::CLogFont const& GetLogFont(int index) const { return m_Font[index].m_logFont; }
    WTL::CFontHandle        GetFont(int index) const { return m_Font[index].m_CFont.m_hFont; }
};
