#include "stdafx.h"

typedef struct 
{
    DWORD       Size;
    DWORD       TimeDateStamp;
    WORD        MajorVersion;
    WORD        MinorVersion;
    DWORD       GlobalFlagsClear;
    DWORD       GlobalFlagsSet;
    DWORD       CriticalSectionDefaultTimeout;
    DWORD       DeCommitFreeBlockThreshold;
    DWORD       DeCommitTotalFreeThreshold;
    DWORD       LockPrefixTable;            // VA
    DWORD       MaximumAllocationSize;
    DWORD       VirtualMemoryThreshold;
    DWORD       ProcessHeapFlags;
    DWORD       ProcessAffinityMask;
    WORD        CSDVersion;
    WORD        Reserved1;
    DWORD       EditList;                   // VA
    PUINT_PTR   SecurityCookie;
    PVOID       *SEHandlerTable;
    DWORD       SEHandlerCount;
}
IMAGE_LOAD_CONFIG_DIRECTORY32_2;

const IMAGE_LOAD_CONFIG_DIRECTORY32_2 _load_config_used = 
{
    sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32_2)
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    , 0
    /* TODO: check this stuff */
    , NULL /* &__security_cookie */
    , NULL /* __safe_se_handler_table */
    , 0 /* (DWORD)(DWORD_PTR) &__safe_se_handler_count */
};
