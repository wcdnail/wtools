#pragma once

#include "wtl.colorizer.h"
#include <atltypes.h>
#include <atlgdi.h>

namespace CF::Colorized
{
    struct PaintContext
    {
        ~PaintContext();
        PaintContext(HWND hwnd, HFONT font);

        WTL::CPaintDC PaindDC;
        CRect            Rect;
        HFONT        PrevFont;
    };

    struct NcPainContext
    {
        ~NcPainContext();
        NcPainContext(HWND hwnd, CRgnHandle rgn);

        WTL::CDCHandle Dc;
        CRect        Rect;
        HRGN          Rgn;
        HWND        hCtrl;
    };
}
