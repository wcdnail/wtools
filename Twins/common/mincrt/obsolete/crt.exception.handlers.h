#ifndef _mincrt_exception_handlers_h__
#define _mincrt_exception_handlers_h__

#ifndef _M_X64
typedef struct _RUNTIME_FUNCTION 
{
    DWORD BeginAddress;
    DWORD EndAddress;
    DWORD UnwindData;
} 
RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;
#endif

#ifdef _M_X64
typedef struct _DISPATCHER_CONTEXT 
{
    ULONG64 ControlPc;
    ULONG64 ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG64 EstablisherFrame;
    ULONG64 TargetIp;
    PCONTEXT ContextRecord;
    PEXCEPTION_ROUTINE LanguageHandler;
    PVOID HandlerData;
} 
DISPATCHER_CONTEXT, *PDISPATCHER_CONTEXT;
#else
typedef struct _DISPATCHER_CONTEXT 
{
    ULONG_PTR ControlPc;
    ULONG_PTR ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG_PTR TargetIp;
    PCONTEXT ContextRecord;
    PEXCEPTION_ROUTINE LanguageHandler;
    PVOID HandlerData;
} 
DISPATCHER_CONTEXT, *PDISPATCHER_CONTEXT;
#endif

typedef struct _EXCEPTION_REGISTRATION_RECORD 
{
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} 
EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

#endif /* _mincrt_exception_handlers_h__ */

