#pragma once

#include "line.chart.h"
#include <deque>

namespace Assist
{
    namespace Sensors
    {
        class Facility;

        class GlobalMemory: public LineChart<std::deque<unsigned __int64> >
        {
        public:
            virtual ~GlobalMemory();

        protected:
            GlobalMemory(Facility& facility, PCTSTR name);

            virtual void Query();
            virtual void UpdateValues(MEMORYSTATUSEX const& mse) = 0;
            virtual CString FormatValue(VT displayValue) const;
        };

        //////////////////////////////////////////////////////////////////////////

        class MemoryLoad: public GlobalMemory
        {
        private:
            friend Facility;
            MemoryLoad(Facility& facility);
            virtual void UpdateValues(MEMORYSTATUSEX const& mse);
            virtual CString FormatValue(VT displayValue) const;
        };

        //////////////////////////////////////////////////////////////////////////

        class FreePhysicalMemory: public GlobalMemory
        {
        private:
            friend Facility;
            FreePhysicalMemory(Facility& facility);
            virtual void UpdateValues(MEMORYSTATUSEX const& mse);
        };

        //////////////////////////////////////////////////////////////////////////

        class FreeVirtualMemory: public GlobalMemory
        {
        private:
            friend Facility;
            FreeVirtualMemory(Facility& facility);
            virtual void UpdateValues(MEMORYSTATUSEX const& mse);
        };

        //////////////////////////////////////////////////////////////////////////

        class FreePagefileMemory: public GlobalMemory
        {
        private:
            friend Facility;
            FreePagefileMemory(Facility& facility);
            virtual void UpdateValues(MEMORYSTATUSEX const& mse);
        };
    }
}
