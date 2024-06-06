#include "stdafx.h"
#include "wcdafx.api.h"

#if defined(_WIN32) && defined(_DLL)

#include "wtl.compositor/wtl.compositor.h"
struct LoadDLLResources
{
    static void OnProcessAttach()
    {
        CF::UI::Compositor::GlobalInit();
    }

    static void OnProcessDetach()
    {
        CF::UI::Compositor::GlobalFree();
    }
};

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "atls.lib")
#pragma comment(lib, "shlwapi.lib")

HMODULE MyInstance = nullptr;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (DLL_PROCESS_ATTACH == reason) {
        DisableThreadLibraryCalls(module);
        MyInstance = module;
        LoadDLLResources::OnProcessAttach();
    }
    else if (DLL_PROCESS_DETACH == reason) {
        LoadDLLResources::OnProcessDetach();
        MyInstance = nullptr;
    }
    return TRUE;
}
#endif
