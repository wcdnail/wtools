#pragma once

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4668) // 4668: '_WIN32_WINNT_WIN10_TH2' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#endif
#include <wtypes.h>
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct _tagSTACKFRAME64;
typedef struct _tagSTACKFRAME64* LPSTACKFRAME64;

struct _IMAGEHLP_MODULE64;
typedef struct _IMAGEHLP_MODULE64* PIMAGEHLP_MODULE64;

struct _IMAGEHLP_LINE64;
typedef struct _IMAGEHLP_LINE64* PIMAGEHLP_LINE64;

struct _IMAGEHLP_SYMBOL64;
typedef struct _IMAGEHLP_SYMBOL64* PIMAGEHLP_SYMBOL64;

typedef BOOL (CALLBACK *STACKWALK_CALLBACK)(HANDLE, HANDLE, 
                                            const PCONTEXT, 
                                            const LPSTACKFRAME64,
                                            const PIMAGEHLP_MODULE64, 
                                            const PIMAGEHLP_LINE64, 
                                            const PIMAGEHLP_SYMBOL64,
                                            DWORD64,
                                            void*);

int stack_walker(HANDLE process, HANDLE thread, CONTEXT const* scontext, 
                int skip, STACKWALK_CALLBACK cb, void* arg);

#ifdef __cplusplus
}
#endif
