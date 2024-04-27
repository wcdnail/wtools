#include "stdafx.h"
#include "dnd.enumformat.h"

namespace Dnd
{
    EnumFmtEtc::EnumFmtEtc(CSimpleArray<FORMATETC> const& arr)
        : Index(0)
        , Formats()
    {
        for(int i = 0; i < arr.GetSize(); ++i)
            Formats.Add(arr[i]);
    }

    EnumFmtEtc::EnumFmtEtc(CSimpleArray<FORMATETC*> const& arr)
        : Index(0)
        , Formats()
    {
        for(int i = 0; i < arr.GetSize(); ++i)
            Formats.Add(*arr[i]);
    }

    STDMETHODIMP EnumFmtEtc::Next(ULONG celt, LPFORMATETC format, ULONG FAR* fetched)
    {
        if(fetched != NULL)
            *fetched=0;

        ULONG rv = celt;

        if (celt <= 0 || format == NULL || Index >= Formats.GetSize())
            return S_FALSE;

        if (fetched == NULL && celt != 1) // fetched can be NULL only for 1 item request
            return S_FALSE;

        while (Index < Formats.GetSize() && rv > 0)
        {
            *format++ = Formats[Index++];
            --rv;
        }

        if (fetched != NULL)
            *fetched = celt - rv;

        return (rv == 0) ? S_OK : S_FALSE;
    }

    STDMETHODIMP EnumFmtEtc::Skip(ULONG celt)
    {
        if((Index + int(celt)) >= Formats.GetSize())
            return S_FALSE;

        Index += celt;
        return S_OK;
    }

    STDMETHODIMP EnumFmtEtc::Reset(void)
    {
        Index = 0;
        return S_OK;
    }

    STDMETHODIMP EnumFmtEtc::Clone(IEnumFORMATETC FAR * FAR* dest)
    {
        if (dest == NULL)
            return E_POINTER;

        EnumFmtEtc *temp = new EnumFmtEtc(Formats);
        if(temp == NULL)
            return E_OUTOFMEMORY;  	

        temp->AddRef();
        temp->Index = Index;
        *dest = temp;

        return S_OK;
    }
}
