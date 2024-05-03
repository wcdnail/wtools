#include "stdafx.h"

#if 0
#include "dnd.dragsource.helper.h"
#include <shlguid.h>

namespace Dnd
{
    DragSourceHelper::DragSourceHelper()
    {
        if (FAILED(::CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDragSourceHelper, (void**)&pDragSourceHelper)))
            pDragSourceHelper = NULL;
    }

    DragSourceHelper::~DragSourceHelper()
    {
        if( pDragSourceHelper!= NULL )
        {
            pDragSourceHelper->Release();
            pDragSourceHelper=NULL;
        }
    }

    HRESULT DragSourceHelper::InitializeFromBitmap(HBITMAP hBitmap, POINT& pt, RECT& rc, IDataObject* pDataObject, COLORREF crColorKey)
    {
        if (pDragSourceHelper == NULL)
            return E_FAIL;

        SHDRAGIMAGE di;
        BITMAP bm;
        ::GetObject(hBitmap, sizeof(bm), &bm);
        di.sizeDragImage.cx = bm.bmWidth;
        di.sizeDragImage.cy = bm.bmHeight;
        di.hbmpDragImage = hBitmap;
        di.crColorKey = crColorKey; 
        di.ptOffset.x = pt.x - rc.left;
        di.ptOffset.y = pt.y - rc.top;

        return pDragSourceHelper->InitializeFromBitmap(&di, pDataObject);
    }

    HRESULT DragSourceHelper::InitializeFromWindow(HWND hwnd, POINT& pt, IDataObject* pDataObject)
    {		
        if(pDragSourceHelper == NULL)
            return E_FAIL;

        return pDragSourceHelper->InitializeFromWindow(hwnd, &pt, pDataObject);
    }
}
#endif
