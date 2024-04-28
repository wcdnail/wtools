#include "stdafx.h"
#include "wtl.colorizer.helpers.h"

namespace CF::Colorized
{
    PaintContext::~PaintContext()
    {
        PaindDC.SelectFont(PrevFont);
    }

    PaintContext::PaintContext(HWND hwnd, HFONT font)
        : PaindDC(hwnd)
        , Rect()
        , PrevFont(PaindDC.SelectFont(font))
    {
        ::GetClientRect(hwnd, Rect);
    }

    NcPainContext::~NcPainContext()
    {
        ::ReleaseDC(hCtrl, Dc);
    }

    NcPainContext::NcPainContext(HWND hwnd, CRgnHandle rgn)
        : Dc(::GetWindowDC(hwnd))
        , Rect()
        , Rgn(rgn)
        , hCtrl(hwnd)

    {
        CRect wrc;
        ::GetWindowRect(hwnd, wrc);

        if (!Rgn || (1 == reinterpret_cast<WPARAM>(Rgn))) {
            Rect = wrc;
            Rect.right -= Rect.left + 1;
            Rect.bottom -= Rect.top + 1;
            Rect.left = Rect.top = 0;
        } 
        else {
            ::GetRgnBox(Rgn, Rect);
        }
    }
}
