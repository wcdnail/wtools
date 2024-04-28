#include "stdafx.h"
#include "dnd.droptarget.h"
#include "panel.view.dnd.h"
#include <shlguid.h>

namespace Dnd
{
    DropTarget::DropTarget(Twins::DragnDropHelper& owner)
        : Owner(owner)
        , AllowDrop(false)
        , Helper(NULL)
        , SupportedFormat(NULL)
    { 
        Helper.CoCreateInstance(CLSID_DragDropHelper);
    }

    DropTarget::~DropTarget()
    {}

    void DropTarget::AddSuportedFormat(FORMATETC& ftetc) 
    {
        Formats.Add(ftetc); 
    }

    bool DropTarget::QueryDrop(DWORD state, POINTL const& pt, LPDWORD effect)
    {  
        DWORD okEffects = *effect; 

        if (!AllowDrop)
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        if (Owner.IsMyPoint(pt))
        {
            *effect = DROPEFFECT_NONE;
            return false;
        }

        //CTRL+SHIFT  -- DROPEFFECT_LINK
        //CTRL        -- DROPEFFECT_COPY
        //SHIFT       -- DROPEFFECT_MOVE
        //no modifier -- DROPEFFECT_MOVE or whatever is allowed by src

        *effect = (state & MK_CONTROL) ?
            ( (state & MK_SHIFT) ? DROPEFFECT_LINK : DROPEFFECT_COPY ):
            ( (state & MK_SHIFT) ? DROPEFFECT_MOVE : 0 );

        if (*effect == 0) 
        {
            if (DROPEFFECT_COPY & okEffects)
                *effect = DROPEFFECT_COPY;

            else if (DROPEFFECT_MOVE & okEffects)
                *effect = DROPEFFECT_MOVE; 

            else if (DROPEFFECT_LINK & okEffects)
                *effect = DROPEFFECT_LINK; 

            else 
                *effect = DROPEFFECT_NONE;
        } 
        else
        {
            if (!(*effect & okEffects))
                *effect = DROPEFFECT_NONE;
        }  

        return (DROPEFFECT_NONE == *effect) ? false : true;
    }   

    HRESULT STDMETHODCALLTYPE DropTarget::DragEnter(IDataObject* object, DWORD state, POINTL pt, DWORD* effect)
    {
        if(object == NULL)
            return E_INVALIDARG;

        if (Helper)
            Helper->DragEnter(Owner.GetHwnd(), object, (LPPOINT)&pt, *effect);

        //IEnumFORMATETC* pEnum;
        //object->EnumFormatEtc(DATADIR_GET,&pEnum);
        //FORMATETC ftm;
        //for()
        //pEnum->Next(1,&ftm,0);
        //pEnum->Release();

        SupportedFormat = NULL;
        for(int i =0; i<Formats.GetSize(); ++i)
        {
            AllowDrop = (object->QueryGetData(&Formats[i]) == S_OK)?true:false;
            if(AllowDrop)
            {
                SupportedFormat = &Formats[i];
                break;
            }
        }

        QueryDrop(state, pt, effect);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DropTarget::DragOver(DWORD state, POINTL pt, DWORD* effect)
    {
        if(Helper)
            Helper->DragOver((LPPOINT)&pt, *effect);

        QueryDrop(state, pt, effect);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DropTarget::DragLeave()
    {
        if (Helper)
            Helper->DragLeave();

        AllowDrop = false;
        SupportedFormat = NULL;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DropTarget::Drop(IDataObject* object, DWORD state, POINTL pt, DWORD* effect)
    {
        if (object == NULL)
            return E_INVALIDARG;	

        if (Helper)
            Helper->Drop(object, (LPPOINT)&pt, *effect);

        if (QueryDrop(state, pt, effect))
        {
            if (AllowDrop && SupportedFormat != NULL)
            {
                STGMEDIUM medium;
                if(object->GetData(SupportedFormat, &medium) == S_OK)
                {
                    CPoint pn(pt.x, pt.y);
                    Owner.OnStop(pn, medium, SupportedFormat, effect);
                    ::ReleaseStgMedium(&medium);
                }
            }
        }

        AllowDrop=false;
        *effect = DROPEFFECT_NONE;
        SupportedFormat = NULL;

        return S_OK;
    }
}
