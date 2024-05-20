#include "stdafx.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <rect.putinto.h>

ATOM& CSpectrumImage::GetWndClassAtomRef()
{
    static ATOM gs_CSpectrumImage_Atom{0};
    return gs_CSpectrumImage_Atom;
}

CSpectrumImage::~CSpectrumImage() = default;

CSpectrumImage::CSpectrumImage(COLORREF crInit, SpectrumKind kind)
    :  m_bMsgHandled{FALSE}
    ,          m_Dib{}
    ,        m_Color{crInit}
    ,        m_ptSel{0, 0}
    , m_SpectrumKind{kind}
    ,    m_pimSlider{nullptr}
    ,     m_pstColor{nullptr}
{
}

bool CSpectrumImage::Initialize(CSpectrumSlider& imSlider, WTL::CStatic& stColor, long cx /*= SPECTRUM_CX*/, long cy /*= SPECTRUM_CY*/)
{
    if (!m_Dib.Create(cx, cy, SPECTRUM_BPP)) {
        return false;
    }
    m_pimSlider = &imSlider;
    m_pstColor = &stColor;
    return true;
}

void CSpectrumImage::SetSpectrumKind(SpectrumKind kind)
{
    if (kind < SPEC_Begin || kind > SPEC_End) {
        return ;
    }
    m_SpectrumKind = kind;

    CRect rcClient{};
    GetMyRect(rcClient);
    NotifyColorChanged(rcClient);

    if (m_pimSlider) {
        OnSliderChanged(m_pimSlider->GetPos());
    }

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
    if (SPEC_HSV_Hue != m_SpectrumKind) {
        if (m_pimSlider) {
            m_pimSlider->UpdateRaster(m_SpectrumKind, m_Color);
        }
    }
    if (m_pstColor) {
        m_pstColor->InvalidateRect(nullptr, FALSE);
    }
    InvalidateRect(nullptr, FALSE);
}

CRGBSpecRect CSpectrumImage::GetRGBSpectrumRect() const
{
    // additional_component => edit int
    auto const R{m_Color.GetRed()};
    auto const G{m_Color.GetGreen()};
    auto const B{m_Color.GetBlue()};
    CRGBSpecRect result{};
    switch (m_SpectrumKind) {
    case SPEC_RGB_Red:
        result.crLT = RGB(R, 0, 0);
        result.crRT = RGB(R, 255, 0);
        result.crLB = RGB(R, 0, 255);
        result.crRB = RGB(R, 255, 255);
        break;
    case SPEC_RGB_Green:
        result.crLT = RGB(0, G, 0);
        result.crRT = RGB(255, G, 0);
        result.crLB = RGB(0, G, 255);
        result.crRB = RGB(255, G, 255);
        break;
    case SPEC_RGB_Blue:
        result.crLT = RGB(0, 0, B);
        result.crRT = RGB(0, 255, B);
        result.crLB = RGB(255, 0, B);
        result.crRB = RGB(255, 255, B);
        break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    return result;
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
    if (m_pimSlider) {
        m_pimSlider->UpdateRaster(m_SpectrumKind, m_Color);
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

void CSpectrumImage::OnLButtonUp(UINT, CPoint)
{
    if (GetCapture() != m_hWnd) {
        return ;
    }
    ReleaseCapture();
}

void CSpectrumImage::GetMyRect(CRect& rcClient) const
{
    GetClientRect(rcClient);
    rcClient.right -= 1;
    rcClient.bottom -= 1;
}

void CSpectrumImage::NotifyColorChanged(CRect const& rcClient)
{
    auto const xScale{static_cast<float>(rcClient.Width()) / static_cast<float>(SPECTRUM_CLR_RANGE)};
    auto const yScale{static_cast<float>(rcClient.Height()) / static_cast<float>(SPECTRUM_CLR_RANGE)};
    auto const   xVal{static_cast<int>(static_cast<float>(m_ptSel.x - rcClient.left) / xScale)};
    auto const   yVal{static_cast<int>(static_cast<float>(m_ptSel.y - rcClient.top) / yScale)};
    OnColorChanged(xVal, yVal);
}

void CSpectrumImage::OnMouseMove(UINT, CPoint pt)
{
    if (GetCapture() != m_hWnd) {
        return;
    }
    CRect rcClient;
    GetMyRect(rcClient);

    pt = Rc::BoundingPoint(rcClient, pt);
    m_ptSel = pt;
    NotifyColorChanged(rcClient);

    if (!m_pstColor) {
        NMHDR nmhdr;
        nmhdr.code = NM_SPECTRUM_CLR_SEL;
        nmhdr.idFrom = GetDlgCtrlID();
        nmhdr.hwndFrom = m_hWnd;
        ::SendMessageW(GetParent(), WM_NOTIFY, (WPARAM)nmhdr.idFrom, reinterpret_cast<LPARAM>(&nmhdr));
    }
}

void CSpectrumImage::OnLButtonDown(UINT, CPoint)
{
    if (GetCapture() != m_hWnd) {
        SetCapture();
    }
}
