#ifndef _UnI_Afx_Core_Windows_Stack_Walker_h__
#define _UnI_Afx_Core_Windows_Stack_Walker_h__

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32

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

#endif

#ifdef __cplusplus
}
#endif

#endif /* _UnI_Afx_Core_Windows_Stack_Walker_h__ */
