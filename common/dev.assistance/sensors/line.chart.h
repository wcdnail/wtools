#pragma once

#include "sensor.h"
#include "float.grid.h"
#include <windows.gdi.rects.h>
#include <atltypes.h>
#include <atlstr.h>
#include <atlgdi.h>

namespace Assist
{
    namespace Sensors
    {
        class Facility;

        template <class StorageType>
        class LineChart: public Sensor
        {
        public:
        protected:
            typedef typename StorageType::value_type VT;

            LineChart(Facility& facility, PCTSTR name);
            virtual ~LineChart();

            void SetUpperLimit(VT lim);
            VT GetUpperLimit() const;

            StorageType& GetValues();
            StorageType const& GetValues() const;

            boost::mutex& GetValuesMutex();
            boost::mutex const& GetValuesMutex() const;

        private:
            VT upLimit_;
            StorageType storage_;
            mutable boost::mutex storageMx_;
            FloatGrid floatGrid_;

            virtual void PrePaint(WTL::CDCHandle dc, CRect const& rc) const;
            virtual void DoPaint(WTL::CDCHandle dc, CRect const& rc) const;
            virtual CString FormatValue(VT displayValue) const;
        };

        template <class StorageType>
        inline LineChart<StorageType>::LineChart(Facility& facility, PCTSTR name)
            : Sensor(facility, name)
            , upLimit_(0)
            , storage_()
            , storageMx_()
            , floatGrid_(12, 12, 4, 0)
        {
        }

        template <class StorageType>
        inline LineChart<StorageType>::~LineChart()
        {
        }

        template <class StorageType>
        inline void LineChart<StorageType>::SetUpperLimit(typename VT lim)
        {
            upLimit_ = lim;
        }

        template <class StorageType>
        inline typename LineChart<StorageType>::VT LineChart<StorageType>::GetUpperLimit() const
        {
            return upLimit_;
        }

        template <class StorageType>
        inline typename StorageType& LineChart<StorageType>::GetValues()
        {
            return storage_;
        }

        template <class StorageType>
        inline typename StorageType const& LineChart<StorageType>::GetValues() const
        {
            return storage_;
        }

        template <class StorageType>
        inline boost::mutex& LineChart<StorageType>::GetValuesMutex()
        {
            return storageMx_;
        }

        template <class StorageType>
        inline boost::mutex const& LineChart<StorageType>::GetValuesMutex() const
        {
            return storageMx_;
        }

        template <class StorageType>
        inline void LineChart<StorageType>::PrePaint(WTL::CDCHandle dc, CRect const& rc) const
        {
            floatGrid_.DoPaint(dc, rc);
        }

        template <class StorageType>
        inline void LineChart<StorageType>::DoPaint(WTL::CDCHandle dc, CRect const& rc) const
        {
            HGDIOBJ lobj = BeginPaint(dc, rc);

            int x = rc.left;
            int xstep = floatGrid_.GetXStep();
            int lx = x;
            int ly = rc.top;
            VT first = 0;

            dc.SelectPen(pen_);

            {
                boost::mutex::scoped_lock lk(storageMx_);
                const double range = rc.Height() / (upLimit_ ? (double)upLimit_ : 1.);
                for (typename StorageType::const_iterator it = storage_.begin(); it != storage_.end(); ++it)
                {
                    VT current = *it;
                    double dy = current * range;
                    int y = rc.bottom - (int)dy;

                    if (x == rc.left) // First point - do nothing.
                        first = current;

                    else
                    {
                        int dispx = (x > rc.right ? rc.right : x);
                        dc.MoveTo(lx, ly);
                        dc.LineTo(dispx, y);
                    }

                    lx = x;
                    ly = y;

                    x += xstep;
                    if (x > (rc.right + xstep))
                        break;
                }
            }

            DoPaintText(dc, rc, FormatValue(first));
            ::SelectObject(dc, lobj);
        }

        template <class StorageType>
        inline CString LineChart<StorageType>::FormatValue(VT displayValue) const
        {
            CString rv;
            rv.Format(_T("%I64lu / %I64lu"), (unsigned __int64)displayValue, (unsigned __int64)upLimit_);
            return rv;
        }
   }
}
