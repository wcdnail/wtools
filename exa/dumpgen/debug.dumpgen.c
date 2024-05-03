#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdlib.h>
#include <time.h>

typedef BOOL (__stdcall *PMiniDumpWriteDump)(
    HANDLE,
    DWORD,
    HANDLE,
    MINIDUMP_TYPE,
    PMINIDUMP_EXCEPTION_INFORMATION,
    PMINIDUMP_USER_STREAM_INFORMATION,
    PMINIDUMP_CALLBACK_INFORMATION
    );

static void GetModuleDumpName(TCHAR* filename, size_t len, BOOL addTimestamp)
{
    struct tm ts = {0};
    TCHAR tsbuf[128] = {0};
    TCHAR temp[1024] = {0};

    if (GetModuleFileName(NULL, temp, _countof(temp)-192))
    {
        if (addTimestamp)
        {
            time_t tts;
            time(&tts);
            localtime_s(&ts, &tts);

            _sntprintf_s(tsbuf, _countof(tsbuf), _countof(tsbuf)
                , _T(".%02d-%02d-%04d.%02d_%02d_%02d")
                , ts.tm_mday, 1 + ts.tm_mon, 1900 + ts.tm_year
                , ts.tm_hour, ts.tm_min, ts.tm_sec
                );

            _tcscat_s(temp, _countof(temp), tsbuf);
        }

        _tcscat_s(temp, _countof(temp), _T(".mdmp"));
    }

    _tcscpy_s(filename, len, temp);
}

static BOOL CreateMiniDump(PCTSTR filename, HANDLE process
                           , PMINIDUMP_EXCEPTION_INFORMATION exceptionInfo
                           , PMINIDUMP_USER_STREAM_INFORMATION streamInfo
                           , PMINIDUMP_CALLBACK_INFORMATION callbackInfo
                           )
{
    BOOL result = FALSE;
    DWORD processId = GetProcessId(process);
    HMODULE debugHelperInst = LoadLibrary(_T("DBGHELP"));

    if (NULL != debugHelperInst)
    {
        PMiniDumpWriteDump _MiniDumpWriteDump = (PMiniDumpWriteDump)GetProcAddress(debugHelperInst, "MiniDumpWriteDump");
        if (_MiniDumpWriteDump)
        {
            HANDLE output = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
            if (INVALID_HANDLE_VALUE != output)
            {
                unsigned flags = 0;

                result = _MiniDumpWriteDump(process, processId, output, (MINIDUMP_TYPE)flags
                    , exceptionInfo, streamInfo, callbackInfo);

                FlushFileBuffers(output);
                CloseHandle(output);
            }
        }

        FreeLibrary(debugHelperInst);
    }

    return result;
}

static LONG CALLBACK _VectoredHandler(_In_  PEXCEPTION_POINTERS exceptionPointers)
{
    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo = { GetCurrentThreadId(), exceptionPointers, TRUE };
    TCHAR dumpFilename[2048] = {0};

    GetModuleDumpName(dumpFilename, _countof(dumpFilename), TRUE);
    CreateMiniDump(dumpFilename, GetCurrentProcess(), &exceptionInfo, NULL, NULL);

    _exit(exceptionPointers->ExceptionRecord->ExceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}

static PVOID prevVeh = NULL;

void InstallMyVeh()
{
     prevVeh = AddVectoredExceptionHandler(1, _VectoredHandler);
}

void RemoveMyVeh()
{
    RemoveVectoredExceptionHandler(_VectoredHandler);
}
