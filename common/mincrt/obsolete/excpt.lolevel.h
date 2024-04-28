/* Great thanx 2 Igor Skochinsky (Hex-Rays). */

#ifndef _mincrt_exceptions_lolevel_h__
#define _mincrt_exceptions_lolevel_h__

#include "crt.min.win.h"

typedef enum _EhConsts
{
    TrylevelNone  = -1,
    TrylevelInvalid = -2,
    VcExceptionMagic = 0x19930520,
    VcExceptionCode = 0xe06d7363,
    SehFilterAccept = 1,
    SehFilterDismiss = -1,
    SehFilterContinueSearch = 0,
    FlagUnwinding = 2,
    FlagExitUnwind = 4,
}
EhConsts;

struct _SCOPETABLE_ENTRY;

/* Frame structure (fs:0 points to the Next)    */
/* -8 DWORD _esp;                               */
/* -4 PEXCEPTION_POINTERS xpointers;            */
typedef struct _EH3_EXCEPTION_REGISTRATION
{
    struct _EH3_EXCEPTION_REGISTRATION *Next;

    PVOID ExceptionHandler;               /* ExceptionHandler points to __except_handler3 (SEH3)        */
                                          /* or __except_handler4 (SEH4)                                */

    struct _SCOPETABLE_ENTRY* ScopeTable; /* ScopeTablepoints to a table of entries describing all      */
                                          /* __try blocks in the function                               */
                                          /* in SEH4, the scope table pointer is XORed with the         */
                                          /* security cookie, to mitigate scope table pointer overwrite */

    DWORD TryLevel;
    PVOID Reserved1;                      /* ? esp */
    PVOID Reserved2;                      /* ? PEXCEPTION_POINTERS */
}
EH3_EXCEPTION_REGISTRATION, *PEH3_EXCEPTION_REGISTRATION;

typedef struct _SCOPETABLE_ENTRY 
{
    DWORD EnclosingLevel;  /* Points to the block which contains the        */
                           /* current one (first table entry is number 0).  */
                           /* Top level (function) is -1 for SEH3,          */
                           /* and -2 for SEH4.                              */
                           
    void* FilterFunc;      /* Points to the exception filter                */
                           /* (expression in the __except operator)         */
                           
    void* HandlerFunc;     /* Points to the __except block body             */
                           /* if FilterFunc is NULL, HandlerFunc is the     */
                           /* __finally block body                          */
                           /* Current try block number is kept in the       */
                           /* TryLevel variable                             */
                           /* of the exception registration frame           */
} 
SCOPETABLE_ENTRY, *PSCOPETABLE_ENTRY;

#if 0
/* SEH4 has additional fields for cookie checks */
typedef struct _EH4_SCOPETABLE 
{
    DWORD GSCookieOffset;
    DWORD GSCookieXOROffset;
    DWORD EHCookieOffset;
    DWORD EHCookieXOROffset;
    /*_EH4_SCOPETABLE_RECORD ScopeRecord[];*/
}
EH4_SCOPETABLE, *PEH4_SCOPETABLE;
#endif

/* -4 void *_esp; */
typedef struct _EHRegistrationNode 
{
    struct _EHRegistrationNode* Next;
    void* frameHandler;
    int state;
}
EHRegistrationNode, *PEHRegistrationNode;

struct _s_UnwindMapEntry;
struct _s_TryBlockMapEntry;
struct _s_ESTypeList;

typedef struct _s_FuncInfo 
{
    unsigned int magicNumber:29;
    unsigned int bbtFlags:3;
    int maxState;
    struct _s_UnwindMapEntry const* pUnwindMap;
    unsigned int nTryBlocks;
    struct _s_TryBlockMapEntry const* pTryBlockMap;
    unsigned int nIPMapEntries;
    void* pIPtoStateMap;
    struct _s_ESTypeList const* pESTypeList;
    int EHFlags;
} 
FuncInfo, *PFuncInfo;

static bool IsExceptionRegistration3Valid(PEH3_EXCEPTION_REGISTRATION reg)
{
    return reg || ((PEH3_EXCEPTION_REGISTRATION)0xffffffff != reg);
}

static bool IsScopeTableEntryValid(PSCOPETABLE_ENTRY st, PNT_TIB tib)
{
    // Align at DWORD boundary ?
    if ((DWORD)st & 0x00000003)
        return false;

    // Out of the stack limit ?
    if ((PVOID)st >= (PVOID)tib->StackLimit 
    &&  (PVOID)st <  (PVOID)tib->StackBase)
        return false;

    return true;
}

static bool IsScopeTableEntryValid(PSCOPETABLE_ENTRY st)
{
    return IsScopeTableEntryValid(st, (PNT_TIB)NtCurrentTeb());
}

static bool IsCppException(PEXCEPTION_RECORD er)
{
    return er
        && (VcExceptionCode == er->ExceptionCode) 
        && (er->NumberParameters > 2)
        ;
}

#endif /* _mincrt_exceptions_lolevel_h__ */
