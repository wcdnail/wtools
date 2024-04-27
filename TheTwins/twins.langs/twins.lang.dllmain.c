#ifndef _UNICODE
#  error _UNICODE is NOT defined!!!
#endif

#include <wtypes.h>

#pragma comment(lib, "atls.lib")

/*
 *
 *
#ifdef _DEBUG
#else
#endif
 *
 *
 */

HMODULE MyInstance = NULL;

BOOL WINAPI DllMain(HMODULE module, unsigned reason, void* reserved)
{
    if (DLL_PROCESS_ATTACH == reason) {
        DisableThreadLibraryCalls(module);
        MyInstance = module;
    }
    else if (DLL_THREAD_DETACH == reason) {
        MyInstance = NULL;
    }
    return 1;
}
