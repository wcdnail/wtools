#ifndef dnd_dataobject_h__
#define dnd_dataobject_h__

#include <com.iunknown.impl.h>
#include <objidl.h>

namespace Dnd
{
    class DropSource;

    class DataObject: public IUnknownImp<IDataObject>
    {
    public:
        DataObject(DropSource* source);
        void CopyMedium(STGMEDIUM* dest, STGMEDIUM* src, FORMATETC* format);

        virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC* format, STGMEDIUM* medium);
        virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC* format, STGMEDIUM* medium);
        virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* format);
        virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC* fin, FORMATETC* fout);
        virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC* format, STGMEDIUM* medium, BOOL release);
        virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD direction, IEnumFORMATETC** enumerator);
        virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC* format, DWORD advf, IAdviseSink* sink, DWORD* connection);
        virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD connection);
        virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** advise);

    protected:
        virtual ~DataObject();

    private:
        DropSource* Source;
        CSimpleArray<FORMATETC*> Formats;
        CSimpleArray<STGMEDIUM*> Medium;
    };
}

#endif // dnd_dataobject_h__
