#include "stdafx.h"
#include "crt.exception.handlers.h"
#include "crt.defs.h"
#include "excpt.lolevel.h"
#include "excpt.rubbish.heap.h"

typedef int (*PFILTER)(void);
typedef void (*PHANDLER)(void);

static EXCEPTION_DISPOSITION _CallHandlers3(
      PEXCEPTION_RECORD erecord
    , PEH3_EXCEPTION_REGISTRATION eframe
    , PCONTEXT ctx
    , PDISPATCHER_CONTEXT dc
    , PEXCEPTION_POINTERS epointers
    )
{
    char dmyChars[16] = {0};
    PEXCEPTION_POINTERS ep = epointers;
    PVOID __sp = (PVOID)eframe->Reserved1;
    
    if (eframe->ScopeTable->FilterFunc)
    {
        int fr = ((PFILTER)eframe->ScopeTable->FilterFunc)();
        if ((EXCEPTION_EXECUTE_HANDLER == fr) && eframe->ScopeTable->HandlerFunc)
        {
            ((PHANDLER)eframe->ScopeTable->HandlerFunc)();
            return (EXCEPTION_DISPOSITION)0;
        }
    }

    if (eframe->ScopeTable->HandlerFunc)
        ((PHANDLER)eframe->ScopeTable->HandlerFunc)();

    return ExceptionContinueSearch;
}

static EXCEPTION_DISPOSITION _SeHandler3(
      PEXCEPTION_RECORD erecord
    , PEH3_EXCEPTION_REGISTRATION eframe
    , PCONTEXT ctx
    , PDISPATCHER_CONTEXT dc
    )
{
    if (!IsExceptionRegistration3Valid(eframe))
        return ExceptionContinueSearch;

    PNT_TIB tib = (PNT_TIB)::NtCurrentTeb();
    if (!IsScopeTableEntryValid(eframe->ScopeTable, tib))
        return ExceptionContinueSearch;

    EXCEPTION_POINTERS epointers = { erecord, ctx };
    return _CallHandlers3(erecord, eframe, ctx, dc, &epointers);
}

extern "C" 
{
    EXCEPTION_DISPOSITION _MCDECL __CxxFrameHandler3(
          PEXCEPTION_RECORD erecord
        , void* frame
        , PCONTEXT context
        , void* dc
        )
    {
        return ExceptionContinueSearch;
    }

    // x86 ...
    EXCEPTION_DISPOSITION _MCDECL _except_handler3(
          PEXCEPTION_RECORD erecord
        , void* frame
        , PCONTEXT ctx
        , void* dctx
        )
    {
        PEH3_EXCEPTION_REGISTRATION eframe = (PEH3_EXCEPTION_REGISTRATION)frame;
        PDISPATCHER_CONTEXT dc = (PDISPATCHER_CONTEXT)dctx;
        EXCEPTION_DISPOSITION retval = ExceptionContinueSearch;

        MinCrt::_DumpRegistration3(eframe, false);
        retval = _SeHandler3(erecord, eframe, ctx, dc);
        
        return ExceptionContinueSearch;
    }

    // x86_64 ...
    EXCEPTION_DISPOSITION _MCDECL __C_specific_handler(PEXCEPTION_RECORD er, unsigned __int64 stackFp, unsigned __int64 storeFp, PCONTEXT ctx, PDISPATCHER_CONTEXT dc, unsigned __int64 gp)
    {
        ::ExitProcess(er->ExceptionCode);
        return (EXCEPTION_DISPOSITION)0;
    }
}
