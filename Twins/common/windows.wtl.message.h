#pragma once

#include <atlwin.h>

namespace Message
{
    template <class TBase>
    inline LRESULT RedirectTo(HWND dest, typename ATL::CWindowImplRoot<TBase> const& window)
    {
        _ATL_MSG const* messagePtr = window.GetCurrentMessage();

        if (NULL != messagePtr)
            return ::SendMessage(dest, messagePtr->message, messagePtr->wParam, messagePtr->lParam);

        return (LRESULT)-1;
    }
}
