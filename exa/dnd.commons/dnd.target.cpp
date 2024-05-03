#include "stdafx.h"
#include "dnd.target.h"
#include <dh.tracing.h>

namespace Dnd
{
    Target::Target() 
        : Helper(NULL)
        , Client(NULL)
    {
        Helper.CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER);
    }

    Target::~Target() 
    {}

    HRESULT Target::Register(HWND client)
    {
        HRESULT hr = ::RegisterDragDrop(client, this);

        if (SUCCEEDED(hr))
            Client = client;

        return hr;
    }

    HRESULT STDMETHODCALLTYPE Target::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        DWORD dwEffect = DROPEFFECT_COPY;

        if (Helper)
        {
            POINT ptt = { pt.x, pt.y };
            Helper->DragEnter(Client, pDataObj, &ptt, dwEffect);
        }

        *pdwEffect = dwEffect;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Target::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        DWORD dwEffect = DROPEFFECT_COPY;

        if (Helper)
        {
            POINT ptt = { pt.x, pt.y };
            Helper->DragOver(&ptt, dwEffect);
        }

        *pdwEffect = dwEffect;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Target::DragLeave(void) 
    {
        if (Helper)
            Helper->DragLeave();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Target::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) 
    {
        DWORD dwEffect = DROPEFFECT_COPY;

        if (Helper)
        {
            POINT ptt = { pt.x, pt.y };
            Helper->Drop(pDataObj, &ptt, dwEffect);
        }

        *pdwEffect = dwEffect;
        return S_OK;
    }
}
