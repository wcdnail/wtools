#pragma once

#include <atlgdi.h>
#include <atltypes.h>

namespace Assist
{
    namespace Sensors
    {
        class FloatGrid
        {
        public:
            FloatGrid(int xstep, int ystep, int xinc, int yinc);
            ~FloatGrid();

            void DoPaint(WTL::CDCHandle dc, CRect const& rc) const;

            int GetXStep() const;
            int GetYStep() const;

        private:
            int xstep_;
            int ystep_;
            int xinc_;
            int yinc_;
            mutable int xent_;
            mutable int yent_;
        };
    }
}
