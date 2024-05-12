#include "stdafx.h"
#include "luicScheme.h"
#include "luicRegistry.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

CSizePair::~CSizePair() = default;
CSizePair::CSizePair() = default;

CScheme::~CScheme() = default;

CScheme::CScheme(StrView name)
    :              m_Name{name}
    , m_bGradientCaptions{false}
    ,        m_bFlatMenus{false}
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

template <typename ReturnType, typename SelfRef>
ReturnType& CScheme::getSizeRangeRef(SelfRef& thiz, int index)
{
    if (index < 0 || index > NCM_Count - 1) {
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        static ReturnType dummy{NCM_Invalid};
        return dummy;
    }
    return thiz.m_SizeRange[index];
}

CNCMetrics::Range const& CScheme::GetSizeRange(int index) const
{
    return getSizeRangeRef<CNCMetrics::Range const>(*this, index);
}

CNCMetrics::Range& CScheme::GetSizeRange(int index)
{
    return getSizeRangeRef<CNCMetrics::Range>(*this, index);
}

template <typename ReturnType, typename SelfRef>
ReturnType& CScheme::getSizeItemeRef(SelfRef& thiz, String const& name)
{
    const auto it = thiz.m_SizesMap.find(name);
    if (it == thiz.m_SizesMap.cend()) {
        DH::TPrintf(L"%s: ERROR: index [%s] out of range\n", __FUNCTIONW__, name.c_str());
        static CSizePair dummy{};
        return dummy;
    }
    return it->second;
}

CSizePair const& CScheme::GetSizePair(String const& name) const
{
    return getSizeItemeRef<CSizePair const>(*this, name);
}

CSizePair& CScheme::GetSizePair(String const& name)
{
    return getSizeItemeRef<CSizePair>(*this, name);
}

bool CSizePair::IsNotEqual(CSizePair const& rhs) const
{
    return m_NCMetric.IsNotEqual(rhs.m_NCMetric)
        || m_Font.IsNotEqual(rhs.m_Font)
        ;
}

bool CScheme::IsNotEqual(CScheme const& rhs) const
{
    return (m_Name != rhs.m_Name)
        || (m_bGradientCaptions != rhs.m_bGradientCaptions)
        || (m_bFlatMenus != rhs.m_bFlatMenus)
        || m_Color.IsNotEqual(rhs.m_Color)
        || IsSizesNotEqual(m_SizesMap, rhs.m_SizesMap)
        || IsSizeRangesNotEqual(m_SizeRange, rhs.m_SizeRange)
        ;
}

bool CScheme::IsSizesNotEqual(SizeMap const& lhs, SizeMap const& rhs)
{
    if (lhs.size() != rhs.size()) {
        return true;
    }
    for (const auto& it: lhs) {
        const auto& jt = rhs.find(it.first);
        if (jt == rhs.cend()) {
            return true;
        }
        if (it.second.IsNotEqual(jt->second)) {
            return true;
        }
    }
    return false;
}

bool CScheme::IsSizeRangesNotEqual(CNCMetrics::Range const (&lhs)[NCM_Count], CNCMetrics::Range const (&rhs)[NCM_Count])
{
    for (int i = 0; i < NCM_Count; i++) {
        if ((lhs[i].min != rhs[i].min)
         || (lhs[i].max != rhs[i].max)) {
            return true;
        }
    }
    return false;
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
    {
        SizeMap sizesMap{};
        auto&      sizes{sizesMap[L"(Current)"]};
        tmpFonts.Swap(sizes.m_Font);
        tmpNCMetrics.CopyTo(sizes.m_NCMetric);
        sizesMap.swap(m_SizesMap);
    }
    tmpColors.Swap(m_Color);
    m_bFlatMenus = bFlatMenus != FALSE;
    m_bGradientCaptions = bGradientCaptions != FALSE;
    DH::TPrintf(L"%s: OK\n", __FUNCTIONW__);
    return true;
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
    if (!tmpFonts.LoadValues(regScheme)) {
        DH::TPrintf(L"%s: ERROR: CFonts::LoadDefaults failed\n", __FUNCTIONW__);
        return false;
    }
    if (!tmpFonts.LoadValues(tmpNCMetrics)) {
        DH::TPrintf(L"%s: ERROR: CFonts::LoadDefaults for CNCMetrcs failed\n", __FUNCTIONW__);
        return false;
    }
    auto& sizes{m_SizesMap[{sName.data(), sName.length()}]};
    tmpFonts.Swap(sizes.m_Font);
    tmpNCMetrics.CopyTo(sizes.m_NCMetric);
    return true;
}

void CScheme::CopyTo(CScheme& target) const
{
    SizeMap sizesCopy{};
    for (auto const& it: m_SizesMap) {
        auto& targetSizes{sizesCopy[it.first]};
        it.second.m_NCMetric.CopyTo(targetSizes.m_NCMetric);
        it.second.m_Font.CopyTo(targetSizes.m_Font);
    }
    sizesCopy.swap(target.m_SizesMap);
    m_Color.CopyTo(target.m_Color);
    target.m_bFlatMenus = m_bFlatMenus;
    target.m_bGradientCaptions = m_bGradientCaptions;
    target.m_Name = m_Name; // NO noecept!
    for (int i = 0; i < NCM_Count; i++) {
        target.m_SizeRange[i] = m_SizeRange[i];
    }
}
