#include "run.context.info.h"
#include "stack.walker.h"
#include <assert.h>

#ifdef _MSC_VER
#  pragma warning(disable: 4820) // 4820: '_LOADED_IMAGE': '4' bytes padding added after data member 'NumberOfSections'
#  pragma warning(disable: 4255) // 4255: 'PSYMBOLSERVERGETOPTIONSPROC': no function prototype given: converting '()' to '(void)'
#  pragma warning(disable: 4191) // 4191: 'type cast': unsafe conversion from 'FARPROC' to 'PGPI'
                                 // 4191:         Making a function call using the resulting pointer may cause your program to fail
#  pragma warning(disable: 4996) // 4996: 'GetVersionExW': was declared deprecated
#  pragma warning(disable: 4090) // 4090: 'function': different 'const' qualifiers
#  pragma warning(disable: 4710) // 4710: 'HRESULT StringCchPrintfW(STRSAFE_LPWSTR,size_t,STRSAFE_LPCWSTR,...)': function not inlined
#  pragma warning(disable: 4668) // 4668: '_WIN32_WINNT_WIN10_TH2' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#endif

#define SECURITY_WIN32
#define WIN32_LEAN_AND_MEAN

#include <SDKDDKVER.h>
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <process.h>
#include <security.h>
#include <strsafe.h>
#include <tchar.h>

#pragma comment(lib, "SECUR32") // ##FIXME: Load SECUR32 dynamically

void get_context(HANDLE thread, DWORD threadId, PCONTEXT context, DWORD flags);

/************************************************************************
 *     Code info                                                        *
 *                                                                      *
 ************************************************************************/

char const* exception_code_string(unsigned code)
{
    #define _caseX_retXToken(C) case C: return #C
    switch(code) {
    _caseX_retXToken(EXCEPTION_ACCESS_VIOLATION      );
    _caseX_retXToken(EXCEPTION_ARRAY_BOUNDS_EXCEEDED );
    _caseX_retXToken(EXCEPTION_BREAKPOINT            );
    _caseX_retXToken(EXCEPTION_DATATYPE_MISALIGNMENT );
    _caseX_retXToken(EXCEPTION_FLT_DENORMAL_OPERAND  );
    _caseX_retXToken(EXCEPTION_FLT_DIVIDE_BY_ZERO    );
    _caseX_retXToken(EXCEPTION_FLT_INEXACT_RESULT    );
    _caseX_retXToken(EXCEPTION_FLT_INVALID_OPERATION );
    _caseX_retXToken(EXCEPTION_FLT_OVERFLOW          );
    _caseX_retXToken(EXCEPTION_FLT_STACK_CHECK       );
    _caseX_retXToken(EXCEPTION_FLT_UNDERFLOW         );
    _caseX_retXToken(EXCEPTION_ILLEGAL_INSTRUCTION   );
    _caseX_retXToken(EXCEPTION_IN_PAGE_ERROR         );
    _caseX_retXToken(EXCEPTION_INT_DIVIDE_BY_ZERO    );
    _caseX_retXToken(EXCEPTION_INT_OVERFLOW          );
    _caseX_retXToken(EXCEPTION_INVALID_DISPOSITION   );
    _caseX_retXToken(EXCEPTION_NONCONTINUABLE_EXCEPTION);
    _caseX_retXToken(EXCEPTION_PRIV_INSTRUCTION      );
    _caseX_retXToken(EXCEPTION_SINGLE_STEP           );
    _caseX_retXToken(EXCEPTION_STACK_OVERFLOW        );
    }
    #undef _caseX_retXToken
    if (0xe0 == (code >> 24)) { /* Visual C++ */
        return "C++EXCEPTION";
    }
    return "";
}

static PCSTR to_utf8(PCWSTR string, int len)
{
    int    len2 = len * 2 + 1;
    PSTR result = (PSTR)calloc(len2, 1);
    if (NULL != result) {
        WideCharToMultiByte(CP_UTF8, 0, string, len, result, len2, NULL, NULL);
    }
    return result;
}

void describe_operation_code(STRING_BUFFER_PTR buffer, int code)
{
    PCSTR output = exception_code_string(code);
    
    if (strlen(output) > 1) {
        sb_format(buffer, "%s", output);
    }
    else {
        HLOCAL local = NULL;
        DWORD l = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, (DWORD)code, 0, (PWSTR)&local, 0, NULL);
        if (l > 2) {
            PWSTR message = (PWSTR)LocalLock(local);
            message[l-2] = L'\0';
            output = to_utf8(message, l);
            if (NULL != output) {
                sb_format(buffer, "(%d) %s", code, output);
                free((void*)output);
            }
            LocalUnlock(local);
        }
        LocalFree(local);
    }
}

/************************************************************************
 *     System info                                                      *
 *                                                                      *
 ************************************************************************/

static char const* cpu_arch_string(DWORD code)
{
    #define _caseX_retXToken(C) case C: return #C
    switch(code) 
    {
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_INTEL);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_MIPS);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_ALPHA);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_PPC);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_SHX);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_ARM);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_IA64);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_ALPHA64);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_MSIL);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_AMD64);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_IA32_ON_WIN64);
    _caseX_retXToken(PROCESSOR_ARCHITECTURE_UNKNOWN);
    }
    #undef _caseX_retXToken
    return NULL;
}

typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

static BOOL get_os_display_name(WCHAR* result, size_t bufSize, LPOSVERSIONINFOEX osvi, LPSYSTEM_INFO si)
{
    PGPI pGPI;
    DWORD dwType;

    if (VER_PLATFORM_WIN32_NT == osvi->dwPlatformId && 
        osvi->dwMajorVersion > 4 )
    {
        StringCchCopyW(result, bufSize, L"Microsoft ");

        if ( osvi->dwMajorVersion == 6 )
        {
            if( osvi->dwMinorVersion == 0 )
            {
                if( osvi->wProductType == VER_NT_WORKSTATION )
                    StringCchCatW(result, bufSize, L"Windows Vista ");
                else 
                    StringCchCatW(result, bufSize, L"Windows Server 2008 ");
            }

            if ( osvi->dwMinorVersion == 1 )
            {
                if( osvi->wProductType == VER_NT_WORKSTATION )
                    StringCchCatW(result, bufSize, L"Windows 7 ");
                else 
                    StringCchCatW(result, bufSize, L"Windows Server 2008 R2 ");
            }

            pGPI = (PGPI)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetProductInfo");
            if (pGPI)
            {
                pGPI(osvi->dwMajorVersion, 
                    osvi->dwMinorVersion, 0, 0, &dwType);
            }
            else
                dwType = PRODUCT_UNDEFINED;

            switch( dwType )
            {
            case PRODUCT_ULTIMATE:
                StringCchCatW(result, bufSize, L"Ultimate Edition");
                break;
            case PRODUCT_HOME_PREMIUM:
                StringCchCatW(result, bufSize, L"Home Premium Edition");
                break;
            case PRODUCT_HOME_BASIC:
                StringCchCatW(result, bufSize, L"Home Basic Edition");
                break;
            case PRODUCT_ENTERPRISE:
                StringCchCatW(result, bufSize, L"Enterprise Edition");
                break;
            case PRODUCT_BUSINESS:
                StringCchCatW(result, bufSize, L"Business Edition");
                break;
            case PRODUCT_STARTER:
                StringCchCatW(result, bufSize, L"Starter Edition");
                break;
            case PRODUCT_CLUSTER_SERVER:
                StringCchCatW(result, bufSize, L"Cluster Server Edition");
                break;
            case PRODUCT_DATACENTER_SERVER:
                StringCchCatW(result, bufSize, L"Datacenter Edition");
                break;
            case PRODUCT_DATACENTER_SERVER_CORE:
                StringCchCatW(result, bufSize, L"Datacenter Edition (core installation)");
                break;
            case PRODUCT_ENTERPRISE_SERVER:
                StringCchCatW(result, bufSize, L"Enterprise Edition");
                break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
                StringCchCatW(result, bufSize, L"Enterprise Edition (core installation)");
                break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
                StringCchCatW(result, bufSize, L"Enterprise Edition for Itanium-based Systems");
                break;
            case PRODUCT_SMALLBUSINESS_SERVER:
                StringCchCatW(result, bufSize, L"Small Business Server");
                break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                StringCchCatW(result, bufSize, L"Small Business Server Premium Edition");
                break;
            case PRODUCT_STANDARD_SERVER:
                StringCchCatW(result, bufSize, L"Standard Edition");
                break;
            case PRODUCT_STANDARD_SERVER_CORE:
                StringCchCatW(result, bufSize, L"Standard Edition (core installation)");
                break;
            case PRODUCT_WEB_SERVER:
                StringCchCatW(result, bufSize, L"Web Server Edition");
                break;
            }
        }

        if (osvi->dwMajorVersion == 5 && osvi->dwMinorVersion == 2)
        {
            if(GetSystemMetrics(SM_SERVERR2))
                StringCchCatW(result, bufSize, L"Windows Server 2003 R2, ");
            else if (osvi->wSuiteMask & VER_SUITE_STORAGE_SERVER)
                StringCchCatW(result, bufSize, L"Windows Storage Server 2003");
            else if (osvi->wProductType == VER_NT_WORKSTATION &&
                si->wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
            {
                StringCchCatW(result, bufSize, L"Windows XP Professional x64 Edition");
            }
            else 
                StringCchCatW(result, bufSize, L"Windows Server 2003, ");

            // Test for the server type.
            if (osvi->wProductType != VER_NT_WORKSTATION)
            {
                if (si->wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
                {
                    if(osvi->wSuiteMask & VER_SUITE_DATACENTER)
                        StringCchCatW(result, bufSize, L"Datacenter Edition for Itanium-based Systems");
                    else if( osvi->wSuiteMask & VER_SUITE_ENTERPRISE )
                        StringCchCatW(result, bufSize, L"Enterprise Edition for Itanium-based Systems");
                }

                else if (si->wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
                {
                    if( osvi->wSuiteMask & VER_SUITE_DATACENTER )
                        StringCchCatW(result, bufSize, L"Datacenter x64 Edition");
                    else if( osvi->wSuiteMask & VER_SUITE_ENTERPRISE )
                        StringCchCatW(result, bufSize, L"Enterprise x64 Edition");
                    else 
                        StringCchCatW(result, bufSize, L"Standard x64 Edition");
                }

                else
                {
                    if (osvi->wSuiteMask & VER_SUITE_COMPUTE_SERVER)
                        StringCchCatW(result, bufSize, L"Compute Cluster Edition");
                    else if(osvi->wSuiteMask & VER_SUITE_DATACENTER)
                        StringCchCatW(result, bufSize, L"Datacenter Edition");
                    else if(osvi->wSuiteMask & VER_SUITE_ENTERPRISE)
                        StringCchCatW(result, bufSize, L"Enterprise Edition");
                    else if (osvi->wSuiteMask & VER_SUITE_BLADE)
                        StringCchCatW(result, bufSize, L"Web Edition");
                    else 
                        StringCchCatW(result, bufSize, L"Standard Edition");
                }
            }
        }

        if (osvi->dwMajorVersion == 5 && osvi->dwMinorVersion == 1)
        {
            StringCchCatW(result, bufSize, L"Windows XP ");
            if (osvi->wSuiteMask & VER_SUITE_PERSONAL)
                StringCchCatW(result, bufSize, L"Home Edition");
            else 
                StringCchCatW(result, bufSize, L"Professional");
        }

        if ( osvi->dwMajorVersion == 5 && osvi->dwMinorVersion == 0 )
        {
            StringCchCatW(result, bufSize, L"Windows 2000 ");

            if ( osvi->wProductType == VER_NT_WORKSTATION )
            {
                StringCchCatW(result, bufSize, L"Professional");
            }
            else 
            {
                if( osvi->wSuiteMask & VER_SUITE_DATACENTER )
                    StringCchCatW(result, bufSize, L"Datacenter Server");
                else if( osvi->wSuiteMask & VER_SUITE_ENTERPRISE )
                    StringCchCatW(result, bufSize, L"Advanced Server");
                else 
                    StringCchCatW(result, bufSize, L"Server");
            }
        }

        // Include service pack (if any) and build number.
        if( osvi->szCSDVersion[0] )
        {
            StringCchCatW(result, bufSize, L" ");
            StringCchCatW(result, bufSize, osvi->szCSDVersion);
        }

        {
            WCHAR buf[80];
            StringCchPrintfW(buf, 80, L" (build %d)", osvi->dwBuildNumber);
            StringCchCatW(result, bufSize, buf);
        }

        if (osvi->dwMajorVersion >= 6)
        {
            if (si->wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
                StringCchCatW(result, bufSize, L", 64-bit");
            else if (si->wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
                StringCchCatW(result, bufSize, L", 32-bit");
        }

        return TRUE; 
    }

    return FALSE;
}

void describe_system_info(STRING_BUFFER_PTR buffer)
{
    SYSTEM_INFO system;
    OSVERSIONINFOEXW version;
    wchar_t userName[256];
    wchar_t hostName[256];
    wchar_t osName[256];
    ULONG userNameLen = _countof(userName);
    DWORD hostNameLen = _countof(hostName);
    PCSTR outputUserName = NULL;
    PCSTR outputHostName = NULL;
    PCSTR outputOSName = NULL;
    
    ZeroMemory(&system, sizeof(system));
    ZeroMemory(&version, sizeof(version));
    ZeroMemory(userName, sizeof(userName));
    ZeroMemory(osName, sizeof(osName));

    version.dwOSVersionInfoSize = sizeof(version);

    GetSystemInfo(&system);
    GetVersionExW((OSVERSIONINFOW*)&version);
    get_os_display_name(osName, _countof(osName), &version, &system);
    GetUserNameExW(NameSamCompatible, userName, &userNameLen);
    GetComputerNameExW(ComputerNameDnsFullyQualified, hostName, &hostNameLen);

    outputUserName = to_utf8(userName, userNameLen);
    outputHostName = to_utf8(hostName, hostNameLen);
    outputOSName = to_utf8(osName, _countof(osName));

        
    sb_format(buffer, 
        /* OSVERSIONINFOEX  */
        "\r\nOS                         %s"
        "\r\nOS Version                 %d.%d.%d (%d)"

        /* SYSTEM_INFO  */
        "\r\nCPU                        %d %s (%d)"
        "\r\nCPU count                  %d"
        "\r\nCPU level                  %d"
        "\r\nCPU revision               %d"
        "\r\nPageSize                   %d"
        "\r\nMinimum app address        %p"
        "\r\nMaximum app address        %p"
        "\r\nActive cpu mask            %d"
        
        "\r\nAllocation granularity     %d"

        /* */
        "\r\nHost                       %s"
        "\r\nUser name                  %s"
        "\r\nGDI objects                %d"
        "\r\nUSER objects               %d"
        "\r\n"
        ,
        /* OSVERSIONINFOEX  */
        outputOSName,
        version.dwMajorVersion, version.dwMinorVersion, 
        version.dwBuildNumber, version.dwPlatformId,

        /* SYSTEM_INFO  */
        system.dwProcessorType, cpu_arch_string(system.wProcessorArchitecture), 
        system.wProcessorArchitecture,
        system.dwNumberOfProcessors,
        system.wProcessorLevel,
        system.wProcessorRevision,
        system.dwPageSize,
        system.lpMinimumApplicationAddress,
        system.lpMaximumApplicationAddress,
        system.dwActiveProcessorMask,
        system.dwAllocationGranularity,

        /* */
        outputHostName,
        outputUserName,
        GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS),
        GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS)
        );

    sb_free_string(outputUserName);
    sb_free_string(outputHostName);
    sb_free_string(outputOSName);
}

/************************************************************************
 *     Call-stack                                                       *
 *                                                                      *
 ************************************************************************/

static BOOL CALLBACK write_stack_frame_info(HANDLE process, HANDLE thread, 
                                            const PCONTEXT context, 
                                            const LPSTACKFRAME64 frame,
                                            const PIMAGEHLP_MODULE64 module, 
                                            const PIMAGEHLP_LINE64 line, 
                                            const PIMAGEHLP_SYMBOL64 symbol,
                                            DWORD64 displacement,
                                            STRING_BUFFER_PTR buffer)
{
    UNREFERENCED_PARAMETER(process);
    UNREFERENCED_PARAMETER(thread);
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(line);
    sb_format(buffer, "\r\n%016I64x %s!%s + 0x%I64x", 
        frame->AddrPC.Offset, 
        module->ModuleName, symbol->Name, 
        displacement);

    return TRUE;
}

void describe_callstack(STRING_BUFFER_PTR buffer, PCRUN_CONTEXT rc)
{
    HANDLE   pid = run_context_get_pid(rc);
    HANDLE   tid = run_context_get_tid(rc);
    PCONTEXT ctx = run_context_get_ctx(rc);
    int     skip = run_context_get_skp(rc);
    stack_walker(pid, tid, ctx, skip, (STACKWALK_CALLBACK)write_stack_frame_info, buffer);
}

/************************************************************************
 *     Modules                                                          *
 *                                                                      *
 ************************************************************************/

static BOOL CALLBACK write_module_info(PCSTR name, DWORD64 base, ULONG size,
                                       STRING_BUFFER* buffer)
{
    int n = sb_format(buffer, "\r\n%08x%08x %16lu %s", 
        (DWORD)(base >> 32), (DWORD)(base & 0xffffffff), 
        size, name);

    return n > 0;
}

void describe_modules(STRING_BUFFER_PTR buffer, void* pid)
{
    EnumerateLoadedModules64(pid, 
        (PENUMLOADED_MODULES_CALLBACK64)write_module_info, buffer);
}

/************************************************************************
 *     Threads                                                          *
 *                                                                      *
 ************************************************************************/

typedef void (CALLBACK *PFENUMTHREAD)(size_t, THREADENTRY32 const*, void*);

static size_t enum_threads(HANDLE snapshot, DWORD processId, 
                           PFENUMTHREAD callBack, void *arg)
{
    THREADENTRY32 te;
    size_t num = 0;

    te.dwSize = sizeof(te);
    if (Thread32First(snapshot, &te)) 
    {
        do
        {
            if (te.th32OwnerProcessID == processId)
            {
                if (NULL != callBack)
                    callBack(num, &te, arg);

                ++num;
            }

            te.dwSize = sizeof(te);
        }
        while (Thread32Next(snapshot, &te));
    }

    return num;
}


typedef struct 
{
    HANDLE thread;
    unsigned threadId;

    HANDLE process;
    unsigned thisThreadId;
    STRING_BUFFER_PTR buffer;
    HANDLE done;
}
GET_THREADS_LIST_REQUEST;

static void CALLBACK write_thread_info(size_t num, THREADENTRY32 const* te, 
                                       GET_THREADS_LIST_REQUEST* request)
{
    CONTEXT context;
    HANDLE process, thread;
    DWORD threadId;
    
    assert(NULL != request);
    assert(NULL != te);

    process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, te->th32OwnerProcessID);
    thread = OpenThread(THREAD_ALL_ACCESS, TRUE, threadId = te->th32ThreadID);

    sb_format(request->buffer, "\r\n%s %d %d %d", 
        (request->thisThreadId == threadId ? ">>>" : "..."),
        num+1, threadId, te->tpBasePri);

    get_context(thread, threadId, &context, CONTEXT_CONTROL);

    stack_walker(process, thread, &context, 0,
        (STACKWALK_CALLBACK)write_stack_frame_info, request->buffer);
    
    CloseHandle(thread);
    CloseHandle(process);
}

static unsigned get_threads_list_proc(GET_THREADS_LIST_REQUEST* request)
{
    HANDLE snapshot;
    DWORD processId = GetProcessId(request->process);
    
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, processId);
    if (INVALID_HANDLE_VALUE == snapshot)
        return 0;

    enum_threads(snapshot, processId, 
        (PFENUMTHREAD)write_thread_info, request);

    CloseHandle(snapshot);
    SetEvent(request->done);
    return 0;
}

void describe_threads(STRING_BUFFER_PTR buffer, void* pid, int inSeparateThread)
{
    GET_THREADS_LIST_REQUEST request;
    DWORD wait = WAIT_OBJECT_0;

    request.thread          = NULL;
    request.threadId        = 0;
    request.process         = pid;
    request.thisThreadId    = GetCurrentThreadId();
    request.buffer          = buffer;
    request.done            = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (inSeparateThread)
    {
        request.thread = (HANDLE)_beginthreadex(NULL, 0, 
            (unsigned(__stdcall*)(void*))get_threads_list_proc, &request, 
            CREATE_SUSPENDED, &request.threadId);
    }

    if (NULL != request.thread)
    {
        ResumeThread(request.thread);
        wait = WaitForSingleObject(request.done, INFINITE);
        CloseHandle(request.thread);
    }
    else
    {
        get_threads_list_proc(&request);
    }
    
    CloseHandle(request.done);
}

/************************************************************************
 *     CPU state                                                        *
 *                                                                      *
 ************************************************************************/

void describe_cpu_info(STRING_BUFFER_PTR buffer, PCRUN_CONTEXT rc)
{
    PCONTEXT context = run_context_get_ctx(rc);

    sb_format(buffer,
#ifdef _M_IX86
        "\r\nEAX  %08I32X EBX   %08I32X ECX  %08I32X EDX  %08I32X"
        "\r\nESI  %08I32X EDI   %08I32X EBP  %08I32X ESP  %08I32X"
        "\r\nEIP  %08I32X FLAGS %08I32X"
        , context->Eax, context->Ebx, context->Ecx, context->Edx 
        , context->Esi, context->Edi, context->Ebp, context->Esp
        , context->Eip, context->EFlags
#elif _M_X64
        "\r\nRAX  %016I64X RBX   %016I64X RCX  %016I64X RDX  %016I64X"
        "\r\nRSI  %016I64X RDI   %016I64X RBP  %016I64X RSP  %016I64X"
        "\r\nR8   %016I64X R9    %016I64X R10  %016I64X R11  %016I64X"
        "\r\nR12  %016I64X R13   %016I64X R14  %016I64X R15  %016I64X"
        "\r\nRIP  %016I64X FLAGS %016I32X"
        , context->Rax, context->Rbx, context->Rcx, context->Rdx 
        , context->Rsi, context->Rdi, context->Rbp, context->Rsp
        , context->R8,  context->R9,  context->R10, context->R11
        , context->R12, context->R13, context->R14, context->R15
        , context->Rip, context->EFlags
#elif _M_IA64
#error No implementation.
#endif
        );
}
