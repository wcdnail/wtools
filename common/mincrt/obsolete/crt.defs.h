#ifndef _mincrt_defs_h__
#define _mincrt_defs_h__

#ifdef _MSC_VER
#  ifdef _M_X64   
#    define _MSTDCALL
#    define _MCDECL
#  else   
#    define _MSTDCALL __stdcall
#    define _MCDECL   __cdecl
#  endif   
#else
#  define _MSTDCALL
#  define _MCDECL
#endif

#include "crt.heap.h"
#include "crt.system.allocator.h"
#include "crt.debug.assistance.h"
#include <deque>

namespace MinCrt
{
    enum Hardcoded
    {
        CppExceptionMagic = 0x19930520,
        CppExceptionCode = 0xe06d7363,
    };

    class CrtDef;

    CrtDef& Crt();

    class CrtDef
    {
    public:
        typedef void (_MCDECL *Dtor)();

        int GetReturnValue() const;
        void SetReturnValue(int val);
        CrtHeap& GetHeap();
        int AddGlobal(Dtor const& dtor);
        void DestructGlobals();
        DebugHelper& GetDbgHelper();

    private:
        friend CrtDef& Crt();

        CrtDef();
        ~CrtDef();

    private:
        typedef std::deque<Dtor, CrtAllocator<Dtor> > DtorsStorage;

        DebugHelper DbgHelper;
        DtorsStorage Dtors;
        CrtHeap Heap;
        int ReturnValue;

    private:
        CrtDef(CrtDef const&);
        CrtDef& operator = (CrtDef const&);
    };

    inline int CrtDef::GetReturnValue() const { return ReturnValue; }
    inline void CrtDef::SetReturnValue(int val) { ReturnValue = val; }
    inline CrtHeap& CrtDef::GetHeap() { return Heap; }
    inline DebugHelper& CrtDef::GetDbgHelper() { return DbgHelper; }
}

#endif /* _mincrt_defs_h__ */
