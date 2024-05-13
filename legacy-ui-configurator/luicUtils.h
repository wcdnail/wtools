#pragma once

#include <winuser.h>
#include <atlstr.h>
#include <string>

static constexpr long DEFAULT_FONT_DPI = 72;

struct ScopedBoolGuard
{
    ScopedBoolGuard(bool& target)
        : target_{target}
        , initial_{target}
    {
        target_ = !initial_;
    }
    ~ScopedBoolGuard()
    {
        target_ = initial_;
    }
    bool& target_;
    bool initial_;
};

template <typename Res>
static inline Res GetCurrentDPI()
{
    int temp = USER_DEFAULT_SCREEN_DPI;
    if (const HDC screenDc{GetDC(nullptr)}) {
        temp = GetDeviceCaps(screenDc, LOGPIXELSY);
        ReleaseDC(nullptr, screenDc);
    }
    return static_cast<Res>(temp);
}

inline int g_DPI()
{
    static int gs_DPI = GetCurrentDPI<int>();
    return gs_DPI;
}

template <typename Res>
inline Res ScaleForDpi(Res n)
{
    return static_cast<Res>(MulDiv(static_cast<int>(n), g_DPI(), USER_DEFAULT_SCREEN_DPI));
}

template <typename Res>
inline Res FontLogToPt(Res n)
{
    return -static_cast<Res>(MulDiv(static_cast<int>(n), DEFAULT_FONT_DPI, g_DPI()));
}

template <typename Res>
inline Res FontPtToLog(Res n)
{
    return -static_cast<Res>(MulDiv(static_cast<int>(n), g_DPI(), DEFAULT_FONT_DPI));
}

enum MFStatus: int
{
    STA_Info = 0,
    STA_Warning,
    STA_Error,
};

void SetMFStatus(int status, PCWSTR format, ...);

void ReportError(ATL::CStringA&& caption, HRESULT code, bool showMBox = false, UINT mbType = MB_ICONERROR);
void ReportError(ATL::CStringW&& caption, HRESULT code, bool showMBox = false, UINT mbType = MB_ICONERROR);

struct StringHash
{
    using      HashType = std::hash<std::wstring_view>;
    using IsTransparent = void;
 
    std::size_t operator()(const wchar_t* str) const      { return HashType{}(str); }
    std::size_t operator()(std::wstring_view str) const   { return HashType{}(str); }
    std::size_t operator()(std::wstring const& str) const { return HashType{}(str); }
};

template <typename CharType>
inline std::basic_string<CharType>& StrUnquote(std::basic_string<CharType>& str)
{
    if (str.length() > 1) {
        if (str.front() == static_cast<CharType>('"') && str.back() == static_cast<CharType>('"')) {
            if (str.length() == 2) {
                str.erase();
            } else {
                str.erase(str.begin());
                str.erase(str.end() - 1);
            }
        }
    }
    return str;
}

HRESULT IFileDialog_GetDisplayName(IFileDialog& dlgImpl, std::wstring& target);

bool CBGetCurText(WTL::CComboBox const& ctlCombo, ATL::CString& result);
bool CBGetCurData(WTL::CComboBox const& ctlCombo, int& result);
bool CBGetCurTextInt(WTL::CComboBox const& ctlCombo, int& result);
