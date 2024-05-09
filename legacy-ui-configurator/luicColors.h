#pragma once

#include <wcdafx.api.h>

enum EColors_Index : int
{
    CLR_Invalid = -1,
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
                                //      COLOR_BTNHILIGHT
                                //      COLOR_3DHIGHLIGHT
                                //      COLOR_3DHILIGHT
    CLR_ButtonDkShadow,         // 21 = COLOR_3DDKSHADOW
    CLR_ButtonLight,            // 22 = COLOR_3DLIGHT
    CLR_InfoText,               // 23 = COLOR_INFOTEXT
    CLR_InfoWindow,             // 24 = COLOR_INFOBK
    CLR_ButtonAlternateFace,    // 25 = COLOR_ALTERNATEBTNFACE
    // (unused, undefined by the SDK) --------------------------------
    CLR_HotTrackingColor,       // 26 = COLOR_HOTLIGHT (Hyperlink)
    CLR_GradientActiveTitle,    // 27 = COLOR_GRADIENTACTIVECAPTION
    CLR_GradientInactiveTitle,  // 28 = COLOR_GRADIENTINACTIVECAPTION
    CLR_MenuHilight,            // 29 = COLOR_MENUHILIGHT
    CLR_MenuBar,                // 30 = COLOR_MENUBAR
    CLR_Count
};

struct CColorPair
{
    DELETE_COPY_MOVE_OF(CColorPair);

    COLORREF    m_Color;
    WTL::CBrush m_Brush;
    bool        m_bCopy;

    ~CColorPair();
    CColorPair();

    bool Reset(COLORREF color);
    bool Reset(WTL::CBrush& hBrush);

    CColorPair& operator = (CColorPair& rhs) noexcept;
    void Swap(CColorPair& rhs) noexcept;
    void CopyTo(CColorPair& target) const noexcept;
};

struct CColors
{
    DELETE_COPY_MOVE_OF(CColors);

    ~CColors();
    CColors();

    static PCWSTR Title(int index);

    bool LoadDefaults();
    void Swap(CColors& rhs) noexcept;
    void CopyTo(CColors& target) const noexcept;

    CColorPair& operator[](int index);
    CColorPair const& operator[](int index) const;

private:
    CColorPair m_Pair[CLR_Count];

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};
