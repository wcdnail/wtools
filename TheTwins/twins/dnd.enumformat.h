#ifndef dnd_enumformat_h__
#define dnd_enumformat_h__

#include <com.iunknown.impl.h>
#include <objidl.h>
#include <atlsimpcoll.h>

namespace Dnd
{
    class EnumFmtEtc: public IUnknownImp<IEnumFORMATETC>
    {
    public:
        EnumFmtEtc(CSimpleArray<FORMATETC> const& arr);
        EnumFmtEtc(CSimpleArray<FORMATETC*> const& arr);

        STDMETHOD(Next)(ULONG, LPFORMATETC, ULONG FAR *);
        STDMETHOD(Skip)(ULONG);
        STDMETHOD(Reset)(void);
        STDMETHOD(Clone)(IEnumFORMATETC FAR * FAR*);

    private:
        int Index;
        CSimpleArray<FORMATETC> Formats;
    };
}

#endif // dnd_enumformat_h__
