#pragma once

//
// https://github.com/dracula/dracula-theme
//
static constexpr COLORREF CLR_Background  = RGB( 40,  42,  54); // #282a36
static constexpr COLORREF CLR_CurrentLine = RGB( 68,  71,  90); // #44475a
static constexpr COLORREF CLR_Selection   = RGB( 68,  71,  90); // #44475a
static constexpr COLORREF CLR_Foreground  = RGB(248, 248, 242); // #f8f8f2
static constexpr COLORREF CLR_Comment     = RGB( 98, 114, 164); // #6272a4
static constexpr COLORREF CLR_Cyan        = RGB(139, 233, 253); // #8be9fd
static constexpr COLORREF CLR_Green       = RGB( 80, 250, 123); // #50fa7b
static constexpr COLORREF CLR_Orange      = RGB(255, 184, 108); // #ffb86c
static constexpr COLORREF CLR_Pink        = RGB(255, 121, 198); // #ff79c6
static constexpr COLORREF CLR_Purple      = RGB(189, 147, 249); // #bd93f9
static constexpr COLORREF CLR_Red         = RGB(255,  85,  85); // #ff5555
static constexpr COLORREF CLR_Yellow      = RGB(241, 250, 140); // #f1fa8c

using     UIColor = winrt::Windows::UI::Color;
using UIColorHlpr = winrt::Windows::UI::ColorHelper;

static const UIColor UI_CLR_Background  = UIColorHlpr::FromArgb(255,  40,  42,  54); // #282a36
static const UIColor UI_CLR_CurrentLine = UIColorHlpr::FromArgb(255,  68,  71,  90); // #44475a
static const UIColor UI_CLR_Selection   = UIColorHlpr::FromArgb(255,  68,  71,  90); // #44475a
static const UIColor UI_CLR_Foreground  = UIColorHlpr::FromArgb(255, 248, 248, 242); // #f8f8f2
static const UIColor UI_CLR_Comment     = UIColorHlpr::FromArgb(255,  98, 114, 164); // #6272a4
static const UIColor UI_CLR_Cyan        = UIColorHlpr::FromArgb(255, 139, 233, 253); // #8be9fd
static const UIColor UI_CLR_Green       = UIColorHlpr::FromArgb(255,  80, 250, 123); // #50fa7b
static const UIColor UI_CLR_Orange      = UIColorHlpr::FromArgb(255, 255, 184, 108); // #ffb86c
static const UIColor UI_CLR_Pink        = UIColorHlpr::FromArgb(255, 255, 121, 198); // #ff79c6
static const UIColor UI_CLR_Purple      = UIColorHlpr::FromArgb(255, 189, 147, 249); // #bd93f9
static const UIColor UI_CLR_Red         = UIColorHlpr::FromArgb(255, 255,  85,  85); // #ff5555
static const UIColor UI_CLR_Yellow      = UIColorHlpr::FromArgb(255, 241, 250, 140); // #f1fa8c
