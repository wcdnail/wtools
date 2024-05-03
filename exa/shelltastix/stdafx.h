#pragma once

#define _SECURE_ATL                             1
#define _USE_POSIX_BY_DEFAULT                   1

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include "targetver.h"

// BOOST
#include <boost/array.hpp> 
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/exception/diagnostic_information.hpp>

// Stuff...
#include <windows.h>
#include <windowsx.h>
#include <wtypes.h>
#include <dwmapi.h>

extern void AtlThrow(HRESULT hr);

// ATL
#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>

// WTL
#include <atlapp.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlsplit.h>

// WTL & more...
#include <ShellAPI.h>
#include <atlctrlx.h>

// STD
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

