#include "stdafx.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>

CSpectrumImage::~CSpectrumImage() = default;

CSpectrumImage::CSpectrumImage()
    :  m_bMsgHandled{FALSE}
    ,          m_Dib{}
    ,        m_Color{0., 0., 100.}
    ,        m_ptSel{0, 0}
    ,     m_bCapture{false}
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

void CSpectrumImage::OnColorChanged(long xPos, long yPos)
{
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:          m_Color.SetRGB(m_Color.GetRed(), xPos, yPos); break;
    case SPEC_RGB_Green:        m_Color.SetRGB(xPos, m_Color.GetGreen(), yPos); break;
    case SPEC_RGB_Blue:         m_Color.SetRGB(xPos, yPos, m_Color.GetBlue()); break;
    case SPEC_HSV_Hue:          m_Color.SetHSV(m_Color.m_dH, xPos / 255.0 * 100.0, (255 - yPos) / 255.0 * 100.0); break;
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
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_HSCROLL(OnWMScroll)
        MSG_WM_VSCROLL(OnWMScroll)
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

void CSpectrumImage::OnPaint(WTL::CDCHandle /*dc*/)
{
    if (m_Color.IsUpdated()) {
        UpdateRaster();
        m_Color.SetUpdated(false);
    }
    WTL::CPaintDC const   dcPaint{m_hWnd};
    WTL::CDCHandle             dc{dcPaint.m_hDC};
    const WTL::CDCHandle dcSource{m_Dib.GetDC(dc)};
    CRect const&           rcDest{dcPaint.m_ps.rcPaint};

    dc.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
        dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);

    DrawMarker(dc);
}

void CSpectrumImage::DrawMarker(WTL::CDCHandle dc)
{
    CRect const rc{m_ptSel.x, m_ptSel.y, m_ptSel.x+1, m_ptSel.y+1};
    dc.InvertRect(CRect(rc.left - 4, rc.top, rc.left, rc.bottom));
    dc.InvertRect(CRect(rc.left, rc.top - 4, rc.right, rc.top));
    dc.InvertRect(CRect(rc.right, rc.top, rc.right + 4, rc.bottom));
    dc.InvertRect(CRect(rc.left, rc.bottom, rc.right, rc.bottom + 4));
}

void CSpectrumImage::OnLButtonUp(UINT, CPoint)
{
    if (!m_bCapture) {
        return ;
    }
    ReleaseCapture();
    m_bCapture = false;
}

void CSpectrumImage::OnMouseMove(UINT, CPoint point)
{
    if (!m_bCapture) {
        return;
    }
    CRect rcClient;
    GetClientRect(rcClient);
    auto const xScale{static_cast<float>(rcClient.Width()) / static_cast<float>(SPECTRUM_CLR_RANGE)};
    auto const yScale{static_cast<float>(rcClient.Height()) / static_cast<float>(SPECTRUM_CLR_RANGE)};
    auto const   xVal{static_cast<int>(static_cast<float>(point.x - rcClient.left) / xScale)};
    auto const   yVal{static_cast<int>(static_cast<float>(point.y - rcClient.top) / yScale)};
    m_ptSel = point;
    OnColorChanged(xVal, yVal);

    NMHDR nmhdr;
    nmhdr.code = NM_SPECTRUM_CLR_SEL;
    nmhdr.idFrom = GetDlgCtrlID();
    nmhdr.hwndFrom = m_hWnd;
    ::SendMessageW(GetParent(), WM_NOTIFY, (WPARAM)nmhdr.idFrom, reinterpret_cast<LPARAM>(&nmhdr));
}

void CSpectrumImage::OnLButtonDown(UINT, CPoint point)
{
    SetFocus();
    SetCapture();
    m_bCapture = true;
}

void CSpectrumImage::OnWMScroll(UINT nSBCode, UINT nPos, WTL::CScrollBar pScrollBar)
{
    ATLTRACE(L">>> %d %d %p\n", nSBCode, nPos, pScrollBar.m_hWnd);
}
