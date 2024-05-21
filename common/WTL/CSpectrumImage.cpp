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
    ,        m_ptSel{0, 0}
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
    CRect rcClient{};
    GetClientRect(rcClient);
    NotifyColorChanged(rcClient);
    m_Color.SetUpdated(true);
    InvalidateRect(nullptr, FALSE);
}

void CSpectrumImage::OnSliderChanged(long nPos)
{
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:          m_Color.SetRGB(255 - nPos, m_Color.GetGreen(), m_Color.GetBlue()); break;
    case SPEC_RGB_Green:        m_Color.SetRGB(m_Color.GetRed(), 255 - nPos, m_Color.GetBlue()); break;
    case SPEC_RGB_Blue:         m_Color.SetRGB(m_Color.GetRed(), m_Color.GetGreen(), 255 - nPos); break;
    case SPEC_HSV_Hue:          m_Color.SetHSV((255 - static_cast<double>(nPos)) / 255.0 * 359.0, m_Color.m_dS, m_Color.m_dV); break;
    case SPEC_HSV_Saturation:   m_Color.SetHSV(m_Color.m_dH, (255 - static_cast<double>(nPos)) / 255.0 * 100.0, m_Color.m_dV); break;
    case SPEC_HSV_Brightness:   m_Color.SetHSV(m_Color.m_dH, m_Color.m_dS, (255 - static_cast<double>(nPos)) / 255.0 * 100.0); break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    InvalidateRect(nullptr, FALSE);
    NotifySend();
}

void CSpectrumImage::OnColorChanged(long xPos, long yPos)
{
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:          m_Color.SetRGB(m_Color.GetRed(), xPos, yPos); break;
    case SPEC_RGB_Green:        m_Color.SetRGB(xPos, m_Color.GetGreen(), yPos); break;
    case SPEC_RGB_Blue:         m_Color.SetRGB(xPos, yPos, m_Color.GetBlue()); break;
    case SPEC_HSV_Hue:          m_Color.SetHSV(m_Color.m_dH, xPos / 255.0 * 100.0, (255 - yPos) / 255.0 * 100.0); break;
    case SPEC_HSV_Saturation:   m_Color.SetHSV(xPos / 255.0 * 359.0, m_Color.m_dS, (255 - yPos) / 255.0 * 100.0); break;
    case SPEC_HSV_Brightness:   m_Color.SetHSV(xPos / 255.0 * 359.0, (255 - yPos) / 255.0 * 100.0, m_Color.m_dV); break;
    default: 
        ATLASSERT(FALSE);
        break;
    }
    InvalidateRect(nullptr, FALSE);
}

CRGBSpecRect CSpectrumImage::GetRGBSpectrumRect() const
{
    auto const    R{m_Color.GetRed()};
    auto const    G{m_Color.GetGreen()};
    auto const    B{m_Color.GetBlue()};
    CRGBSpecRect rv{};
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:
        rv.crLT = RGB(R, 0, 0);
        rv.crRT = RGB(R, 255, 0);
        rv.crLB = RGB(R, 0, 255);
        rv.crRB = RGB(R, 255, 255);
        break;
    case SPEC_RGB_Green:
        rv.crLT = RGB(0, G, 0);
        rv.crRT = RGB(255, G, 0);
        rv.crLB = RGB(0, G, 255);
        rv.crRB = RGB(255, G, 255);
        break;
    case SPEC_RGB_Blue:
        rv.crLT = RGB(0, 0, B);
        rv.crRT = RGB(0, 255, B);
        rv.crLB = RGB(255, 0, B);
        rv.crRB = RGB(255, 255, B);
        break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    return rv;
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
    case SPEC_RGB_Blue:         DDraw_RGB(m_Dib, GetRGBSpectrumRect()); break;
    case SPEC_HSV_Hue:          DDraw_HSV_Hue(m_Dib, m_Color.m_dH); break;
    case SPEC_HSV_Saturation:   DDraw_HSV_Sat(m_Dib, m_Color.m_dS * 0.01); break;
    case SPEC_HSV_Brightness:   DDraw_HSV_Sat(m_Dib, m_Color.m_dV * 0.01); break;
    default:
        ATLASSERT(FALSE);
        break;
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
    CRect const            rcDest{dcPaint.m_ps.rcPaint};

    dc.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
                  dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);

    DrawMarker(dc);
}

void CSpectrumImage::DrawMarker(WTL::CDCHandle dc) const
{
    CRect const rc{m_ptSel.x, m_ptSel.y, m_ptSel.x+1, m_ptSel.y+1};
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

void CSpectrumImage::NotifyColorChanged(CRect const& rcClient)
{
    auto const xScale{static_cast<float>(rcClient.Width()) / static_cast<float>(SPECTRUM_CLR_RANGE)};
    auto const yScale{static_cast<float>(rcClient.Height()) / static_cast<float>(SPECTRUM_CLR_RANGE)};
    auto const   xVal{static_cast<int>(static_cast<float>(m_ptSel.x - rcClient.left) / xScale)};
    auto const   yVal{static_cast<int>(static_cast<float>(m_ptSel.y - rcClient.top) / yScale)};
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

    pt = Rc::BoundingPoint(rcClient, pt);
    m_ptSel = pt;
    NotifyColorChanged(rcClient);
}

void CSpectrumImage::OnLButtonDown(UINT, CPoint)
{
    if (GetCapture() != m_hWnd) {
        SetCapture();
    }
}
