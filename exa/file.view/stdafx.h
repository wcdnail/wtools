#pragma once

#define WINVER          0x0501
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       0x0600
#define _RICHEDIT_VER   0x0200

namespace WTL { class CAppModule; }
extern WTL::CAppModule _Module;

#include <atlbase.h>
#include <atltypes.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlx.h>
#include <atlconv.h>
#include <psapi.h>

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/cstdint.hpp>
#include <fstream>
#include <vector>
