#include "stdafx.h"
#include "crt.debug.assistance.h"

namespace MinCrt
{
    DebugHelper::DebugHelper(DWORD processId, DWORD options /*= SYMOPT_DEFERRED_LOADS*/)
        : Process(::OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId))
        , DbgHelper(::LoadLibrary(_T("dbghelp")))
        , _SymInitializeW(NULL)
        , _SymGetOptions(NULL)
        , _SymSetOptions(NULL)
        , _SymCleanup(NULL)
        , _SymGetSymFromAddr64(NULL)
        , _SymGetLineFromAddr64(NULL)
        , _UnDecorateSymbolName(NULL)
        , _StackWalk64(NULL)
    {
        _SymInitializeW = (PSymInitializeW)::GetProcAddress(DbgHelper, "SymInitializeW");
        _SymGetOptions = (PSymGetOptions)::GetProcAddress(DbgHelper, "SymGetOptions");
        _SymSetOptions = (PSymSetOptions)::GetProcAddress(DbgHelper, "SymSetOptions");
        _SymCleanup = (PSymCleanup)::GetProcAddress(DbgHelper, "SymCleanup");
        _SymGetSymFromAddr64 = (PSymGetSymFromAddr64)::GetProcAddress(DbgHelper, "SymGetSymFromAddr64");
        _SymGetLineFromAddr64 = (PSymGetLineFromAddr64)::GetProcAddress(DbgHelper, "SymGetLineFromAddr64");
        _UnDecorateSymbolName = (PUnDecorateSymbolName)::GetProcAddress(DbgHelper, "UnDecorateSymbolName");
        _StackWalk64 = (PStackWalk64)::GetProcAddress(DbgHelper, "StackWalk64");

        if (_SymGetOptions && _SymSetOptions && _SymInitializeW)
        {
            _SymSetOptions(/*_SymGetOptions() | */options);
            _SymInitializeW(Process, NULL, TRUE);
        }
    }

    DebugHelper::~DebugHelper()
    {
        ::CloseHandle(Process);

        if (_SymCleanup)
            _SymCleanup(Process);

        if (DbgHelper)
            ::FreeLibrary(DbgHelper);
    }

    int DebugHelper::GetSymbolNameByAddress(void* address, SymString& result, DWORD& flags, DWORD64& disp) const
    {
        if (!_SymGetSymFromAddr64)
            return ERROR_NOT_SUPPORTED;

        struct SYMINFO
        {
            IMAGEHLP_SYMBOL64 sym;
            char name[512];
        };

        SYMINFO info;
        info.sym.SizeOfStruct = sizeof(info.sym);
        info.sym.MaxNameLength = sizeof(info.name);

        disp = 0;
        flags = 0;
        BOOL pb = _SymGetSymFromAddr64(Process, (DWORD64)address, &disp, &info.sym);
        int retval = ::GetLastError();

        if (pb)
        {
            ::strcpy_s(result, _countof(result), info.sym.Name);
            flags = info.sym.Flags;
        }

        return retval;
    }

    int DebugHelper::GetSourceLineByAddress(void* address, SymString& source, int& line, DWORD& disp) const
    {
        if (!_SymGetLineFromAddr64)
            return ERROR_NOT_SUPPORTED;

        IMAGEHLP_LINE64 buffer;
        buffer.SizeOfStruct = sizeof(buffer);

        disp = 0;
        line = -1;
        BOOL pb = _SymGetLineFromAddr64(Process, (DWORD64)address, &disp, &buffer);
        int retval = ::GetLastError();

        if (pb)
        {
            ::strcpy_s(source, _countof(source), buffer.FileName);
            line = buffer.LineNumber;
        }

        return retval;
    }

    DWORD DebugHelper::UndecorateSymbolName(PCSTR dname, PSTR uname, DWORD ulen, DWORD flags) const
    {
        if (!_UnDecorateSymbolName)
        {
            ::SetLastError(ERROR_NOT_SUPPORTED);
            return 0;
        }

        return _UnDecorateSymbolName(dname, uname, ulen, flags);
    }

    bool DebugHelper::WalkStack(DWORD arch
        , HANDLE thread
        , LPSTACKFRAME64 frame
        , PVOID context
        , PREAD_PROCESS_MEMORY_ROUTINE64 memread
        , PFUNCTION_TABLE_ACCESS_ROUTINE64 ftaccessor
        , PGET_MODULE_BASE_ROUTINE64 basegetter
        , PTRANSLATE_ADDRESS_ROUTINE64 addresstor)
    {
        if (!_StackWalk64)
        {
            ::SetLastError(ERROR_NOT_SUPPORTED);
            return false;
        }

        return FALSE != _StackWalk64(arch, Process, thread, frame, context, memread, ftaccessor, basegetter, addresstor);
    }
}
