#pragma once

#define _wcsVAL(S)		_MK_UNICODE(# S)
#define  wcsVAL(S)		_wcsVAL(S)
#define _strVAL(S)		# S
#define  strVAL(S)		strVAL(S)

#ifdef UNICODE
#define IdentToString(A)	_wcsVAL(A)
#else
#define IdentToString(A)	_strVAL(A)
#endif

extern const TCHAR * D3DSwapEffectToString	(D3DSWAPEFFECT);
extern const TCHAR * D3DFormatToString		(D3DFORMAT);
extern const TCHAR * D3DPoolToString		(D3DPOOL);

extern void AddLogD3DPresentParameters(const D3DPRESENT_PARAMETERS&);
