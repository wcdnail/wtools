#include "stdafx.h"
#include "sensor.h"
#include "sensor.facility.h"
#include "sensor.query.thread.h"
#include <windows.gdi.rects.h>

namespace Assist
{
    namespace Sensors
    {
        Sensor::Sensor(Facility& owner, PCTSTR caption)
            : facility_(owner)
            , queryThread_(SensorStatusThread())
            , pen_(::CreatePen(PS_SOLID, 1, 0xffbf91))
            , gridPen_(::CreatePen(PS_SOLID, 1, 0x70302c))
            , borderPen_(::CreatePen(PS_SOLID, 1, 0x80403c))
            , caption_(caption)
        {
            color_[0] = 0x520601;
            color_[1] = 0xffbf11;
            color_[2] = 0xffbf11;

            queryThread_.AddSensor(this);
        }

        Sensor::~Sensor()
        {
            queryThread_.DelSensor(this);
        }
        
        void Sensor::Invalidate() const
        {
            facility_.Invalidate();
        }

        void Sensor::SetBackCol(COLORREF col)
        {
            color_[0] = col;
        }

        void Sensor::SetTextCol(COLORREF col)
        {
            color_[1] = col;
        }

        void Sensor::SetCaptionCol(COLORREF col)
        {
            color_[2] = col;
        }

        void Sensor::SetLineStyle(int style, int width, COLORREF col)
        {
            CPen pen;
            if (pen.CreatePen(style, width, col))
                pen_.Attach(pen.Detach());
        }

        void Sensor::SetGridLineStyle(int style, int width, COLORREF col)
        {
            CPen pen;
            if (pen.CreatePen(style, width, col))
                gridPen_.Attach(pen.Detach());
        }

        void Sensor::SetBorderLineStyle(int style, int width, COLORREF col)
        {
            CPen pen;
            if (pen.CreatePen(style, width, col))
                borderPen_.Attach(pen.Detach());
        }

        void Sensor::SetCaption(PCTSTR caption)
        {
            caption_ = caption;
        }

        void Sensor::DoPaint(CDCHandle dc, CRect const& rc) const
        {
        }

        HGDIOBJ Sensor::BeginPaint(WTL::CDCHandle dc, CRect const& rc) const
        {
            dc.FillSolidRect(rc, color_[0]);
            HPEN lpen = dc.SelectPen(gridPen_);

            PrePaint(dc, rc);

            dc.SelectPen(borderPen_);
            Cf::FrameRect(dc, rc);

            return (HGDIOBJ)lpen;
        }

        void Sensor::PrePaint(WTL::CDCHandle dc, CRect const& rc) const
        {
        }

        void Sensor::DoPaintText(WTL::CDCHandle dc, CRect const& rc, CString const& text) const
        {
            if (caption_.IsEmpty() && text.IsEmpty())
                return ;

            COLORREF ltextColor = dc.SetTextColor(color_[2]);
            int lmode = dc.SetBkMode(OPAQUE);
            int lbk = dc.SetBkColor(color_[0]);

            TEXTMETRIC tm;
            dc.GetTextMetrics(&tm);

            if (!caption_.IsEmpty())
            {
                CRect rcText(rc.left + 2, rc.top + 2, rc.right - 2, rc.top + tm.tmHeight + 2);
                dc.DrawText(caption_, caption_.GetLength(), rcText, DT_LEFT);
            }

            if (!text.IsEmpty())
            {
                dc.SetTextColor(color_[1]);
                CRect rcText(rc.left + 2, rc.bottom - tm.tmHeight - 2, rc.right - 2, rc.bottom - 2);
                dc.DrawText(text, text.GetLength(), rcText, DT_RIGHT);
            }

            dc.SetBkColor(lbk);
            dc.SetBkMode(lmode);
            dc.SetTextColor(ltextColor);
        }

        void Sensor::Query()
        {
        }

        //////////////////////////////////////////////////////////////////////////

        DummySensor::DummySensor(Facility& owner)
            : Sensor(owner)
        {
        }

        DummySensor::~DummySensor()
        {
        }

        void DummySensor::DoPaint(CDCHandle dc, CRect const& rc) const
        {
        }
    }
}
