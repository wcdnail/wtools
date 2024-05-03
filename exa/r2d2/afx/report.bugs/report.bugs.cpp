#include "stdafx.h"
#include <string>
#include <sstream>
#include "../report.bugs.codes.h"
#include "../report.bugs.h"
#include "../win.dll.h"
#include "report.dlg.h"

CAppModule _Module;

bool GetCallstack(std::string& result, HANDLE process, HANDLE thread, CONTEXT const* ctx) 
{
    try
    {
        DynamicLibrary dbgHelp("dbghelp.dll", 0);

        typedef BOOL (CALLBACK *FPStackWalk64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, LPCVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
        typedef BOOL (CALLBACK *FPSymInitialize)(HANDLE, PCTSTR, BOOL);
        typedef BOOL (CALLBACK *FPSymGetSymFromAddr64)(HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64);
        typedef BOOL (CALLBACK *FPSymGetLineFromAddr64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
        typedef BOOL (CALLBACK *FPSymCleanup)(HANDLE);
        typedef VOID (CALLBACK *FPRtlCaptureContext)(PCONTEXT);

        FPStackWalk64 StackWalk64;
        FPSymInitialize SymInitialize;
        FPSymGetSymFromAddr64 SymGetSymFromAddr64;
        FPSymGetLineFromAddr64 SymGetLineFromAddr64;
        PFUNCTION_TABLE_ACCESS_ROUTINE64 SymFunctionTableAccess64;
        PGET_MODULE_BASE_ROUTINE64 SymGetModuleBase64;
        FPSymCleanup SymCleanup;

        dbgHelp.GetFunctionWithSameName(SymInitialize);
        dbgHelp.GetFunctionWithSameName(StackWalk64);
        dbgHelp.GetFunctionWithSameName(SymGetLineFromAddr64);
        dbgHelp.GetFunctionWithSameName(SymGetSymFromAddr64);
        dbgHelp.GetFunctionWithSameName(SymFunctionTableAccess64);
        dbgHelp.GetFunctionWithSameName(SymGetModuleBase64);
        dbgHelp.GetFunctionWithSameName(SymCleanup);

        DynamicLibrary kernel32("kernel32.dll", 0);
        FPRtlCaptureContext RtlCaptureContext;
        kernel32.GetFunctionWithSameName(RtlCaptureContext);

        if (!SymInitialize(process, NULL, TRUE)) 
        {
            AtlTrace(_T("DbgHelp: SymInitialize failed\n"));
            return false;
        }

        CONTEXT CTX;
        if (!ctx) 
        {
            ZeroMemory(&CTX, sizeof(CTX));
            CTX.ContextFlags = CONTEXT_FULL;

            if (::GetCurrentThread() == thread) 
                RtlCaptureContext( &CTX );
            else 
            {
                ::SuspendThread(thread);
                ::GetThreadContext(thread, &CTX);
                ::ResumeThread(thread);
            }
            ctx = &CTX;
        }

        STACKFRAME64 SF;
        ZeroMemory(&SF, sizeof(SF));

        SF.AddrPC.Mode          = AddrModeFlat;
        SF.AddrFrame.Mode       = AddrModeFlat;
        SF.AddrStack.Mode       = AddrModeFlat;
        SF.AddrBStore.Mode      = AddrModeFlat;

#ifdef _M_IX86
        DWORD ImageType         = IMAGE_FILE_MACHINE_I386;
        SF.AddrPC.Offset        = ctx->Eip;
        SF.AddrFrame.Offset     = ctx->Ebp;
        SF.AddrStack.Offset     = ctx->Esp;
#elif _M_X64
        DWORD ImageType         = IMAGE_FILE_MACHINE_AMD64;
        SF.AddrPC.Offset        = ctx->Rip;
        SF.AddrFrame.Offset     = ctx->Rsp;
        SF.AddrStack.Offset     = ctx->Rsp;
#elif _M_IA64
        DWORD imageType         = IMAGE_FILE_MACHINE_IA64;
        SF.AddrPC.Offset        = ctx->StIIP;
        SF.AddrFrame.Offset     = ctx->IntSp;
        SF.AddrBStore.Offset    = ctx->RsBSP;
        SF.AddrStack.Offset     = ctx->IntSp;
#endif
        for (int i=0; ; ++i) 
        {
            try 
            {
                if (!StackWalk64(ImageType, process, thread, &SF, ctx, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) 
                {
                    AtlTrace("DbgHelp: StackWalk64 failed\n");
                    break;
                }

                if (SF.AddrPC.Offset == SF.AddrReturn.Offset) 
                {
                    AtlTrace("DbgHelp: StackWalk64 endless call-stack (0x%x) ", SF.AddrPC.Offset);
                    break;
                }

                if (0 == SF.AddrPC.Offset) break;

                enum { STACKWALK_MAX_NAMELEN = 1024 };
                char SymBuf[sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN]={0};
                IMAGEHLP_SYMBOL64 * pSym = (IMAGEHLP_SYMBOL64*)SymBuf;
                pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
                pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

                IMAGEHLP_LINE64 LINE = {0};
                LINE.SizeOfStruct = sizeof(LINE);

                DWORD64 OffsetFromSmybol = 0;
                DWORD LineDisplacement = 0;

                SymGetSymFromAddr64 (process, SF.AddrPC.Offset, &OffsetFromSmybol, pSym);
                SymGetLineFromAddr64(process, SF.AddrPC.Offset, &LineDisplacement, &LINE);

                std::ostringstream temp;
                if (LINE.FileName)
                {
                    temp << LINE.FileName;
                    temp << " (" << std::dec << LINE.LineNumber << "): ";
                }

                if (pSym->Name)
                    temp << pSym->Name;

                temp << "\r\n";

                std::string temp_line(temp.str());
                AtlTrace(temp_line.c_str());
                result += temp_line.c_str();
            }
            catch(...) 
            {
                break;
            }
        }

        SymCleanup(process);
    }
    catch(Exception const& ex) 
    {
        AtlTrace(ex.Dump());
        return false;
    }
    catch(...)
    {
        AtlTrace(__FUNCSIG__ " failed\n");
        return false;
    }

    return true;
}

int DoBugReport(BugReportBase const& report)
{
    HWND owner = GetActiveWindow();
    int rv = ReportShownOk;

    std::string callstack;
    GetCallstack(callstack, GetCurrentProcess(), GetCurrentThread(), NULL);

    try
    {
        CReportBugDlg dialog(report, callstack.c_str());
        dialog.DoModal(owner);
    }
    catch(...)
    {
        MessageBoxA(owner, report.GetMessage(), NULL, MB_ICONSTOP);
        rv = ReportShownFailed;
    }

    return rv;
}
