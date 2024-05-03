#pragma once

#define UNREFERENCED_ARG(a) (a)

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4820) // 4820: '_tagOLECMDTEXT': '2' bytes padding added after data member '_tagOLECMDTEXT::rgwz'
#  pragma warning(disable: 4365) // 4365: 'argument': conversion from 'const HRESULT' to 'DWORD',
#  pragma warning(disable: 5204) // 5204: 'Font': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
#  pragma warning(disable: 5246) // 5246: 'protected: static ATL::CTrace::CategoryMap * ATL::CTrace::m_nMap': the initialization of a subobject should be wrapped in braces
#  pragma warning(disable: 4774) // 4774: 'swprintf_s' : format string expected in argument 3 is not a string literal
#  pragma warning(disable: 5267) // 5267: definition of implicit copy constructor for 'WTL::CDCT<false>' is deprecated because it has a user-provided destructor
#  pragma warning(disable: 4668) // 4668: '_WIN32_WINNT_WIN10_TH2' is not defined as a preprocessor macro, replacing with '0'
#  pragma warning(disable: 5027) // 5027: 'WTL::CPrintDialogImpl<WTL::CPrintDialog>': move assignment operator was implicitly defined as deleted
#  pragma warning(disable: 4626) // 4626: 'WTL::CPrintDialog': assignment operator was implicitly defined as deleted
#  pragma warning(disable: 4191) // 4191: 'type cast': unsafe conversion from 'FARPROC' to 'PFNCREATETRANSACTION'
#  pragma warning(disable: 4625) // 4625: 'WTL::CStaticDataInitCriticalSectionLock': copy constructor was implicitly defined as deleted
#  pragma warning(disable: 5026) // 5026: 'WTL::CStaticDataInitCriticalSectionLock': move constructor was implicitly defined as deleted
#  pragma warning(disable: 5262) // 5262: implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]] when a break statement is intentionally omitted between cases
#  pragma warning(disable: 5045) // 5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#endif

#ifndef _UNICODE
#  error _UNICODE is NOT defined!!!
#endif

#ifdef _WIN32
#  define  _CRT_SECURE_NO_WARNINGS
#  define WIN32_LEAN_AND_MEAN
#  define STRICT
#  define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#  define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#  include <SDKDDKVer.h>
#  include <windows.h>
#  include <windowsx.h>
#  include <wtypes.h>
#  include <winnls.h>
#  include <objidl.h>
#  include <oleidl.h>
#  include <shobjidl.h>
#  include <shlguid.h>
#  include <shellapi.h>
#endif

#ifdef _MSC_VER
#  pragma warning(pop)
#endif
