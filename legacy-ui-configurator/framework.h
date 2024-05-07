#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "targetver.h"
#define _ATL_APARTMENT_THREADED
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS          // some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
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

#include <winrt/base.h>

#include <CommCtrl.h>
#include <shellapi.h>
#include <prsht.h>

#include <atlcomcli.h>
#include <combaseapi.h>
#include <ShObjIdl_core.h>
#include <comdef.h>
#include <comip.h>
#include <comutil.h>

#include <gdiplus.h>

#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <utility>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <list>
#include <map>
#include <set>
