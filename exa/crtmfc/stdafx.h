#pragma once

#define _SECURE_ATL 1
#define VC_EXTRALEAN                        // Исключите редко используемые компоненты из заголовков Windows
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // некоторые конструкторы CString будут явными
#define _AFX_ALL_WARNINGS                   // отключает функцию скрытия некоторых общих и часто пропускаемых предупреждений MFC

#include "targetver.h"
#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <afxodlgs.h>
#include <afxdisp.h>
#include <afxdtctl.h>
#include <afxcmn.h>
#include <afxcontrolbars.h>
#include <afxsock.h>


