# WTools
Misc Windows Tools

## [WCD] Color Picker
May be buGs with HSL/HSV colors, need investigating

First Release [1.0](https://github.com/wcdnail/wtools/releases/tag/WCPKR-1.0)

![WCPKRS1](wcd.color.picker/wcd.color.picker.png)
![WCPKRS2](wcd.color.picker/wcd.color.picker.mag.png)

## Build
MSVC solution files.
CMake will be provided later.

### Build deps
- [WTL](https://sourceforge.net/projects/wtl/)
- [WIL](https://github.com/microsoft/wil)
- [CLIPP](https://github.com/wcdnail/clipp) WARNING: use this fork instead original (TCHAR support)
- [boost-config](https://github.com/boostorg/config) compiler & stdlib macros
- [GTEST](https://github.com/google/googletest) for UT

### Build prepare
Clone VCPKG;

WARNING: Use [this fork](https://github.com/wcdnail/wcpkg.git), there're CLIPP with TCHAR support

Win CMD:
```cmd
cd /D "c:\PATH\TO\VCPKG"

rem WARN: setup VCPKG_ROOT system wide
setx VCPKG_ROOT %CD%

rem setup VCPKG_ROOT env var for this cmd session
set VCPKG_ROOT=%CD%

call bootstrap-vcpkg.bat -disableMetrics
vcpkg install wtl wil clipp gtest boost-config

rem ...
rem See legacy alert
vcpkg install boost
```

### Legacy alert
There're a lot of legacy, to solve install boost, to do not care COMMENT code.

## Thanks to
- [VCPKG team](https://github.com/microsoft/vcpkg)
- [WTL team](https://sourceforge.net/projects/wtl/)
- [Franco Tortoriello et al.](https://gitlab.com/ftortoriello)
- [Tim Smith CColorButton](https://www.codeproject.com/Articles/2430/Color-Picker-for-WTL-with-XP-themes)
- [Matthias C. M. Troffaes mcmtroffaes inipp](https://github.com/mcmtroffaes/inipp)
- [Meacz's ChooseColor](https://github.com/meacz/ChooseColor.git)
- [David Swigger's Photoshop's-like color pickers](https://www.codeproject.com/Articles/2577/Xguiplus-A-set-of-Photoshop-s-like-color-pickers)
- [Jake Besworth's Simple Color Conversions](https://github.com/jakebesworth/Simple-Color-Conversions/blob/master/color.c)
- [Ken Zhang's (zhangyuwu's) OutputDebugString listener](https://www.codeproject.com/Articles/23776/Mechanism-of-OutputDebugString)
- [DebugViewPP team](https://github.com/CobaltFusion/DebugViewPP)
