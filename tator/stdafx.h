#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "targetver.h"

#include <windows.h>
#include <WinSock2.h>
#include <shellapi.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <atlapp.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlmisc.h>
#include <atlctrlw.h>
#include <atlprint.h>
#include <atlfind.h>
#include <atlcrack.h>
#include <atlimage.h>

#define UNREFERENCED_ARG(a) (a)

#undef max
#undef min
