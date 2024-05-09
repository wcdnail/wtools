#include "stdafx.h"
#include "luicScheme.h"
#include "luicRegistry.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

CScheme::~CScheme() = default;

CScheme::CScheme(StrView name)
    : m_Name{name}
    , m_bGradientCaptions{false}
    , m_bFlatMenus{false}
{
    for (int i = 0; i < NCM_Count; i++) {
        m_SizeRange[i] = CNCMetrics::DefaultRange(i);
    }
}

CScheme::Item const& CScheme::ItemDef(int index)
{
    // Assign the color and metric numbers to each element of the combo box
    //                              Name  Size 1               Size 2               Color 1                 Color 2                         Font            Fontcolor
    static constexpr Item gs_item[IT_Count] =
    {
    /* 0*/ {                  L"Desktop", -1,                  -1,                  COLOR_DESKTOP,          -1,                             FONT_Desktop,   -1                          },
    /* 1*/ {   L"Application Background", -1,                  -1,                  COLOR_APPWORKSPACE,     -1,                             -1,             -1                          },
    /* 2*/ {                   L"Window", -1,                  -1,                  COLOR_WINDOW,           COLOR_WINDOWFRAME,              -1,             COLOR_WINDOWTEXT            },
    /* 3*/ {                     L"Menu", NCM_MenuHeight,      NCM_MenuWidth,       COLOR_MENU,             -1,                             FONT_Menu,      COLOR_MENUTEXT              },
    /* 4*/ {       L"Title Bar - Active", NCM_CaptionHeight,   NCM_CaptionWidth,    COLOR_ACTIVECAPTION,    COLOR_GRADIENTACTIVECAPTION,    FONT_Caption,   COLOR_CAPTIONTEXT           },
    /* 5*/ {     L"Title Bar - Inactive", NCM_CaptionHeight,   NCM_CaptionWidth,    COLOR_INACTIVECAPTION,  COLOR_GRADIENTINACTIVECAPTION,  FONT_Caption,   COLOR_INACTIVECAPTIONTEXT   },
    /* 6*/ {      L"Title Bar - Palette", NCM_SMCaptionHeight, NCM_SMCaptionWidth,  -1,                     -1,                             FONT_SMCaption, -1                          },
    /* 7*/ {   L"Window Border - Active", NCM_Border,          -1,                  COLOR_ACTIVEBORDER,     -1,                             -1,             -1                          },
    /* 8*/ { L"Window Border - Inactive", NCM_Border,          -1,                  COLOR_INACTIVEBORDER,   -1,                             -1,             -1                          },
    /* 9*/ {                L"Scrollbar", NCM_ScrollWidth,     NCM_ScrollHeight,    COLOR_SCROLLBAR,        -1,                             -1,             -1                          },
    /*10*/ {                L"3D Object", -1,                  -1,                  COLOR_3DFACE,           -1,                             -1,             COLOR_BTNTEXT               },
    /*11*/ {                L"3D Shadow", -1,                  -1,                  COLOR_3DSHADOW,         COLOR_3DDKSHADOW,               -1,             -1                          },
    /*12*/ {                 L"3D Light", -1,                  -1,                  COLOR_3DHILIGHT,        COLOR_3DLIGHT,                  -1,             -1                          },
    /*13*/ {            L"Selected Item", -1,                  -1,                  COLOR_HIGHLIGHT,        -1,                             -1,             COLOR_HIGHLIGHTTEXT         },
    /*14*/ {            L"Disabled Item", -1,                  -1,                  -1,                     -1,                             -1,             COLOR_GRAYTEXT              },
    /*15*/ {                  L"ToolTip", -1,                  -1,                  COLOR_INFOBK,           -1,                             FONT_Tooltip,   COLOR_INFOTEXT              },
    /*16*/ {              L"Message Box", -1,                  -1,                  -1,                     -1,                             FONT_Message,   COLOR_WINDOWTEXT            },
    /*17*/ {                L"Hyperlink", -1,                  -1,                  -1,                     -1,                             -1,             COLOR_HOTLIGHT              },
    /*18*/ {          L"Menu Bar (Flat)", -1,                  -1,                  COLOR_MENUBAR,          COLOR_MENUHILIGHT,              -1,             -1                          },
    /*19*/ {     L"Window Padded Border", NCM_PaddedBorder,    -1,                  -1,                     -1,                             -1,             -1                          },
    };
    if (index < 0 || index >= IT_Count) {
        static constexpr Item dummy{L"INVALID ITEM", -1, -1, -1, -1, -1, -1};
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return dummy;
    }
    return gs_item[index];
}

bool CScheme::LoadDefaults()
{
    BOOL  bGradientCaptions{FALSE};
    BOOL         bFlatMenus{FALSE};
    CColors       tmpColors{};
    CNCMetrics tmpNCMetrics{};
    CFonts         tmpFonts{};
    if (!tmpColors.LoadDefaults()) {
        DH::TPrintf(L"%s: ERROR: CColors::LoadDefaults failed\n", __FUNCTIONW__);
        return false;
    }
    if (!tmpNCMetrics.LoadDefaults()) {
        DH::TPrintf(L"%s: ERROR: CNCMetrics::LoadDefaults failed\n", __FUNCTIONW__);
        return false;
    }
    if (!tmpFonts.LoadDefaults()) {
        DH::TPrintf(L"%s: ERROR: CFonts::LoadDefaults failed\n", __FUNCTIONW__);
        return false;
    }
    if (!tmpFonts.LoadValues(tmpNCMetrics)) {
        DH::TPrintf(L"%s: ERROR: CFonts::LoadDefaults for CNCMetrcs failed\n", __FUNCTIONW__);
        return false;
    }
    if (!SystemParametersInfoW(SPI_GETGRADIENTCAPTIONS, 0, &bGradientCaptions, 0)) {
        DH::TPrintf(L"%s: ERROR: SystemParametersInfoW failed\n", __FUNCTIONW__);
        return false;
    }
    if (!SystemParametersInfoW(SPI_GETFLATMENU, 0, &bFlatMenus, 0)) {
        DH::TPrintf(L"%s: ERROR: SystemParametersInfoW failed\n", __FUNCTIONW__);
        return false;
    }
    tmpFonts.Swap(m_Font);
    tmpNCMetrics.CopyTo(m_NCMetric);
    tmpColors.Swap(m_Color);
    m_bFlatMenus = bFlatMenus != FALSE;
    m_bGradientCaptions = bGradientCaptions != FALSE;
    DH::TPrintf(L"%s: OK\n", __FUNCTIONW__);
    return true;
}

void CScheme::CopyTo(CScheme& target) const
{
    m_Color.CopyTo(target.m_Color);
    m_NCMetric.CopyTo(target.m_NCMetric);
    m_Font.CopyTo(target.m_Font);
    target.m_bFlatMenus = m_bFlatMenus;
    target.m_bGradientCaptions = m_bGradientCaptions;
    target.m_Name = m_Name; // NO noecept!
}

bool CScheme::LoadValues(CRegistry const& regScheme)
{
    DWORD  bGradients{FALSE};
    DWORD  bFlatMenus{FALSE};
    CColors tmpColors{};
    if (!tmpColors.LoadValues(regScheme)) {
        DH::TPrintf(L"%s: ERROR: CColors::LoadValues failed\n", __FUNCTIONW__);
        return false;
    }
    tmpColors.Swap(m_Color);
    if (regScheme.GetValue<DWORD>(L"Gradients", bGradients)) {
        m_bFlatMenus = bFlatMenus != FALSE;
    }
    if (regScheme.GetValue<DWORD>(L"FlatMenus", bFlatMenus)) {
        m_bGradientCaptions = bGradients != FALSE;
    }
    return true;
}

bool CScheme::LoadSizes(StrView sName, CRegistry const& regScheme)
{
    CNCMetrics tmpNCMetrics{};
    CFonts         tmpFonts{};
    if (!tmpNCMetrics.LoadValues(regScheme)) {
        DH::TPrintf(L"%s: ERROR: CNCMetrics::LoadValues failed\n", __FUNCTIONW__);
        return false;
    }
    if (!tmpFonts.LoadDefaults()) {
        DH::TPrintf(L"%s: ERROR: CFonts::LoadDefaults failed\n", __FUNCTIONW__);
        return false;
    }
    if (!tmpFonts.LoadValues(tmpNCMetrics)) {
        DH::TPrintf(L"%s: ERROR: CFonts::LoadDefaults for CNCMetrcs failed\n", __FUNCTIONW__);
        return false;
    }
    tmpFonts.Swap(m_Font);
    tmpNCMetrics.CopyTo(m_NCMetric);
    return true;
}
