#pragma once

#include "dh.tracing.h"
#include "wtl.colorizer.h"
#include <atltypes.h>
#include <atlgdi.h>

namespace Cf
{
    struct Colorizer::PaintContext
    {
        ~PaintContext();
        PaintContext(HWND hwnd, HFONT font);

        WTL::CPaintDC PaindDC;
        CRect            Rect;
        HFONT        PrevFont;
    };

    struct Colorizer::NcPainContext
    {
        ~NcPainContext();
        NcPainContext(HWND hwnd, CRgnHandle rgn);

        WTL::CDCHandle Dc;
        CRect        Rect;
        HRGN          Rgn;
        HWND        hCtrl;
    };
}
