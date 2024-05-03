#pragma once

#ifdef _WIN32
#  define  _CRT_SECURE_NO_WARNINGS
#  define WIN32_LEAN_AND_MEAN
#  define STRICT
#  define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#  define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#  define _WTL_NO_CSTRING
#  define _WTL_NO_WTYPES
   
#  include <sdkddkver.h>
   
#  include <windows.h>
#  include <windowsx.h>
#  include <wtypes.h>
#  include <winnls.h>
#  include <objidl.h>
#  include <oleidl.h>
#  include <shobjidl.h>
#  include <shlguid.h>
#  include <shellapi.h>
#  include <comdef.h>

#  include <atlbase.h>
#  include <atltypes.h>
#  include <atlstr.h>
#  include <atlwin.h>
#  include <atltypes.h>
#  include <atlcomcli.h>
#  include <atlcom.h>
#  include <atlsimpcoll.h>

#  include <atlapp.h>
#  include <atlmisc.h>
#  include <atlframe.h>
#  include <atlcrack.h>
#  include <atlctrls.h>
#  include <atldlgs.h>
#  include <atlsplit.h>
#  include <atlgdi.h>
#  include <atluser.h>
#  include <atlctrlx.h>
#  include <atlribbon.h>
#endif
