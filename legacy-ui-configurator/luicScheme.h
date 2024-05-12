#pragma once

#include "luicNCMetric.h"
#include "luicColors.h"
#include "luicFonts.h"
#include <wcdafx.api.h>
#include <string_view>
#include <string>
#include <memory>
#include <unordered_map>

struct CRegistry;
struct CScheme;

using CSchemePtr = std::shared_ptr<CScheme>;

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

struct CSizePair
{
    DELETE_COPY_MOVE_OF(CSizePair);

    CNCMetrics m_NCMetric;
    CFonts         m_Font;

    ~CSizePair();
    CSizePair();

    WTL::CLogFont const& GetLogFont(int index) const { return m_Font[index].m_logFont; }
    WTL::CFontHandle        GetFont(int index) const { return m_Font[index].m_CFont.m_hFont; }

    bool IsNotEqual(CSizePair const& rhs) const;
};

struct CScheme
{
    DELETE_COPY_MOVE_OF(CScheme);

    using StrView = std::wstring_view;
    using  String = std::wstring;
    using SizeMap = std::unordered_map<String, CSizePair>;

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

    ~CScheme();
    CScheme(StrView name);

    static Item const& ItemDef(int index);

    bool LoadDefaults();
    void CopyTo(CScheme& target) const;
    bool LoadValues(CRegistry const& regScheme);
    bool LoadSizes(StrView sName, CRegistry const& regScheme);

    String const&                                           Name() const { return m_Name; }
    bool                                      IsGradientCaptions() const { return m_bGradientCaptions; }
    bool                                             IsFlatMenus() const { return m_bFlatMenus; }
    COLORREF                                   GetColor(int index) const { return m_Color[index].m_Color; }
    WTL::CBrush const&                         GetBrush(int index) const { return m_Color[index].m_Brush; }
    CColorPair const&                      GetColorPair(int index) const { return m_Color[index]; }
    CColorPair&                            GetColorPair(int index)       { return m_Color[index]; }
    int                 GetNCMetric(String const& name, int index) const { return GetSizePair(name).m_NCMetric[index]; }
    int&                GetNCMetric(String const& name, int index)       { return GetSizePair(name).m_NCMetric[index]; }
    WTL::CLogFont const& GetLogFont(String const& name, int index) const { return GetSizePair(name).m_Font[index].m_logFont; }
    WTL::CFontHandle        GetFont(String const& name, int index) const { return GetSizePair(name).m_Font[index].m_CFont.m_hFont; }
    CFontPair const&    GetFontPair(String const& name, int index) const { return GetSizePair(name).m_Font[index]; }
    CFontPair&          GetFontPair(String const& name, int index)       { return GetSizePair(name).m_Font[index]; }
    SizeMap const&                                   GetSizesMap() const { return m_SizesMap; };
    SizeMap&                                         GetSizesMap()       { return m_SizesMap; };

    CNCMetrics::Range const& GetSizeRange(int index) const;
    CNCMetrics::Range& GetSizeRange(int index);
    CSizePair const& GetSizePair(String const& name) const;
    CSizePair& GetSizePair(String const& name);

    void SetGradientCaptions(bool bValue) { m_bGradientCaptions = bValue; }
    void        SetFlatMenus(bool bValue) { m_bFlatMenus = bValue; }

    bool IsNotEqual(CScheme const& rhs) const;

private:
    String                            m_Name;
    bool                 m_bGradientCaptions;
    bool                        m_bFlatMenus;
    CColors                          m_Color;
    SizeMap                       m_SizesMap;
    CNCMetrics::Range m_SizeRange[NCM_Count];

    static bool IsSizesNotEqual(SizeMap const& lhs, SizeMap const& rhs);
    static bool IsSizeRangesNotEqual(CNCMetrics::Range const (&lhs)[NCM_Count], CNCMetrics::Range const (&rhs)[NCM_Count]);

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getSizeRangeRef(SelfRef& thiz, int index);

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getSizeItemeRef(SelfRef& thiz, String const& name);
};
