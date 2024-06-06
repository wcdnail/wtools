#include "stdafx.h"
#include "extern.run.h"
#include <dh.tracing.h>

namespace Twins
{
    namespace Extern
    {
        HRESULT Run(Item const& item, 
                    std::wstring const& directory, 
                    std::wstring const& arguments, 
                    int show/* = 1*/,
                    wchar_t const* operation/* = NULL*/)
        {
            SHELLEXECUTEINFOW info = {0};

            info.cbSize         = sizeof(info);
            info.fMask          = 0
                                | SEE_MASK_NOCLOSEPROCESS
                                | SEE_MASK_DOENVSUBST
                              //| SEE_MASK_NOASYNC 
                                | SEE_MASK_FLAG_NO_UI 
                                ;
            info.lpVerb         = operation;
            info.lpFile         = item.Path.c_str();
            info.lpDirectory    = directory.c_str();
            info.lpParameters   = arguments.c_str();
            info.nShow          = show;

            bool    rv = TRUE == ::ShellExecuteExW(&info);
            HRESULT hr = ::GetLastError();

            DH::TPrintf(0, L"RUNEXTRN: `%s (%s)` - %s - %p %p\n",
                item.Name.c_str(), item.Path.c_str(), (rv ? L"OK" : L"FAIL"),
                info.hInstApp, info.hProcess
            );

            if (nullptr != info.hProcess) {
                ::CloseHandle(info.hProcess);
            }
            return hr;
        }
    }
}
