#include "stdafx.h"
#include "wcdafx.api.h"

#if defined(_WIN32) && defined(_DLL)

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")

HMODULE MyInstance = nullptr;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (DLL_PROCESS_ATTACH == reason) {
        DisableThreadLibraryCalls(module);
        MyInstance = module;
    }
    else if (DLL_THREAD_DETACH == reason) {
        MyInstance = nullptr;
    }
    return TRUE;
}
#endif
