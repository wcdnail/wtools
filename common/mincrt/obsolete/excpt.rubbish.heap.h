#ifndef _mincrt_exceptions_rubbish_heap_h__
#define _mincrt_exceptions_rubbish_heap_h__

#include "crt.defs.h"
#include "crt.report.h"
#include "crt.debug.assistance.h"
#include "crt.exception.handlers.h"
#include "excpt.lolevel.h"

#define _SeperatorCaption               "-----------------------------------------------------------------------------------------------------------------\n"
#define _ExceptionRegistrationCaption   "\nEh registration frame -----------------------------------------------------------------------------------------\n"
#define _ExceptionRecordCaption         "\nEh reocrd -----------------------------------------------------------------------------------------------------\n"
#define _DispatcherContextCaption       "\nEh dispatcher context -----------------------------------------------------------------------------------------\n"
#define _InternalHandlerCaption         "\nException -----------------------------------------------------------------------------------------------------\n"
#define ___C_specific_handlerCaption    "\nC_specific_handler --------------------------------------------------------------------------------------------\n"
#define _DumpRegistration3Caption       "\n_EH3_EXCEPTION_REGISTRATION -----------------------------------------------------------------------------------\n"
#define _EHRegistrationNodeCaption      "\nEHRegistrationNode --------------------------------------------------------------------------------------------\n"

#ifdef __cplusplus
namespace MinCrt
{
    static inline void* GetCurentBp(PCONTEXT context)
    {
#ifdef _M_X64
        return (void*)context->Rbp;
#else
        return (void*)context->Ebp;
#endif 
    }

    static inline void* GetCurentSp(PCONTEXT context)
    {
#ifdef _M_X64
        return (void*)context->Rsp;
#else
        return (void*)context->Esp;
#endif 
    }

    static inline void* GetCurentIp(PCONTEXT context)
    {
#ifdef _M_X64
        return (void*)context->Rip;
#else
        return (void*)context->Eip;
#endif 
    }

    static inline char const* ExceptionCodeString(DWORD code)
    {
#define _TokX(C) case C: return #C

        switch(code) 
        {
            _TokX(EXCEPTION_ACCESS_VIOLATION);
            _TokX(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
            _TokX(EXCEPTION_BREAKPOINT);
            _TokX(EXCEPTION_DATATYPE_MISALIGNMENT);
            _TokX(EXCEPTION_FLT_DENORMAL_OPERAND);
            _TokX(EXCEPTION_FLT_DIVIDE_BY_ZERO);
            _TokX(EXCEPTION_FLT_INEXACT_RESULT);
            _TokX(EXCEPTION_FLT_INVALID_OPERATION);
            _TokX(EXCEPTION_FLT_OVERFLOW);
            _TokX(EXCEPTION_FLT_STACK_CHECK);
            _TokX(EXCEPTION_FLT_UNDERFLOW);
            _TokX(EXCEPTION_ILLEGAL_INSTRUCTION);
            _TokX(EXCEPTION_IN_PAGE_ERROR);
            _TokX(EXCEPTION_INT_DIVIDE_BY_ZERO);
            _TokX(EXCEPTION_INT_OVERFLOW);
            _TokX(EXCEPTION_INVALID_DISPOSITION);
            _TokX(EXCEPTION_NONCONTINUABLE_EXCEPTION);
            _TokX(EXCEPTION_PRIV_INSTRUCTION);
            _TokX(EXCEPTION_SINGLE_STEP);
            _TokX(EXCEPTION_STACK_OVERFLOW);
        }

#undef _TokX

        return "";
    }

    static inline bool _DumpAddress(void* address, bool skipAddress, bool skipSource = true, bool skipLf = false)
    {
        bool retval = false;

        DWORD64 disp64;
        DWORD flags;
        MinCrt::SymString symbol;

        if (0 == Crt().GetDbgHelper().GetSymbolNameByAddress(address, symbol, flags, disp64))
        {
            if (skipAddress)
                ::TraceA("+ 0x%I64x %s", disp64, symbol);
            else
                ::TraceA("0x%p + 0x%I64x %s", address, disp64, symbol);

            retval = true;
        }

        if (!skipSource)
        {
            MinCrt::SymString source;
            int line;
            DWORD disp;
            if (0 == Crt().GetDbgHelper().GetSourceLineByAddress(address, source, line, disp))
                ::TraceA(" `%s, %d`", source, line);
        }

        if (!skipLf)
            ::OutputDebugStringA("\n");

        return retval;
    }

    static inline void _StackWalk(PCONTEXT context)
    {
        STACKFRAME64 frame;

        frame.AddrPC.Offset = (DWORD64)GetCurentIp(context);
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = (DWORD64)GetCurentBp(context);
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = (DWORD64)GetCurentSp(context);
        frame.AddrStack.Mode = AddrModeFlat;

        DWORD arhc = 
#ifdef _M_X64
            IMAGE_FILE_MACHINE_AMD64;
#else
            IMAGE_FILE_MACHINE_I386;
#endif 

        ::OutputDebugStringA("stack walk     "); 
        _DumpAddress(GetCurentIp(context), false);

        while (Crt().GetDbgHelper().WalkStack(arhc, GetCurrentThread(), &frame, context))
        {
            MinCrt::SymString source;
            int line;
            DWORD disp;

            if (0 == Crt().GetDbgHelper().GetSourceLineByAddress((void*)frame.AddrPC.Offset, source, line, disp))
            {
                ::TraceA("%s(%d): ", source, line);

                _DumpAddress((void*)frame.AddrPC.Offset, false, true, true);

                ::OutputDebugStringA("(");
                int count = 0;
                for (size_t i=0; i<_countof(frame.Params); i++)
                {
                    DWORD64 disp64;
                    DWORD flags;
                    MinCrt::SymString symbol;
                    if (0 == Crt().GetDbgHelper().GetSymbolNameByAddress((void*)frame.Params[i], symbol, flags, disp64))
                    {
                        if (count != 0)
                            ::OutputDebugStringA(", ");

                        ::OutputDebugStringA(symbol);
                        ++count;
                    }
                }

                ::OutputDebugStringA(") ");
                ::OutputDebugStringA("\n");
            }
            else
            {
                ::TraceA("0x%p ", frame.AddrPC.Offset);
                _DumpAddress((void*)frame.AddrPC.Offset, true);
            }
        }
    }

    static inline void _DumpExceptionRegistration(PEXCEPTION_REGISTRATION_RECORD reg)
    {
        ::OutputDebugStringA(_ExceptionRegistrationCaption);

        ::TraceA("0x%p ", reg); _DumpAddress((void*)reg, true);

#ifndef _M_X64
        int counter = 0;
        PEXCEPTION_REGISTRATION_RECORD p = reg;
        while ((PEXCEPTION_REGISTRATION_RECORD)0xffffffff != p)
        {
            _DumpAddress(p->Handler, false);
            p = p->Next;
        }
#endif

        ::OutputDebugStringA(_SeperatorCaption);
    }

    static inline void _DumpExceptionRecord(PEXCEPTION_RECORD er, PCONTEXT context)
    {
        ::OutputDebugStringA(_ExceptionRecordCaption);

        ::TraceA("code           0x%x %s\n", er->ExceptionCode, ExceptionCodeString(er->ExceptionCode));
        ::TraceA("address        0x%p ", er->ExceptionAddress); _DumpAddress(er->ExceptionAddress, true);
        ::TraceA("context ip     0x%p ", GetCurentIp(context)); _DumpAddress(GetCurentIp(context), true);
        ::TraceA("flags          0x%p ", er->ExceptionFlags); _DumpAddress((void*)er->ExceptionFlags, true);
        ::TraceA("param#         %d\n", er->NumberParameters);

        for (int i = 0; i < (int)er->NumberParameters; i++)
        {
            ::TraceA("param#%02d       0x%p ", i, er->ExceptionInformation[i]);
            _DumpAddress((void*)er->ExceptionInformation[i], true);
        }

        bool isCpp = (CppExceptionCode == er->ExceptionCode) && (er->NumberParameters > 2);
        ::TraceA("c++ exception  %s\n", isCpp ? "yes" : "no");

        if (isCpp)
        {
            void* eobject = (void*)er->ExceptionInformation[1];
            type_info const& etype = typeid(eobject);

            ::TraceA("type_info      %s (%s)\n", etype.name(), etype.raw_name());

            SymString dname;
            DWORD flags;
            DWORD64 disp;
            if (0 == Crt().GetDbgHelper().GetSymbolNameByAddress((void*)er->ExceptionInformation[2], dname, flags, disp))
            {
                SymString undname;

                Crt().GetDbgHelper().UndecorateSymbolName(dname, undname, _countof(undname), 0);
                ::TraceA("undecorated    %s (%s)\n", undname, dname);
            }
        }

        ::OutputDebugStringA(_SeperatorCaption);
    }

    static inline void _DumpFunctionEntry(PRUNTIME_FUNCTION fe)
    {
        ::TraceA("func entry     0x%p ", fe); _DumpAddress((void*)fe, true);
#if 0 //def _M_X64
        ::TraceA("func begin     0x%p ", fe->BeginAddress); _DumpAddress((void*)fe->BeginAddress, true);
        ::TraceA("func end       0x%p ", fe->EndAddress); _DumpAddress((void*)fe->EndAddress, true);
        ::TraceA("func unwind    0x%p ", fe->UnwindData); _DumpAddress((void*)fe->UnwindData, true);
#endif
    }

    static inline void _DumpDispatcherContext(PDISPATCHER_CONTEXT dc)
    {
        ::OutputDebugStringA(_DispatcherContextCaption);

        ::TraceA("control pc     0x%p ", dc->ControlPc); _DumpAddress((void*)dc->ControlPc, true);
        ::TraceA("image base     0x%p ", dc->ImageBase); _DumpAddress((void*)dc->ImageBase, true);

        if (dc->FunctionEntry)
            _DumpFunctionEntry(dc->FunctionEntry);

        ::TraceA("frame          0x%p ", dc->EstablisherFrame); _DumpAddress((void*)dc->EstablisherFrame, true);
        ::TraceA("target ip      0x%p ", dc->TargetIp); _DumpAddress((void*)dc->TargetIp, true);
        ::TraceA("context ip     0x%p ", GetCurentIp(dc->ContextRecord)); _DumpAddress(GetCurentIp(dc->ContextRecord), true);
        ::TraceA("lang handler   0x%p ", dc->LanguageHandler); _DumpAddress(dc->LanguageHandler, true);
        ::TraceA("handler data   0x%p ", dc->HandlerData); _DumpAddress(dc->HandlerData, true);

        ::OutputDebugStringA(_SeperatorCaption);
    }

    static inline void _DumpScopedTable(PSCOPETABLE_ENTRY st)
    {
        if (!st || !IsScopeTableEntryValid(st))
            return ;

        ::TraceA("  enclevel     %d\n", st->EnclosingLevel);
        ::TraceA("  filter       0x%p ", st->FilterFunc); _DumpAddress((void*)st->FilterFunc, true);
        ::TraceA("  handler      0x%p ", st->HandlerFunc); _DumpAddress((void*)st->HandlerFunc, true);
    }

    static inline bool _DumpRegistration3(PEH3_EXCEPTION_REGISTRATION reg, bool recursive)
    {
        if (!IsExceptionRegistration3Valid(reg))
            return false;

        ::OutputDebugStringA(_DumpRegistration3Caption);

        ::TraceA("next           0x%p ", reg->Next); _DumpAddress((void*)reg->Next, true);
        ::TraceA("handler        0x%p ", reg->ExceptionHandler); _DumpAddress((void*)reg->ExceptionHandler, true);
        ::TraceA("scoped table   0x%p ", reg->ScopeTable); _DumpAddress((void*)reg->ScopeTable, true);
        _DumpScopedTable(reg->ScopeTable);
        ::TraceA("try level      %d\n", reg->TryLevel);
        ::TraceA("reserved1      0x%p ", reg->Reserved1); _DumpAddress(reg->Reserved1, true);
        ::TraceA("reserved2      0x%p ", reg->Reserved2); _DumpAddress(reg->Reserved2, true);

        if (recursive)
        {
            if (!_DumpRegistration3(reg->Next, recursive))
                ::OutputDebugStringA(_SeperatorCaption);
        }
        else
            ::OutputDebugStringA(_SeperatorCaption);

        return true;
    }
    
    static inline bool _DumpRegistrationNode(PEHRegistrationNode reg)
    {
        if (!reg || (PEHRegistrationNode)0xffffffff == reg)
            return false;

        ::OutputDebugStringA(_EHRegistrationNodeCaption);

        ::TraceA("next           0x%p ", reg->Next); _DumpAddress((void*)reg->Next, true);
        ::TraceA("frame handler  0x%p ", reg->frameHandler); _DumpAddress((void*)reg->frameHandler, true);
        ::TraceA("state          0x%x\n", reg->state);

        if (!_DumpRegistrationNode(reg->Next))
            ::OutputDebugStringA(_SeperatorCaption);

        return true;
    }

}
#endif /* __cplusplus */

#endif /* _mincrt_exceptions_rubbish_heap_h__ */
