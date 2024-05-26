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
- WTL
- WIL
- GTEST (optional)

### Build prepare
Clone VCPKG, for example [my fork](https://github.com/wcdnail/wcpkg.git)

Win CMD:
```cmd
cd /D "c:\PATH\TO\VCPKG"
setx VCPKG_ROOT=%CD%
call bootstrap-vcpkg.bat -disableMetrics
vcpkg install wtl wil
rem optional
vcpkg install gtest
```

## Thanks to
- [VCPKG team](https://github.com/microsoft/vcpkg)
- [WTL team](https://sourceforge.net/projects/wtl/)
- [Franco Tortoriello et al.](https://gitlab.com/ftortoriello)
- [Tim Smith CColorButton](https://www.codeproject.com/Articles/2430/Color-Picker-for-WTL-with-XP-themes)
- [Matthias C. M. Troffaes mcmtroffaes inipp](https://github.com/mcmtroffaes/inipp)
- [Meacz's ChooseColor](https://github.com/meacz/ChooseColor.git)
- [David Swigger's Photoshop's-like color pickers](https://www.codeproject.com/Articles/2577/Xguiplus-A-set-of-Photoshop-s-like-color-pickers)
- [Jake Besworth's Simple Color Conversions](https://github.com/jakebesworth/Simple-Color-Conversions/blob/master/color.c)
