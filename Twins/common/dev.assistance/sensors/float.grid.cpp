#include "stdafx.h"
#include "float.grid.h"

namespace Assist
{
    namespace Sensors
    {
        FloatGrid::FloatGrid(int xstep, int ystep, int xinc, int yinc)
            : xstep_(xstep)
            , ystep_(ystep)
            , xinc_(xinc)
            , yinc_(yinc)
            , xent_(0)
            , yent_(0)
        {
        }

        FloatGrid::~FloatGrid()
        {
        }

        static inline int StartPoint(int x, int ent)
        {
            return x + ent;
        }

        static void IncrementAndTruncate(int& ent, int inc, int step)
        {
            ent += inc;

            if ((ent >= step) || (ent <= -step))
                ent = 0;
        }

        void FloatGrid::DoPaint(WTL::CDCHandle dc, CRect const& rc) const
        {
            for (int x = StartPoint(rc.left, xent_); x < rc.right; x += xstep_)
            {
                dc.MoveTo(x, rc.top);
                dc.LineTo(x, rc.bottom);
            }

            IncrementAndTruncate(xent_, xinc_, xstep_);

            for (int y = StartPoint(rc.top, yent_); y < rc.bottom; y += ystep_)
            {
                dc.MoveTo(rc.left, y);
                dc.LineTo(rc.right, y);
            }

            IncrementAndTruncate(yent_, yinc_, ystep_);
        }

        int FloatGrid::GetXStep() const
        {
            return xstep_;
        }

        int FloatGrid::GetYStep() const
        {
            return ystep_;
        }
    }
}
