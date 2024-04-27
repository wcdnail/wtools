#include "stdafx.h"
#include "dnd.dropsource.h"

namespace Dnd
{
    DropSource::~DropSource() 
    {}

    DropSource::DropSource()
        : IsDropped(false) 
    {}

    STDMETHODIMP DropSource::QueryContinueDrag(BOOL escape, DWORD state)
    {
        if (escape)
            return DRAGDROP_S_CANCEL;

        if (!(state & (MK_LBUTTON | MK_RBUTTON)))
        {
            IsDropped = true;
            return DRAGDROP_S_DROP;
        }

        return S_OK;

    }

    STDMETHODIMP DropSource::GiveFeedback(DWORD effect)
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
}
