#include "stdafx.h"
#include "memory.stuff.h"
#include "strut.human.size.h"

#include <psapi.h>
#pragma comment(lib, "psapi")

MemoryStuff::MemoryStuff()
{
    ATLTRACE(_T("Basics ---------------------------------------------------------\n")
             _T("sizeof(char)      : %d bytes\n")
             _T("sizeof(short)     : %d bytes\n")
             _T("sizeof(int)       : %d bytes\n")
             _T("sizeof(long)      : %d bytes\n")
             _T("sizeof(DWORD)     : %d bytes\n")
             _T("sizeof(SIZE_T)    : %d bytes\n")
             _T("sizeof(DWORDLONG) : %d bytes\n")
             _T("----------------------------------------------------------------\n")
             , sizeof(char)
             , sizeof(short)
             , sizeof(int)
             , sizeof(long)
             , sizeof(DWORD)
             , sizeof(SIZE_T)
             , sizeof(DWORDLONG)
             );

#if 0
    MEMORYSTATUS ms = { sizeof(ms), 0 };
    ::GlobalMemoryStatus(&ms);
    /**
    dwLength                The size of the MEMORYSTATUS data structure, in bytes. You do not need to set this member before calling the GlobalMemoryStatus function; the function sets it.
    dwMemoryLoad            A number between 0 and 100 that specifies the approximate percentage of physical memory that is in use (0 indicates no memory use and 100 indicates full memory use).
    dwTotalPhys             The amount of actual physical memory, in bytes.
    dwAvailPhys             The amount of physical memory currently available, in bytes. This is the amount of physical memory that can be immediately reused without having to write its contents to disk first. It is the sum of the size of the standby, free, and zero lists.
    dwTotalPageFile         The current size of the committed memory limit, in bytes. This is physical memory plus the size of the page file, minus a small overhead.
    dwAvailPageFile         The maximum amount of memory the current process can commit, in bytes. This value should be smaller than the system-wide available commit. To calculate this value, call GetPerformanceInfo and subtract the value of CommitTotal from CommitLimit.
    dwTotalVirtual          The size of the user-mode portion of the virtual address space of the calling process, in bytes. This value depends on the type of process, the type of processor, and the configuration of the operating system. For example, this value is approximately 2 GB for most 32-bit processes on an x86 processor and approximately 3 GB for 32-bit processes that are large address aware running on a system with 4 GT RAM Tuning enabled.
    dwAvailVirtual          The amount of unreserved and uncommitted memory currently in the user-mode portion of the virtual address space of the calling process, in bytes.
    /**/
    
    ATLTRACE(_T("MEMORYSTATUS ---------------------------------------------------\n")
             _T("dwLength        : %d\n")
             _T("dwMemoryLoad    : %d\n")
             _T("dwTotalPhys     : %s\n")
             _T("dwAvailPhys     : %s\n")
             _T("dwTotalPageFile : %s\n")
             _T("dwAvailPageFile : %s\n")
             _T("dwTotalVirtual  : %s\n")
             _T("dwAvailVirtual  : %s\n")
             _T("----------------------------------------------------------------\n")
             , /* DWORD  */ ms.dwLength
             , /* DWORD  */ ms.dwMemoryLoad
             , /* SIZE_T */ Twins::HumanSize(ms.dwTotalPhys)
             , /* SIZE_T */ Twins::HumanSize(ms.dwAvailPhys)
             , /* SIZE_T */ Twins::HumanSize(ms.dwTotalPageFile)
             , /* SIZE_T */ Twins::HumanSize(ms.dwAvailPageFile)
             , /* SIZE_T */ Twins::HumanSize(ms.dwTotalVirtual)
             , /* SIZE_T */ Twins::HumanSize(ms.dwAvailVirtual)
             );
#endif // 0

    MEMORYSTATUSEX mse = { sizeof(mse), 0 };
    ::GlobalMemoryStatusEx(&mse);
    /**
    dwLength                The size of the structure, in bytes. You must set this member before calling GlobalMemoryStatusEx.
    dwMemoryLoad            A number between 0 and 100 that specifies the approximate percentage of physical memory that is in use (0 indicates no memory use and 100 indicates full memory use).
    ullTotalPhys            The amount of actual physical memory, in bytes.
    ullAvailPhys            The amount of physical memory currently available, in bytes. This is the amount of physical memory that can be immediately reused without having to write its contents to disk first. It is the sum of the size of the standby, free, and zero lists.
    ullTotalPageFile        The current committed memory limit for the system or the current process, whichever is smaller, in bytes. To get the system-wide committed memory limit, call GetPerformanceInfo.
    ullAvailPageFile        The maximum amount of memory the current process can commit, in bytes. This value is equal to or smaller than the system-wide available commit value. To calculate the system-wide available commit value, call GetPerformanceInfo and subtract the value of CommitTotal from the value of CommitLimit.
    ullTotalVirtual         The size of the user-mode portion of the virtual address space of the calling process, in bytes. This value depends on the type of process, the type of processor, and the configuration of the operating system. For example, this value is approximately 2 GB for most 32-bit processes on an x86 processor and approximately 3 GB for 32-bit processes that are large address aware running on a system with 4-gigabyte tuning enabled.
    ullAvailVirtual         The amount of unreserved and uncommitted memory currently in the user-mode portion of the virtual address space of the calling process, in bytes.
    ullAvailExtendedVirtual Reserved. This value is always 0.
    /**/

    ATLTRACE(_T("MEMORYSTATUSEX -------------------------------------------------\n")
             _T("dwLength                : %d\n")
             _T("dwMemoryLoad            : %d\n")
             _T("ullTotalPhys            : %s\n")
             _T("ullAvailPhys            : %s\n")
             _T("ullTotalPageFile        : %s\n")
             _T("ullAvailPageFile        : %s\n")
             _T("ullTotalVirtual         : %s\n")
             _T("ullAvailVirtual         : %s\n")
             _T("ullAvailExtendedVirtual : %s\n")
             _T("----------------------------------------------------------------\n")
            , /* DWORD     */ Twins::HumanSize(mse.dwLength               )
            , /* DWORD     */ Twins::HumanSize(mse.dwMemoryLoad           )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullTotalPhys           )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullAvailPhys           )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullTotalPageFile       )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullAvailPageFile       )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullTotalVirtual        )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullAvailVirtual        )
            , /* DWORDLONG */ Twins::HumanSize(mse.ullAvailExtendedVirtual)
            );

    PERFORMANCE_INFORMATION pi = {0};
    ::GetPerformanceInfo(&pi, sizeof(pi));
    /**
    cb                  The size of this structure, in bytes.
    CommitTotal         The number of pages currently committed by the system. Note that committing pages (using VirtualAlloc with MEM_COMMIT) changes this value immediately; however, the physical memory is not charged until the pages are accessed.
    CommitLimit         The current maximum number of pages that can be committed by the system without extending the paging file(s). This number can change if memory is added or deleted, or if pagefiles have grown, shrunk, or been added. If the paging file can be extended, this is a soft limit.
    CommitPeak          The maximum number of pages that were simultaneously in the committed state since the last system reboot.
    PhysicalTotal       The amount of actual physical memory, in pages.
    PhysicalAvailable   The amount of physical memory currently available, in pages. This is the amount of physical memory that can be immediately reused without having to write its contents to disk first. It is the sum of the size of the standby, free, and zero lists.
    SystemCache         The amount of system cache memory, in pages. This is the size of the standby list plus the system working set.
    KernelTotal         The sum of the memory currently in the paged and nonpaged kernel pools, in pages.
    KernelPaged         The memory currently in the paged kernel pool, in pages.
    KernelNonpaged      The memory currently in the nonpaged kernel pool, in pages.
    PageSize            The size of a page, in bytes.
    HandleCount         The current number of open handles.
    ProcessCount        The current number of processes.
    ThreadCount         The current number of threads.
    /**/

    ATLTRACE(_T("PERFORMANCE_INFORMATION ----------------------------------------\n")
             _T("cb                : %d\n")
             _T("CommitTotal       : %s\n")
             _T("CommitLimit       : %s\n")
             _T("CommitPeak        : %s\n")
             _T("PhysicalTotal     : %s\n")
             _T("PhysicalAvailable : %s\n")
             _T("SystemCache       : %s\n")
             _T("KernelTotal       : %s\n")
             _T("KernelPaged       : %s\n")
             _T("KernelNonpaged    : %s\n")
             _T("PageSize          : %s\n")
             _T("HandleCount       : %d\n")
             _T("ProcessCount      : %d\n")
             _T("ThreadCount       : %d\n")
             _T("----------------------------------------------------------------\n")
             , /* DWORD  */ pi.cb
             , /* SIZE_T */ Twins::HumanSize(pi.CommitTotal      )
             , /* SIZE_T */ Twins::HumanSize(pi.CommitLimit      )
             , /* SIZE_T */ Twins::HumanSize(pi.CommitPeak       )
             , /* SIZE_T */ Twins::HumanSize(pi.PhysicalTotal    )
             , /* SIZE_T */ Twins::HumanSize(pi.PhysicalAvailable)
             , /* SIZE_T */ Twins::HumanSize(pi.SystemCache      )
             , /* SIZE_T */ Twins::HumanSize(pi.KernelTotal      )
             , /* SIZE_T */ Twins::HumanSize(pi.KernelPaged      )
             , /* SIZE_T */ Twins::HumanSize(pi.KernelNonpaged   )
             , /* SIZE_T */ Twins::HumanSize(pi.PageSize         )
             , /* DWORD  */ pi.HandleCount
             , /* DWORD  */ pi.ProcessCount
             , /* DWORD  */ pi.ThreadCount
             );

    PROCESS_MEMORY_COUNTERS_EX pmce = {0};
    ::GetProcessMemoryInfo(::GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmce, sizeof(pmce));
    /**
    cb                          The size of the structure, in bytes.
    PageFaultCount              The number of page faults.
    PeakWorkingSetSize          The peak working set size, in bytes.
    WorkingSetSize              The current working set size, in bytes.
    QuotaPeakPagedPoolUsage     The peak paged pool usage, in bytes.
    QuotaPagedPoolUsage         The current paged pool usage, in bytes.
    QuotaPeakNonPagedPoolUsage  The peak nonpaged pool usage, in bytes.
    QuotaNonPagedPoolUsage      The current nonpaged pool usage, in bytes.
    PagefileUsage               The Commit Charge value in bytes for this process. Commit Charge is the total amount of memory that the memory manager has committed for a running process.
                                Windows 7 and Windows Server 2008 R2 and earlier:  PagefileUsage is always zero. Check PrivateUsage instead.
    PeakPagefileUsage           The peak value in bytes of the Commit Charge during the lifetime of this process.
    PrivateUsage                Same as PagefileUsage. The Commit Charge value in bytes for this process. Commit Charge is the total amount of memory that the memory manager has committed for a running process.
    /**/

    ATLTRACE(_T("PROCESS_MEMORY_COUNTERS_EX -------------------------------------\n")
             _T("cb                         : %d\n")
             _T("PageFaultCount             : %d\n")
             _T("PeakWorkingSetSize         : %s\n")
             _T("WorkingSetSize             : %s\n")
             _T("QuotaPeakPagedPoolUsage    : %s\n")
             _T("QuotaPagedPoolUsage        : %s\n")
             _T("QuotaPeakNonPagedPoolUsage : %s\n")
             _T("QuotaNonPagedPoolUsage     : %s\n")
             _T("PagefileUsage              : %s\n")
             _T("PeakPagefileUsage          : %s\n")
             _T("PrivateUsage               : %s\n")
             _T("----------------------------------------------------------------\n")
             , /* DWORD  */ pmce.cb
             , /* DWORD  */ pmce.PageFaultCount
             , /* SIZE_T */ Twins::HumanSize(pmce.PeakWorkingSetSize        )
             , /* SIZE_T */ Twins::HumanSize(pmce.WorkingSetSize            )
             , /* SIZE_T */ Twins::HumanSize(pmce.QuotaPeakPagedPoolUsage   )
             , /* SIZE_T */ Twins::HumanSize(pmce.QuotaPagedPoolUsage       )
             , /* SIZE_T */ Twins::HumanSize(pmce.QuotaPeakNonPagedPoolUsage)
             , /* SIZE_T */ Twins::HumanSize(pmce.QuotaNonPagedPoolUsage    )
             , /* SIZE_T */ Twins::HumanSize(pmce.PagefileUsage             )
             , /* SIZE_T */ Twins::HumanSize(pmce.PeakPagefileUsage         )
             , /* SIZE_T */ Twins::HumanSize(pmce.PrivateUsage              )
             );
}

MemoryStuff::~MemoryStuff()
{
}
