#include "stdafx.h"
#include "line.chart.h"
#include <deque>

namespace Assist
{
    namespace Sensors
    {
        typedef LineChart<std::deque<unsigned __int64> > UInt64DequeLineChart_T;
    }
}

