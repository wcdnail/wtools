#pragma once
//DEFER: compile with clang
//DEFER: Show/Hide controls
//DEFER: Improved eventing
//DEFER: resizeable dialogs

#include <string.h>

#include <vector>
#include <string>
#include <algorithm>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <regex>

#include <windows.h>
#include <CommCtrl.h>
#include <shlwapi.h>
#include <atlstr.h>
#include <atlctrls.h>

#ifdef __GNUC__
#include "gccisshit.h"
#else
#ifndef LWS_USEVISUALSTYLE
#define LWS_USEVISUALSTYLE  0x0008
#endif 
#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER       0x1501     // Set the cue banner with the lParm = LPCWSTR
#endif
#ifndef BS_DEFCOMMANDLINK
#define BS_DEFCOMMANDLINK 0x0000000F
#endif
#endif

#ifndef CTRL_TYPEDEFS
#ifdef UNICODE

#define CTRL_TYPEDEFS(ctrl) \
    typedef basic_ ## ctrl<char> ctrl; \
    typedef basic_ ## ctrl<wchar_t> w ## ctrl; \
    typedef basic_ ## ctrl<wchar_t> t ## ctrl;

#define CONTAINER_CTRL_TYPEDEFS(ctrl) \
    typedef basic_ ## ctrl<char, layout_engine> ctrl; \
    typedef basic_ ## ctrl<wchar_t, layout_engine> w ## ctrl; \
    typedef basic_ ## ctrl<wchar_t, layout_engine> t ## ctrl;

#else
#define CTRL_TYPEDEFS(ctrl) \
    typedef basic_ ## ctrl<char> ctrl; \
    typedef basic_ ## ctrl<wchar_t> w ## ctrl; \
    typedef basic_ ## ctrl<char> t ## ctrl;

#define CONTAINER_CTRL_TYPEDEFS(ctrl) \
    typedef basic_ ## ctrl<char, layout_engine> ctrl; \
    typedef basic_ ## ctrl<wchar_t, layout_engine> w ## ctrl; \
    typedef basic_ ## ctrl<char, layout_engine> t ## ctrl;

#endif // UNICODE
#endif // CTRL_TYPEDEFS


namespace rottedfrog
{
#include "errors.h"
#include "shared_handle.h"
#include "winhelper.h"
#include "abstractwindow.h"
#include "abstractcontrol.h"
#include "window.h"
#include "controlprimitives.h"
#include "subclassedcontrolbase.h"
#include "paragraph.h"
#include "syslink.h"
#include "group.h"
#include "containeroperators.h"
#include "layoutengine.h"
#include "dialog.h"
#include "containercontrol.h"
#include "buttonbase.h"
#include "button.h"
#include "checkbox.h"
#include "radiobutton.h"
#include "edit.h"
#include "combo.h"
#include "list.h"
#include "image.h"
#include "groupbox.h"
#include "tab.h"
#include "spacer.h"
#include "progressbar.h"
#include "static.h"
}
