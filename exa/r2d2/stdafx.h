#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4201 ) // nonstandard extension used : nameless struct/union
#pragma warning ( disable : 4244 ) // conversion from 'double' to 'int', possible loss of data

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>
#include <strsafe.h>
#include <CommCtrl.h>

#include <d3d9.h>
#include <xinput.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <mmsystem.h>

#include <atlbase.h>
#include <atltrace.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#define _MK_UNICODE(S)		L ## S
#define MK_UNICODE(S)		_MK_UNICODE(S)

