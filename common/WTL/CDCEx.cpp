#include "stdafx.h"
#include "CDCEx.h"

CDCEx::~CDCEx()
{
    if (m_hDC) {
        ReleaseDC(m_hWnd, m_hDC);
    }
}

CDCEx::CDCEx(HWND hWnd, WTL::CRgnHandle rgn, DWORD dwFlags)
    : WTL::CDCHandle{GetDCEx(hWnd, rgn, dwFlags)}
    ,         m_hWnd{hWnd}
{
    if (!m_hDC) {
        auto const code = static_cast<HRESULT>(GetLastError());
        ATLTRACE(L"GetDCEx failed! <%d>\n", code);
    }
    ATLASSERT(m_hDC != nullptr);
}
