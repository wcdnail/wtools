#include "stdafx.h"
#include "dnd.dataobject.h"
#include "dnd.dropsource.h"
#include "dnd.enumformat.h"

namespace Dnd
{
    DataObject::DataObject(DropSource* source)
        : Source(source)
        , Formats()
        , Medium()
    {}

    DataObject::~DataObject()
    {
        for (int i = 0; i < Medium.GetSize(); ++i)
        {
            ReleaseStgMedium(Medium[i]);
            delete Medium[i];
        }

        for(int j = 0; j < Formats.GetSize(); ++j)
            delete Formats[j];
    }

    STDMETHODIMP DataObject::GetData(FORMATETC* format, STGMEDIUM* medium)
    { 
        if(format == NULL || medium == NULL)
            return E_INVALIDARG;

        medium->hGlobal = NULL;

        ATLASSERT(Medium.GetSize() == Formats.GetSize());

        for(int i=0; i < Formats.GetSize(); ++i)
        {
            if (format->tymed & Formats[i]->tymed &&
                format->dwAspect == Formats[i]->dwAspect &&
                format->cfFormat == Formats[i]->cfFormat)
            {
                CopyMedium(medium, Medium[i], Formats[i]);
                return S_OK;
            }
        }

        return DV_E_FORMATETC;
    }

    STDMETHODIMP DataObject::GetDataHere(FORMATETC* format, STGMEDIUM* medium)
    { 
        return E_NOTIMPL;
    }

    STDMETHODIMP DataObject::QueryGetData(FORMATETC* format)
    { 
        if (format == NULL)
            return E_INVALIDARG;

        // support others if needed DVASPECT_THUMBNAIL  
        // DVASPECT_ICON   
        // DVASPECT_DOCPRINT
        
        if (!(DVASPECT_CONTENT & format->dwAspect))
            return (DV_E_DVASPECT);

        HRESULT  hr = DV_E_TYMED;
        for(int i = 0; i < Formats.GetSize(); ++i)
        {
            if (format->tymed & Formats[i]->tymed)
            {
                if (format->cfFormat == Formats[i]->cfFormat)
                    return S_OK;
                else
                    hr = DV_E_CLIPFORMAT;
            }
            else
                hr = DV_E_TYMED;
        }

        return hr;
    }

    STDMETHODIMP DataObject::GetCanonicalFormatEtc(FORMATETC* fin, FORMATETC* fout)
    { 
        if (fout == NULL)
            return E_INVALIDARG;

        return DATA_S_SAMEFORMATETC;
    }

    STDMETHODIMP DataObject::SetData(FORMATETC* format, STGMEDIUM* medium, BOOL release)
    { 
        if (format == NULL || medium == NULL)
            return E_INVALIDARG;

        ATLASSERT(format->tymed == medium->tymed);
        FORMATETC* tformat = new FORMATETC;
        STGMEDIUM* tmedium = new STGMEDIUM;

        if(tformat == NULL || tmedium == NULL)
            return E_OUTOFMEMORY;

        ZeroMemory(tformat, sizeof(FORMATETC));
        ZeroMemory(tmedium, sizeof(STGMEDIUM));

        *tformat = *format;
        Formats.Add(tformat);

        if(release)
            *tmedium = *medium;
        else
            CopyMedium(tmedium, medium, format);

        Medium.Add(tmedium);
        return S_OK;
    }

    void DataObject::CopyMedium(STGMEDIUM* dest, STGMEDIUM* source, FORMATETC* format)
    {
        switch (source->tymed)
        {
        case TYMED_HGLOBAL:
            dest->hGlobal = (HGLOBAL)::OleDuplicateData(source->hGlobal, format->cfFormat, NULL);
            break;
        case TYMED_GDI:
            dest->hBitmap = (HBITMAP)::OleDuplicateData(source->hBitmap, format->cfFormat, NULL);
            break;
        case TYMED_MFPICT:
            dest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(source->hMetaFilePict,format->cfFormat, NULL);
            break;
        case TYMED_ENHMF:
            dest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(source->hEnhMetaFile,format->cfFormat, NULL);
            break;
        case TYMED_FILE:
            source->lpszFileName = (LPOLESTR)OleDuplicateData(source->lpszFileName,format->cfFormat, NULL);
            break;
        case TYMED_ISTREAM:
            dest->pstm = source->pstm;
            source->pstm->AddRef();
            break;
        case TYMED_ISTORAGE:
            dest->pstg = source->pstg;
            source->pstg->AddRef();
            break;
        case TYMED_NULL:
        default:
            break;
        }

        dest->tymed = source->tymed;
        dest->pUnkForRelease = NULL;
        if (source->pUnkForRelease != NULL)
        {
            dest->pUnkForRelease = source->pUnkForRelease;
            source->pUnkForRelease->AddRef();
        }
    }

    STDMETHODIMP DataObject::EnumFormatEtc(DWORD direction, IEnumFORMATETC** enumerator)
    { 
        if(enumerator == NULL)
            return E_POINTER;

        *enumerator=NULL;
        switch (direction)
        {
        case DATADIR_GET:
            *enumerator = new EnumFmtEtc(Formats);

            if(*enumerator == NULL)
                return E_OUTOFMEMORY;

            (*enumerator)->AddRef(); 
            break;

        case DATADIR_SET:
        default:
            return E_NOTIMPL;
            break;
        }

        return S_OK;
    }

    STDMETHODIMP DataObject::DAdvise(FORMATETC* format, DWORD advf, IAdviseSink* sink, DWORD* connection)
    { 
        return OLE_E_ADVISENOTSUPPORTED;
    }

    STDMETHODIMP DataObject::DUnadvise(DWORD connection)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE DataObject::EnumDAdvise(IEnumSTATDATA** advise)
    {
        return OLE_E_ADVISENOTSUPPORTED;
    }
}
