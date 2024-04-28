#include "stdafx.h"
#include "iconoz.h"
#include "res/resource.h"

namespace Twins
{
    MyIcons::MyIcons(HINSTANCE resModule)
        : Main          (::LoadIcon(resModule, MAKEINTRESOURCE(IDR_MAINFRAME)))
        , UpperDir      (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_UPPERDIR)))
        , RedFloppy     (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_REDFLOPPY)))
        , LinkOverlay   (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_LINKOVERLAY)))
        , Confirm       (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_CONFIRM)))
        , Cancel        (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_CANCEL)))
        , HeaderAsc     (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_HEADER_UP)))
        , HeaderDesc    (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_HEADER_DOWN)))
    {}

    const MyIcons Icons(::GetModuleHandle(NULL));
}
