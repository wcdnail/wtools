#pragma once

#include <wcdafx.api.h>
#include <atlwin.h>

struct CCustomControl: ATL::CWindowImpl<CCustomControl>
{
    using WndSuper = ATL::CWindowImpl<CCustomControl>;

    DELETE_MOVE_OF(CCustomControl);

    WCDAFX_API ~CCustomControl() override;
    WCDAFX_API virtual HRESULT PreCreateWindow() = 0;

protected:
    WCDAFX_API CCustomControl();
    WCDAFX_API HRESULT PreCreateWindowImpl(ATOM& rAtom, ATL::CWndClassInfo const& clsInfo);
};
