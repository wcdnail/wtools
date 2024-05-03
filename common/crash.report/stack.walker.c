#include "stdafx.h"
#include "debug.output.h"
#include "stack.walker.h"
#include <assert.h>

#ifdef _MSC_VER
#  pragma warning(disable: 4820) // 4820: '_LOADED_IMAGE': '4' bytes padding added after data member 'NumberOfSections'
#  pragma warning(disable: 4255) // 4255: 'PSYMBOLSERVERGETOPTIONSPROC': no function prototype given: converting '()' to '(void)'
#endif
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVER.h>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp")

static DWORD init_stack_frame(LPSTACKFRAME64 stackFrame, PCONTEXT context)
{
    DWORD result;

    assert(NULL != stackFrame);
    assert(NULL != context);

    ZeroMemory(stackFrame, sizeof(STACKFRAME64));

#ifdef _M_IX86
    result                          = IMAGE_FILE_MACHINE_I386;
    stackFrame->AddrPC.Offset       = context->Eip;
    stackFrame->AddrPC.Mode         = AddrModeFlat;
    stackFrame->AddrFrame.Offset    = context->Ebp;
    stackFrame->AddrFrame.Mode      = AddrModeFlat;
    stackFrame->AddrStack.Offset    = context->Esp;
    stackFrame->AddrStack.Mode      = AddrModeFlat;
#elif _M_X64
    result                          = IMAGE_FILE_MACHINE_AMD64;
    stackFrame->AddrPC.Offset       = context->Rip;
    stackFrame->AddrPC.Mode         = AddrModeFlat;
    stackFrame->AddrFrame.Offset    = context->Rsp;
    stackFrame->AddrFrame.Mode      = AddrModeFlat;
    stackFrame->AddrStack.Offset    = context->Rsp;
    stackFrame->AddrStack.Mode      = AddrModeFlat;
#elif _M_IA64
    result                          = IMAGE_FILE_MACHINE_IA64;
    stackFrame->AddrPC.Offset       = context->StIIP;
    stackFrame->AddrPC.Mode         = AddrModeFlat;
    stackFrame->AddrFrame.Offset    = context->IntSp;
    stackFrame->AddrFrame.Mode      = AddrModeFlat;
    stackFrame->AddrBStore.Offset   = context->RsBSP;
    stackFrame->AddrBStore.Mode     = AddrModeFlat;
    stackFrame->AddrStack.Offset    = context->IntSp;
    stackFrame->AddrStack.Mode      = AddrModeFlat;
#else
#error No implementation for this platforms.
#endif

    return result;
}

static BOOL WINAPI read_process_memory_impl(HANDLE process, DWORD64 base, PVOID buffer, DWORD size, LPDWORD readed)
{
    SIZE_T st;
    BOOL rv = ReadProcessMemory(process, (LPVOID) base, buffer, size, &st);
    *readed = (DWORD) st;
    return rv;
}

int stack_walker(HANDLE process, HANDLE thread, CONTEXT const* scontext, int skip, STACKWALK_CALLBACK cb, void *arg)
{
    enum { MAX_SYM_LEN = 1024 };

    int                 num;
    DWORD               imageType;
    CONTEXT             contextBuffer;
    STACKFRAME64        frameBuffer;
    unsigned char       symBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_LEN];
    IMAGEHLP_LINE64     lineBuffer;
    IMAGEHLP_MODULE64   moduleBuffer;

    PCONTEXT            context  = &contextBuffer;
    LPSTACKFRAME64      frame    = &frameBuffer;
    PIMAGEHLP_SYMBOL64  symbol   = (PIMAGEHLP_SYMBOL64)symBuffer;
    PIMAGEHLP_LINE64    line     = &lineBuffer;
    PIMAGEHLP_MODULE64  module   = &moduleBuffer;
    if (!SymInitialize(process, NULL, TRUE)) {
        DEBUGPRINT2(GetLastError(), "SymInitialize failed. ");
        return 0;
    }
    assert(NULL != scontext);
    memcpy(context, scontext, sizeof(CONTEXT));
    imageType = init_stack_frame(frame, context);
    for (num = 0; ; ++num) {
        if (!StackWalk64(imageType, process, thread, frame, context, 
            read_process_memory_impl, 
            SymFunctionTableAccess64, 
            SymGetModuleBase64, 
            NULL)
            ) {
            break;
        }
        if (num < skip) {
            continue;
        }
        if (frame->AddrPC.Offset == frame->AddrReturn.Offset) {
            DEBUGPRINT("AddrPC == AddrReturn\n");
            break;
        }
        if (frame->AddrPC.Offset) {
            DWORD64 displacement;
            DWORD displacementLine;
            ZeroMemory(symbol, sizeof(symBuffer));
            symbol->SizeOfStruct = sizeof(symBuffer);
            symbol->MaxNameLength = MAX_SYM_LEN;
            ZeroMemory(line, sizeof(IMAGEHLP_LINE64));
            line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            ZeroMemory(module, sizeof(IMAGEHLP_MODULE64));
            module->SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
            SymGetSymFromAddr64(process, frame->AddrPC.Offset, &displacement, symbol);
            SymGetLineFromAddr64(process, frame->AddrPC.Offset, &displacementLine, line);
            SymGetModuleInfo64(process, frame->AddrPC.Offset, module);
            dbg_printf("%s(%d): @%s!%08x%08x+%08x%08x (%s)\n", 
                line->FileName, line->LineNumber, 
                module->ModuleName,
                (DWORD)(frame->AddrPC.Offset >> 32), 
                (DWORD)(frame->AddrPC.Offset & 0xffffffff), 
                (DWORD)(displacement >> 32), 
                (DWORD)(displacement & 0xffffffff), 
                symbol->Name);
            if (NULL != cb) {
                if (!cb(process, thread, context, frame, module, line, symbol, displacement, arg)) {
                    break;
                }
            }
        }
    }
    if (!SymCleanup(process)) {
        DEBUGPRINT2(GetLastError(), "SymCleanup failed. ");
    }
    return 0;
}
