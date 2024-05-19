#include "stdafx.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>

CSpectrumImage::~CSpectrumImage() = default;

CSpectrumImage::CSpectrumImage()
    :  m_bMsgHandled{FALSE}
    ,          m_Dib{}
    ,        m_Color{0., 0., 100.}
    , m_SpectrumKind{SPEC_HSV_Hue}
    ,    m_pimSlider{nullptr}
{
}

bool CSpectrumImage::Initialize(CSpectrumSlider& imSlider, long cx /*= SPECTRUM_CX*/, long cy /*= SPECTRUM_CY*/)
{
    if (!m_Dib.Create(cx, cy, SPECTRUM_BPP)) {
        return false;
    }
    m_pimSlider = &imSlider;
    return true;
}

void CSpectrumImage::SetSpectrumKind(SpectrumKind kind)
{
    if (kind < SPEC_Begin || kind > SPEC_End) {
        return ;
    }
    m_SpectrumKind = kind;
    InvalidateRect(nullptr, FALSE);
}

void CSpectrumImage::OnSliderChanged(long nPos)
{
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:          break;
    case SPEC_RGB_Green:        break;
    case SPEC_RGB_Blue:         break;
    case SPEC_HSV_Hue:          m_Color.SetHue((255 - static_cast<double>(nPos)) / 255.0 * 359.0); break;
    case SPEC_HSV_Saturation:   break;
    case SPEC_HSV_Brightness:   break;
    default: break;
    }
    InvalidateRect(nullptr, FALSE);
}

ATOM& CSpectrumImage::GetWndClassAtomRef()
{
    static ATOM gs_CSpectrumImage_Atom{0};
    return gs_CSpectrumImage_Atom;
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
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
}

void CSpectrumImage::UpdateRaster()
{
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:          break;
    case SPEC_RGB_Green:        break;
    case SPEC_RGB_Blue:         break;
    case SPEC_HSV_Hue:          DDraw_HSV_Hue(m_Dib, m_Color.m_dH); break;
    case SPEC_HSV_Saturation:   break;
    case SPEC_HSV_Brightness:   break;
    default: break;
    }
    if (m_pimSlider) {
        m_pimSlider->UpdateRaster(m_SpectrumKind);
    }
    m_Dib.FreeResources();
}

void CSpectrumImage::OnPaint(WTL::CDCHandle dc)
{
    UNREFERENCED_PARAMETER(dc);

    if (m_Color.IsUpdated()) {
        UpdateRaster();
        m_Color.SetUpdated(false);
    }

    WTL::CPaintDC         dcPaint{m_hWnd};
    const WTL::CDCHandle dcSource{m_Dib.GetDC(dc)};
    CRect const&           rcDest{dcPaint.m_ps.rcPaint};

    dcPaint.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), 
        dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);
}
