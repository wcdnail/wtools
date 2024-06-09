#pragma once

#define  GDIPVER 0x0110
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _ATL_APARTMENT_THREADED
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "targetver.h"

#include <windows.h>
#include <WinSock2.h>
#include <shellapi.h>

// ATL
#include <atlbase.h>
#include <atltypes.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlctl.h>
#include <atlcom.h>
#include <atlcomcli.h>
#include <atlsimpcoll.h>

// WTL
#include <atlapp.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlctrlw.h>
#include <atldlgs.h>
#include <atlmisc.h>
#include <atlprint.h>
#include <atlfind.h>
#include <atlcrack.h>
#include <atlimage.h>
#include <atltheme.h>
#include <atlgdi.h>
#include <atlddx.h>

#define UNREFERENCED_ARG(a) (a)

#undef max
#undef min
