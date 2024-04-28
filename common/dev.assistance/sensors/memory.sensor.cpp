#include "stdafx.h"
#include "memory.sensor.h"
#include <windows.gdi.rects.h>
#include <string.utils.human.size.h>

namespace Assist
{
    namespace Sensors
    {
        GlobalMemory::GlobalMemory(Facility& facility, PCTSTR name)
            : LineChart(facility, name)
        {
        }

        GlobalMemory::~GlobalMemory()
        {
        }

        void GlobalMemory::Query()
        {
            MEMORYSTATUSEX buffer_;
            ::ZeroMemory(&buffer_, sizeof(buffer_));
            buffer_.dwLength = sizeof(buffer_);
            ::GlobalMemoryStatusEx(&buffer_);

            UpdateValues(buffer_);
        }

        void GlobalMemory::UpdateValues(MEMORYSTATUSEX const& mse)
        {
        }

        CString GlobalMemory::FormatValue(VT displayValue) const
        {
            CString rv;
            rv.Format(_T("%s / %s"), Str::HumanSize(displayValue), Str::HumanSize(GetUpperLimit()));
            return rv;
        }

        //////////////////////////////////////////////////////////////////////////

        MemoryLoad::MemoryLoad(Facility& facility)
            : GlobalMemory(facility, _T("Physical memory in use"))
        {
        }

        void MemoryLoad::UpdateValues(MEMORYSTATUSEX const& mse)
        {
            boost::mutex::scoped_lock lk(GetValuesMutex());
            SetUpperLimit(100);
            GetValues().push_front(mse.dwMemoryLoad);
        }

        CString MemoryLoad::FormatValue(VT displayValue) const
        {
            CString rv;
            rv.Format(_T("%d %%"), (int)displayValue);
            return rv;
        }

        //////////////////////////////////////////////////////////////////////////

        FreePhysicalMemory::FreePhysicalMemory(Facility& facility)
            : GlobalMemory(facility, _T("Free physical memory"))
        {
        }

        void FreePhysicalMemory::UpdateValues(MEMORYSTATUSEX const& mse)
        {
            boost::mutex::scoped_lock lk(GetValuesMutex());
            SetUpperLimit(mse.ullTotalPhys);
            GetValues().push_front(mse.ullAvailPhys);
        }

        //////////////////////////////////////////////////////////////////////////

        FreeVirtualMemory::FreeVirtualMemory(Facility& facility)
            : GlobalMemory(facility, _T("Free virtual memory"))
        {
        }

        void FreeVirtualMemory::UpdateValues(MEMORYSTATUSEX const& mse)
        {
            boost::mutex::scoped_lock lk(GetValuesMutex());
            SetUpperLimit(mse.ullTotalVirtual);
            GetValues().push_front(mse.ullAvailVirtual);
        }

        //////////////////////////////////////////////////////////////////////////

        FreePagefileMemory::FreePagefileMemory(Facility& facility)
            : GlobalMemory(facility, _T("Free pagefile memory"))
        {
        }

        void FreePagefileMemory::UpdateValues(MEMORYSTATUSEX const& mse)
        {
            boost::mutex::scoped_lock lk(GetValuesMutex());
            SetUpperLimit(mse.ullTotalPageFile);
            GetValues().push_front(mse.ullAvailPageFile);
        }
    }
}
