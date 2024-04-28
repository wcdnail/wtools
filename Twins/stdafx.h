#pragma once

#ifndef _UNICODE
#  error _UNICODE is NOT defined!!!
#endif

#ifdef _WIN32
#  define  _CRT_SECURE_NO_WARNINGS
#  define WIN32_LEAN_AND_MEAN
#  define STRICT
#  define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#  define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
   
#  include <cf.windef.target.h>
   
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
#  include <atlstr.h>
#  include <atlwin.h>
#  include <atltypes.h>
#  include <atlcomcli.h>
#  include <atlcom.h>
#  include <atlsimpcoll.h>
  
#  include <atlapp.h>
#  include <atlframe.h>
#  include <atlcrack.h>
#  include <atlctrls.h>
#  include <atldlgs.h>
#  include <atlsplit.h>
#  include <atlgdi.h>
#  include <atluser.h>
#  include <atlctrlx.h>
#endif

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <functional>
#include <thread>
#include <filesystem>
#include <system_error>

#include <todo.fixme.etc.h>

extern void _NotImplementedYet(wchar_t const* caption);
