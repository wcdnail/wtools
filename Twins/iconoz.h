#pragma once

#include <windef.h>
#include <atlgdi.h>
#include <atluser.h>

namespace Twins
{
    struct MyIcons
    {
        CIcon Main;
        CIcon UpperDir;
        CIcon RedFloppy;
        CIcon LinkOverlay;
        CIcon Confirm;
        CIcon Cancel;
        CIcon HeaderAsc;
        CIcon HeaderDesc;

        MyIcons(HINSTANCE resModule);
    };

    extern const MyIcons Icons;
}
