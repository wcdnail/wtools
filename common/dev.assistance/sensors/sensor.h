#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <atlgdi.h>
#include <atltypes.h>
#include <atlstr.h>

namespace Assist
{
    namespace Sensors
    {
        class Facility;
            
        class Sensor: boost::noncopyable
        {
        public:
            class StatusThread;

            void SetBackCol(COLORREF col);
            void SetTextCol(COLORREF col);
            void SetCaptionCol(COLORREF col);
            void SetLineStyle(int style, int width, COLORREF col);
            void SetGridLineStyle(int style, int width, COLORREF col);
            void SetBorderLineStyle(int style, int width, COLORREF col);
            void SetCaption(PCTSTR caption);

        protected:
            friend Facility;
            friend StatusThread;

            Facility& facility_;
            StatusThread& queryThread_;
            CPen pen_;              // line style
            CPen gridPen_;          // grid style
            CPen borderPen_;        // border style
            COLORREF color_[3];     // 0 - background, 1 - text, 2 - caption
            CString caption_;

            Sensor(Facility& owner, PCTSTR caption = _T("Sensor #"));
            virtual ~Sensor() = 0;

            void Invalidate() const;
            virtual void DoPaint(WTL::CDCHandle dc, CRect const& rc) const = 0;
            virtual HGDIOBJ BeginPaint(WTL::CDCHandle dc, CRect const& rc) const;
            virtual void PrePaint(WTL::CDCHandle dc, CRect const& rc) const;
            virtual void DoPaintText(WTL::CDCHandle dc, CRect const& rc, CString const& text) const;

            virtual void Query() = 0;
        };

        typedef boost::shared_ptr<Sensor> SensorPtr;

        //////////////////////////////////////////////////////////////////////////

        class DummySensor: public Sensor
        {
        public:
            virtual ~DummySensor();

        private:
            friend Facility;
            DummySensor(Facility& owner);
            virtual void DoPaint(WTL::CDCHandle dc, CRect const& rc) const;
        };
    }
}
