#include "stdafx.h"
#include "CColorStatic.h"
#include <rect.putinto.h>
#include <color.stuff.h>
#include <atlmisc.h>

CColorStatic::~CColorStatic() = default;

CColorStatic::CColorStatic(COLORREF crColor)
    : m_crColor{crColor}
    ,  m_nAlpha{RGB_MAX_INT}
    ,  m_brBack{WTL::AtlGetStockBrush(LTGRAY_BRUSH)}
{
}

void CColorStatic::DoPaint(WTL::CDCHandle dc, CRect const& rc) const
{
    int const iSave{dc.SaveDC()};
    if (m_nAlpha < RGB_MAX_INT) {
        CRect const        rcColor{0, 0, 1, 1};
        WTL::CDC           dcColor{CreateCompatibleDC(dc)};
        WTL::CBitmap const bmColor{CreateCompatibleBitmap(dc, rcColor.Width(), rcColor.Height())};
        int const            iSave{dcColor.SaveDC()};
        BLENDFUNCTION const  blend{AC_SRC_OVER, 0, static_cast<BYTE>(m_nAlpha), 0};
        dc.SelectBrush(m_brBack);
        dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
        dcColor.SelectBitmap(bmColor);
        dcColor.FillSolidRect(rc, m_crColor);
        dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(),
                      dcColor, 0, 0, rcColor.Width(), rcColor.Height(), blend);
        dcColor.RestoreDC(iSave);
    }
    else {
        dc.FillSolidRect(rc, m_crColor);
    }
    dc.RestoreDC(iSave);
}

BOOL CColorStatic::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        CHAIN_MSG_MAP(WTL::CBufferedPaintImpl<CColorStatic>)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}
