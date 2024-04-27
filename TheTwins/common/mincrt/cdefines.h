#ifndef _crt_defines_h__
#define _crt_defines_h__

#ifdef _MSC_VER
#  define _ccdecl       __cdecl
#
#  if defined (_M_X64) || defined (_M_ARM)
#    define _cunaligned __unaligned
#    define _bits       64
#    define _ccdecl     __cdecl
#  elif defined (_M_IX86)
#    define _cunaligned 
#    define _bits       32
#  else
#    error Choosen architecture is not supported, yet.
#  endif
#
#else
#  define _ccdecl       __cdecl
#endif
#
#define _extrnc         extern "C"
#
#ifdef __cplusplus
#  define _extrnc_beg   _extrnc {
#  define _extrnc_end   }
#else
#  define _extrnc_beg
#  define _extrnc_end
#
   typedef int bool;
#
#endif

typedef void (_ccdecl *_pvfv)(void);
typedef int  (_ccdecl *_pifv)(void);
typedef void (_ccdecl *_pvfi)(int);

#endif /* _crt_defines_h__ */
