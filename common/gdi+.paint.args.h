#pragma once

#include "windows.uses.gdi+.h"
#include <atlgdi.h>
#include <atltypes.h>
#include <boost/noncopyable.hpp>

namespace Gdi
{
    struct PaintArgs: boost::noncopyable
    {
        CDC& dc;
        Gdiplus::Graphics& gr;
        CRect const& rc;
        Gdiplus::RectF const& bbox;
        Gdiplus::Font const& font;
        Gdiplus::StringFormat const& format;
        Gdiplus::Brush const& brush;

        PaintArgs(CDC& dcRef
                , Gdiplus::Graphics& grRef
                , CRect const& rcRef
                , Gdiplus::RectF const& bboxRef
                , Gdiplus::Font const& fontRef
                , Gdiplus::StringFormat const& formatRef
                , Gdiplus::Brush const& brushRef
                );

        ~PaintArgs();
    };

    inline PaintArgs::PaintArgs(CDC& dcRef
                              , Gdiplus::Graphics& grRef
                              , CRect const& rcRef
                              , Gdiplus::RectF const& bboxRef
                              , Gdiplus::Font const& fontRef
                              , Gdiplus::StringFormat const& formatRef
                              , Gdiplus::Brush const& brushRef
                              )
        : dc(dcRef)
        , gr(grRef)
        , rc(rcRef)
        , bbox(bboxRef)
        , font(fontRef)
        , format(formatRef)
        , brush(brushRef)
    {}

    inline PaintArgs::~PaintArgs()
    {}
}
