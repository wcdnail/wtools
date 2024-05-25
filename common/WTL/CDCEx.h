#pragma once

#include <wcdafx.api.h>
#include <atlgdi.h>

struct CDCEx: WTL::CDCHandle
{
    DELETE_COPY_MOVE_OF(CDCEx);

    WCDAFX_API ~CDCEx();
    WCDAFX_API CDCEx(HWND hWnd, WTL::CRgnHandle rgn, DWORD dwFlags);

private:
    HWND m_hWnd;
};
