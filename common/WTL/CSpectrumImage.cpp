#include "stdafx.h"
#include "CSpectrumImage.h"
#include <DDraw.DGI/DDGDIStuff.h>

enum : int
{
    SPECTRUM_CX = 32,
    SPECTRUM_CY = 32,
};

CSpectrumImage::~CSpectrumImage() = default;
CSpectrumImage::CSpectrumImage() = default;

void CSpectrumImage::SetSpectrumKind(SpectrumKind kind)
{
    if (kind < SPEC_Begin || kind > SPEC_End) {
        return ;
    }
    m_SpectrumKind = kind;
    m_dPrevHue = -1.0;
    InvalidateRect(nullptr, FALSE);
}

BOOL CSpectrumImage::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL const bSave{m_bMsgHandled};
    BOOL const  bRet{_ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)};
    m_bMsgHandled = bSave;
    return bRet;
}

BOOL CSpectrumImage::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        MSG_WM_CREATE(OnCreate)
        MSG_WM_PAINT(OnPaint)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

int CSpectrumImage::OnCreate(LPCREATESTRUCT pCS)
{
    UNREFERENCED_PARAMETER(pCS);
    if (!m_Dib.Create(SPECTRUM_CX, SPECTRUM_CY, 32)) { // pCS->cx, pCS->cy
        return -1;
    }
    m_dHue     = 0.0;
    m_dPrevHue = -1.0;
    return 0;
}

void CSpectrumImage::OnPaint(WTL::CDCHandle dc)
{
    UNREFERENCED_PARAMETER(dc);

    if (m_dHue != m_dPrevHue) {
        switch (m_SpectrumKind) {
        case SPEC_RGB_Red:
            break;
        case SPEC_RGB_Green:
            break;
        case SPEC_RGB_Blue:
            break;
        case SPEC_HSV_Hue:
            DDraw_HSV_Hue(m_Dib, m_dHue);
            break;
        case SPEC_HSV_Saturation:
            break;
        case SPEC_HSV_Brightness:
            break;
        default: 
            break;
        }
        m_dPrevHue = m_dHue;
    }

    WTL::CPaintDC         dcPaint{m_hWnd};
    const WTL::CDCHandle dcSource{m_Dib.GetDC(dc)};
    CRect const&           rcDest{dcPaint.m_ps.rcPaint};

    dcPaint.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), 
        dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);
}
