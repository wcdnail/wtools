#include "stdafx.h"
#include "CSpectrumImage.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <rect.putinto.h>
#include <atlcrack.h>

ATOM& CSpectrumImage::GetWndClassAtomRef()
{
    static ATOM gs_CSpectrumImage_Atom{0};
    return gs_CSpectrumImage_Atom;
}

CSpectrumImage::~CSpectrumImage() = default;

CSpectrumImage::CSpectrumImage(COLORREF crInit, SpectrumKind kind)
    :          m_Dib{}
    ,        m_Color{crInit}
    , m_SpectrumKind{kind}
{
}

bool CSpectrumImage::Initialize(long cx /*= SPECTRUM_CX*/, long cy /*= SPECTRUM_CY*/)
{
    if (!m_Dib.Create(cx, cy, SPECTRUM_BPP)) {
        return false;
    }
    return true;
}

void CSpectrumImage::SetSpectrumKind(SpectrumKind kind)
{
    if (kind < SPEC_Begin || kind > SPEC_End) {
        return ;
    }
    m_SpectrumKind = kind;
    CRect rc{};
    GetClientRect(rc);
    CPoint const pt{m_Color.GetColorPoint(m_SpectrumKind, rc)};
    NotifyColorChanged(rc, pt);
    m_Color.SetUpdated(true);
    InvalidateRect(nullptr, FALSE);
}

BOOL CSpectrumImage::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) {
    case 0:
        MSG_WM_CREATE(OnCreate)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
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
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue:         DDraw_RGB(m_Dib, m_Color.GetRGBSpectrumRect(m_SpectrumKind)); break;
    case SPEC_HSV_Hue:          DDraw_HSV_Hue(m_Dib, m_Color.m_dH); break;
    case SPEC_HSV_Saturation:   DDraw_HSV_Sat(m_Dib, m_Color.m_dS / HSV_SAT_MAX); break;
    case SPEC_HSV_Brightness:   DDraw_HSV_Val(m_Dib, m_Color.m_dV / HSV_VAL_MAX); break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    m_Dib.FreeResources();
}

void CSpectrumImage::OnColorChanged(double xPos, double yPos)
{
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:          m_Color.SetRGB(m_Color.GetRed(), static_cast<int>(xPos), static_cast<int>(yPos)); break;
    case SPEC_RGB_Green:        m_Color.SetRGB(static_cast<int>(xPos), m_Color.GetGreen(), static_cast<int>(yPos)); break;
    case SPEC_RGB_Blue:         m_Color.SetRGB(static_cast<int>(yPos), static_cast<int>(xPos), m_Color.GetBlue()); break;
    case SPEC_HSV_Hue:          m_Color.SetHSV(m_Color.m_dH, xPos / RGB_MAX * HSV_SAT_MAX, (RGB_MAX - yPos) / RGB_MAX * HSV_VAL_MAX); break;
    case SPEC_HSV_Saturation:   m_Color.SetHSV(xPos / RGB_MAX * HSV_HUE_MAX, m_Color.m_dS, (RGB_MAX - yPos) / RGB_MAX * HSV_VAL_MAX); break;
    case SPEC_HSV_Brightness:   m_Color.SetHSV(xPos / RGB_MAX * HSV_HUE_MAX, (RGB_MAX - yPos) / RGB_MAX * HSV_SAT_MAX, m_Color.m_dV); break;
    default: 
        ATLASSERT(FALSE);
        break;
    }
    InvalidateRect(nullptr, FALSE);
}

void CSpectrumImage::OnPaint(WTL::CDCHandle /*dc*/)
{
    if (m_Color.IsUpdated()) {
        UpdateRaster();
        m_Color.SetUpdated(false);
    }
    WTL::CPaintDC const dcPaint{m_hWnd};
    WTL::CDCHandle           dc{dcPaint.m_hDC};
    HDC const          dcSource{m_Dib.GetDC(dc)};
    CRect const          rcDest{dcPaint.m_ps.rcPaint};
    dc.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
                  dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);
    DrawMarker(dc, rcDest);
}

void CSpectrumImage::DrawMarker(WTL::CDCHandle dc, CRect const& rcDest)
{
    CPoint const pt{m_Color.GetColorPoint(m_SpectrumKind, rcDest)};
    CRect const  rc{pt.x-1, pt.y-1, pt.x+1, pt.y+1};
    dc.InvertRect(CRect(rc.left - 5, rc.top, rc.left, rc.bottom));
    dc.InvertRect(CRect(rc.left, rc.top - 5, rc.right, rc.top));
    dc.InvertRect(CRect(rc.right, rc.top, rc.right + 5, rc.bottom));
    dc.InvertRect(CRect(rc.left, rc.bottom, rc.right, rc.bottom + 5));
}

void CSpectrumImage::OnLButtonUp(UINT, CPoint) const
{
    if (GetCapture() != m_hWnd) {
        return ;
    }
    ReleaseCapture();
}

void CSpectrumImage::NotifySend() const
{
    NMHDR nmHeader;
    nmHeader.code = NM_SPECTRUM_CLR_SEL;
    nmHeader.idFrom = GetDlgCtrlID();
    nmHeader.hwndFrom = m_hWnd;
    ::SendMessageW(GetParent(), WM_NOTIFY, (WPARAM)nmHeader.idFrom, reinterpret_cast<LPARAM>(&nmHeader));
}

void CSpectrumImage::NotifyColorChanged(CRect const& rc, CPoint pt)
{
    auto const xScale{static_cast<double>(rc.Width()) / RGB_MAX};
    auto const yScale{static_cast<double>(rc.Height()) / RGB_MAX};
    auto         xVal{static_cast<double>(pt.x - rc.left) / xScale};
    auto         yVal{static_cast<double>(pt.y - rc.top) / yScale};
    if (xVal < 0) {
        xVal = 0;
    }
    else if (xVal > RGB_MAX) {
        xVal = RGB_MAX;
    }
    if (yVal < 0) {
        yVal = 0;
    }
    else if (yVal > RGB_MAX) {
        yVal = RGB_MAX;
    }
    OnColorChanged(xVal, yVal);
    NotifySend();
}

void CSpectrumImage::OnMouseMove(UINT, CPoint pt)
{
    if (GetCapture() != m_hWnd) {
        return;
    }
    CRect rcClient;
    GetClientRect(rcClient);
    NotifyColorChanged(rcClient, pt);
}

void CSpectrumImage::OnLButtonDown(UINT, CPoint)
{
    if (m_hWnd != GetCapture()) {
        SetCapture();
    }
    else {
        ReleaseCapture();
    }
}
