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
    HDC screenDc{GetDC(nullptr)};
    if (screenDc) {
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
 