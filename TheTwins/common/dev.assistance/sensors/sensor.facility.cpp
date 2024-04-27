#include "stdafx.h"
#include "sensor.facility.h"
#include <atlwin.h>
#include <atlctrls.h>
#include <windows.gdi.rects.h>
#include <windows.wtl.buffered.paint.h>

namespace Assist
{
    namespace Sensors
    {
        class Facility::Container: public CWindowImpl<Facility::Container, WTL::CStatic>
                                 , Cf::DoubleBuffered
        {
        public:
            Container(Facility& facility)
                : Cf::DoubleBuffered(facility.backColor_, true)
                , facilityRef_(facility)
            {
            }

            ~Container()
            {
            }

        private:
            Facility& facilityRef_;

            BEGIN_MSG_MAP_EX(Container)
                MSG_WM_PAINT(OnPaint)
                CHAIN_MSG_MAP(Cf::DoubleBuffered)
            END_MSG_MAP()

            void OnPaint(CDCHandle dcSender)
            {
                WTL::CPaintDC paintDc(m_hWnd);
                Cf::BufferedPaint bufferedPaint(paintDc, GetSecondDc(), IsDoubleBuffered(), m_hWnd);
                CDC& curDc = bufferedPaint.GetCurrentDc();

                CRect rc;
                GetClientRect(rc);
                facilityRef_.DoPaint(curDc.m_hDC, rc);
            }

        private:
            Container(Container const&);
            Container& operator = (Container const&);
        };

        Facility::Facility() /* throw(std::bad_alloc) */
            : containerPtr_(new Container(*this))
            , sensors_()
            , backColor_(::GetSysColor(COLOR_MENU))
            , font_(::CreateFont(-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Small")))
        {
        }

        Facility::~Facility()
        {
        }

        bool Facility::Prepare(HWND ownerWindow, CRect const& rc, UINT controlId)
        {
            CRect rcContainer = rc;
            HWND hsc = containerPtr_->Create(ownerWindow, rcContainer, _T("Here must be sensors...")
                , WS_CHILD | WS_VISIBLE 
                | WS_CLIPCHILDREN | WS_CLIPSIBLINGS 
                //| SS_SUNKEN
                , 0, controlId);

            if (hsc)
            {
                ::SetWindowText(hsc, _T(""));
            }

            return NULL != hsc;
        }

        void Facility::Invalidate()
        {
            ::InvalidateRect(containerPtr_->m_hWnd, NULL, FALSE);
        }

        enum
        {
            MaxColumnCount = 5,
        };

        void Facility::DoPaint(CDCHandle dc, CRect const& rc) const
        {
            HFONT lfont = dc.SelectFont(font_);
            dc.FillSolidRect(rc, backColor_);
            dc.SetBkMode(TRANSPARENT);

            int count = (int)sensors_.size();
            if (!count)
                return ;

            const int columns = (count < MaxColumnCount ? count : MaxColumnCount);
            const int rows = (count > MaxColumnCount ? (count/MaxColumnCount) + 1 : 1);

            int iw = rc.Width() / columns;
            int ih = rc.Height() / rows;

            CRect rcSensorPlace(rc.left, rc.top, rc.left + iw, rc.top + ih);

            int column = 1;
            for (SensorVector::const_iterator it = sensors_.begin(); it != sensors_.end(); ++it)
            {
                CRect rcSensor = rcSensorPlace;
                rcSensor.DeflateRect(2, 2);
                (*it)->DoPaint(dc, rcSensor);

                ++column;
                rcSensorPlace.left = rcSensorPlace.right;
                if (column > columns)
                {
                    column = 1;
                    rcSensorPlace.left = rc.left;
                    rcSensorPlace.top = rcSensorPlace.bottom;
                    rcSensorPlace.bottom = rcSensorPlace.top + ih;
                }
                rcSensorPlace.right = rcSensorPlace.left + iw;
            }

            dc.SelectFont(lfont);
        }
    }
}
