#ifndef _mincrt_debug_assistance_h__
#define _mincrt_debug_assistance_h__

namespace MinCrt
{
    typedef char SymString[1024];

    class DebugHelper
    {
    public:
        DebugHelper(DWORD processId, DWORD options = SYMOPT_DEFERRED_LOADS);
        ~DebugHelper();

        int GetSymbolNameByAddress(void* address, SymString& result, DWORD& flags, DWORD64& disp) const;
        int GetSourceLineByAddress(void* address, SymString& source, int& line, DWORD& disp) const;
        DWORD UndecorateSymbolName(PCSTR dname, PSTR uname, DWORD ulen, DWORD flags) const;

        bool WalkStack(DWORD arch
            , HANDLE thread
            , LPSTACKFRAME64 frame
            , PVOID context
            , PREAD_PROCESS_MEMORY_ROUTINE64 memread = NULL
            , PFUNCTION_TABLE_ACCESS_ROUTINE64 ftaccessor = NULL
            , PGET_MODULE_BASE_ROUTINE64 basegetter = NULL
            , PTRANSLATE_ADDRESS_ROUTINE64 addresstor = NULL);

    private:
        typedef BOOL (__stdcall *PSymInitializeW)(HANDLE, PCWSTR, BOOL);
        typedef DWORD (__stdcall *PSymGetOptions)(void);
        typedef DWORD (__stdcall *PSymSetOptions)(DWORD);
        typedef BOOL (__stdcall *PSymCleanup)(HANDLE);
        typedef BOOL (__stdcall *PSymGetSymFromAddr64)(HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64);
        typedef BOOL (__stdcall *PSymGetLineFromAddr64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
        typedef DWORD (__stdcall *PUnDecorateSymbolName)(PCSTR, PSTR, DWORD, DWORD);
        typedef BOOL (__stdcall *PStackWalk64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);

        HANDLE Process;
        HMODULE DbgHelper;
        PSymInitializeW _SymInitializeW;
        PSymGetOptions _SymGetOptions;
        PSymSetOptions _SymSetOptions;
        PSymCleanup _SymCleanup;
        PSymGetSymFromAddr64 _SymGetSymFromAddr64;
        PSymGetLineFromAddr64 _SymGetLineFromAddr64;
        PUnDecorateSymbolName _UnDecorateSymbolName;
        PStackWalk64 _StackWalk64;

    private:
        DebugHelper(DebugHelper const&);
        DebugHelper& operator = (DebugHelper const&);
    };
}

#endif /* _mincrt_debug_assistance_h__ */
