#include "stdafx.h"
#include "luicColors.h"
#include "luicRegistry.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

CColorPair::~CColorPair()
{
    if (m_bCopy) {
        m_Brush.m_hBrush = nullptr;
    }
}

CColorPair::CColorPair()
    : m_Color{CLR_INVALID}
    , m_Brush{nullptr}
    , m_bCopy{false}
{
}

CColors::~CColors() = default;
CColors::CColors() = default;

PCWSTR CColors::Title(int index)
{
    static constexpr PCTSTR gs_name[CLR_Count] = {
        TEXT("Scrollbar"),              // 00 = COLOR_SCROLLBAR
        TEXT("Background"),             // 01 = COLOR_BACKGROUND
        // COLOR_DESKTOP -------------------------------------------------
        TEXT("ActiveTitle"),            // 02 = COLOR_ACTIVECAPTION
        TEXT("InactiveTitle"),          // 03 = COLOR_INACTIVECAPTION
        TEXT("Menu"),                   // 04 = COLOR_MENU
        TEXT("Window"),                 // 05 = COLOR_WINDOW
        TEXT("WindowFrame"),            // 06 = COLOR_WINDOWFRAME
        TEXT("MenuText"),               // 07 = COLOR_MENUTEXT
        TEXT("WindowText"),             // 08 = COLOR_WINDOWTEXT
        TEXT("TitleText"),              // 09 = COLOR_CAPTIONTEXT
        TEXT("ActiveBorder"),           // 10 = COLOR_ACTIVEBORDER
        TEXT("InactiveBorder"),         // 11 = COLOR_INACTIVEBORDER
        TEXT("AppWorkSpace"),           // 12 = COLOR_APPWORKSPACE
        TEXT("Hilight"),                // 13 = COLOR_HIGHLIGHT
        TEXT("HilightText"),            // 14 = COLOR_HIGHLIGHTTEXT
        TEXT("ButtonFace"),             // 15 = COLOR_BTNFACE
        // COLOR_3DFACE --------------------------------------------------
        TEXT("ButtonShadow"),           // 16 = COLOR_BTNSHADOW
        // COLOR_3DSHADOW ------------------------------------------------
        TEXT("GrayText"),               // 17 = COLOR_GRAYTEXT
        // (Disabled menu item selection) --------------------------------
        TEXT("ButtonText"),             // 18 = COLOR_BTNTEXT
        TEXT("InactiveTitleText"),      // 19 = COLOR_INACTIVECAPTIONTEXT
        TEXT("ButtonHilight"),          // 20 = COLOR_BTNHIGHLIGHT
        TEXT("ButtonDkShadow"),         // 21 = COLOR_3DDKSHADOW
        TEXT("ButtonLight"),            // 22 = COLOR_3DLIGHT
        TEXT("InfoText"),               // 23 = COLOR_INFOTEXT
        TEXT("InfoWindow"),             // 24 = COLOR_INFOBK
        TEXT("ButtonAlternateFace"),    // 25 = COLOR_ALTERNATEBTNFACE
        // (unused, undefined by the SDK) --------------------------------
        TEXT("HotTrackingColor"),       // 26 = COLOR_HOTLIGHT (Hyperlink)
        TEXT("GradientActiveTitle"),    // 27 = COLOR_GRADIENTACTIVECAPTION
        TEXT("GradientInactiveTitle"),  // 28 = COLOR_GRADIENTINACTIVECAPTION
        TEXT("MenuHilight"),            // 29 = COLOR_MENUHILIGHT
        TEXT("MenuBar")                 // 30 = COLOR_MENUBAR
    };
    if (index < 0 || index > CLR_Count - 1) {
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return L"INVALID COLOR";
    }
    return gs_name[index];
}

bool CColors::LoadDefaults()
{
    bool bReset = true;
    CColorPair pair[CLR_Count];
    for (int i = 0; i < CLR_Count; i++) {
        bReset = pair[i].Reset(static_cast<COLORREF>(GetSysColor(i)));
        if (!bReset) {
            return false;
        }
    }
    for (int i = 0; i < CLR_Count; i++) {
        m_Pair[i] = pair[i];
    }
    return true;
}

void CColorPair::Swap(CColorPair& rhs) noexcept
{
    std::swap(m_Color, rhs.m_Color);
    std::swap(m_Brush.m_hBrush, rhs.m_Brush.m_hBrush);
}

void CColors::Swap(CColors& rhs) noexcept
{
    for (int i = 0; i < CLR_Count; i++) {
        rhs[i].Swap(m_Pair[i]);
    }
}

void CColorPair::CopyTo(CColorPair& target) const noexcept
{
    if (m_bCopy && target.m_Brush.m_hBrush) {
        target.m_Brush.Attach(m_Brush.m_hBrush);
        target.m_bCopy = false;
    }
    else {
        target.m_Brush.m_hBrush = m_Brush.m_hBrush;
        target.m_bCopy = true;
    }
    target.m_Color = m_Color;
}

void CColors::CopyTo(CColors& target) const noexcept
{
    for (int i = 0; i < CLR_Count; i++) {
        m_Pair[i].CopyTo(target.m_Pair[i]);
    }
}

bool CColors::LoadValues(CRegistry const& regScheme)
{
    CColorPair pair[CLR_Count];
    for (int i = 0; i < CLR_Count; i++) {
        std::wstring name = L"Color" + std::to_wstring(i);
        DWORD dwColor = CLR_INVALID;
        if (!regScheme.GetValue<DWORD>(name, dwColor)) {
            return false;
        }
        if (!pair[i].Reset(static_cast<COLORREF>(dwColor))) {
            return false;
        }
    }
    for (int i = 0; i < CLR_Count; i++) {
        m_Pair[i] = pair[i];
    }
    return true;
}

template <typename ReturnType, typename SelfRef>
ReturnType& CColors::getRefByIndex(SelfRef& thiz, int index)
{
    if (index < 0 || index > CLR_Count - 1) {
        static ReturnType dummy{};
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return dummy;
    }
    return thiz.m_Pair[index];
}

CColorPair& CColors::operator[](int index)
{
    return getRefByIndex<CColorPair>(*this, index);
}

CColorPair const& CColors::operator[](int index) const
{
    return getRefByIndex<const CColorPair>(*this, index);
}

bool CColors::IsNotEqual(CColors const& rhs) const
{
    for (int i = 0; i < CLR_Count; i++) {
        if (m_Pair[i].m_Color != rhs.m_Pair[i].m_Color) {
            return true;
        }
    }
    return false;
}

bool CColorPair::Reset(COLORREF color)
{
    const HBRUSH hBrush = CreateSolidBrush(color);
    if (!hBrush) {
        const auto code = static_cast<HRESULT>(GetLastError());
        const auto codeText = Str::ErrorCode<wchar_t>::SystemMessage(code);
        DH::TPrintf(L"%s: ERROR: CreateSolidBrush failed: %d '%s'\n", __FUNCTIONW__, code, codeText.GetString());
        return false;
    }
    m_Color = color;
    m_Brush.Attach(hBrush);
    return true;
}

bool CColorPair::Reset(WTL::CBrush& hBrush)
{
    LOGBRUSH temp;
    if (!hBrush.GetLogBrush(&temp)) {
        const auto code = static_cast<HRESULT>(GetLastError());
        const auto codeText = Str::ErrorCode<wchar_t>::SystemMessage(code);
        DH::TPrintf(L"%s: ERROR: GetLogBrush failed: %d '%s'\n", __FUNCTIONW__, code, codeText.GetString());
        return false;
    }
    m_Color = temp.lbColor;
    if (m_bCopy) {
        m_Brush.m_hBrush = nullptr;
        m_bCopy = false;
    }
    m_Brush.Attach(hBrush.Detach());
    return true;
}

CColorPair& CColorPair::operator=(CColorPair& rhs) noexcept
{
    m_Color = rhs.m_Color;
    if (m_bCopy) {
        m_Brush.m_hBrush = nullptr;
        m_bCopy = false;
    }
    m_Brush.Attach(rhs.m_bCopy ? rhs.m_Brush.m_hBrush : rhs.m_Brush.Detach());
    return *this;
}
