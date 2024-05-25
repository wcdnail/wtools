#pragma once

#include <wcdafx.api.h>
#include <atlgdi.h>

struct CDCEx: WTL::CDCHandle
{
    ~CDCEx();
    CDCEx(HWND hWnd, WTL::CRgnHandle rgn, DWORD dwFlags);

private:
    HWND m_hWnd;
};
