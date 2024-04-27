#pragma once

#include "dh.tracing.h"
#include "wtl.colorizer.h"
#include <atltypes.h>
#include <atlgdi.h>

namespace Cf
{
    struct Colorizer::PaintContext
    {
        PaintContext(HWND hwnd, HFONT font)
            : PaindDC(hwnd)
            , Rect()
            , PrevFont(PaindDC.SelectFont(font))
        {
            ::GetClientRect(hwnd, Rect);
        }

        ~PaintContext()
        {
            PaindDC.SelectFont(PrevFont);
        }

        WTL::CPaintDC PaindDC;
        CRect Rect;

    private:
        HFONT PrevFont;
    };

    struct Colorizer::NcPainContext
    {
        NcPainContext(HWND hwnd, CRgnHandle rgn)
            : Dc(::GetWindowDC(hwnd))
            , Rect()
            , Rgn(rgn)
            , ControlHandle(hwnd)
        {
            CRect wrc;
            ::GetWindowRect(hwnd, wrc);

            if (!Rgn || (1 == (WPARAM)Rgn)) 
            {
                Rect = wrc;
                Rect.right -= Rect.left + 1;
                Rect.bottom -= Rect.top + 1;
                Rect.left = Rect.top = 0;
            } 
            else 
                ::GetRgnBox(Rgn, Rect);
        }

        ~NcPainContext()
        {
            ::ReleaseDC(ControlHandle, Dc);
        }

        WTL::CDCHandle Dc;
        CRect Rect;
        HRGN Rgn;

    private:
        HWND ControlHandle;
    };
}
