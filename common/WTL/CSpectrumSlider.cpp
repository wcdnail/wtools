#include "stdafx.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <atlmisc.h>
#include <atlcrack.h>
#include <atlgdi.h>

ATOM& CSpectrumSlider::GetWndClassAtomRef()
{
    static ATOM gs_CSpectrumSlider_Atom{0};
    return gs_CSpectrumSlider_Atom;
}

CSpectrumSlider::~CSpectrumSlider() = default;

CSpectrumSlider::CSpectrumSlider(SpectrumKind const& nKind, CColorUnion& unColor)
    :   m_nKind{nKind}
    , m_unColor{unColor}
    ,     m_Dib{}
{
}

bool CSpectrumSlider::Initialize(long cx /*= SPECTRUM_SLIDER_CX*/)
{
    if (!m_Dib.Create(cx, 1, 32)) {
        return false;
    }
    return true;
}

void CSpectrumSlider::UpdateRaster()
{
    switch (m_nKind) {
    case SPEC_RGB_Red:        DDraw_RGB_Grad(m_Dib, m_unColor.GetRedFirst(), m_unColor.GetRedLast()); break;
    case SPEC_RGB_Green:      DDraw_RGB_Grad(m_Dib, m_unColor.GetGreenFirst(), m_unColor.GetGreenLast()); break;
    case SPEC_RGB_Blue:       DDraw_RGB_Grad(m_Dib, m_unColor.GetBlueFirst(), m_unColor.GetBlueLast()); break;
    case SPEC_HSV_Hue:        DDraw_HSV_H(m_Dib, 1., 1.); break;
    case SPEC_HSV_Saturation: DDraw_HSV_S(m_Dib, m_unColor.m_H, m_unColor.m_V * 0.01); break;
    case SPEC_HSV_Brightness: DDraw_HSV_V(m_Dib, m_unColor.m_H, m_unColor.m_S * 0.01); break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    m_Dib.FreeResources();
    Invalidate();
}

double CSpectrumSlider::GetValueAndRange(long& nPos) const
{
    switch (m_nKind) {
    case SPEC_RGB_Red:        nPos = m_unColor.GetRed(); break;
    case SPEC_RGB_Green:      nPos = m_unColor.GetGreen(); break;
    case SPEC_RGB_Blue:       nPos = m_unColor.GetBlue(); break;
    case SPEC_HSV_Hue:        nPos = m_unColor.m_H; return HSV_HUE_MAX;
    case SPEC_HSV_Saturation: nPos = m_unColor.m_S; return HSV_SAT_MAX;
    case SPEC_HSV_Brightness: nPos = m_unColor.m_V; return HSV_VAL_MAX;
    default:
        ATLASSERT(FALSE);
        break;
    }
    return RGB_MAX;
}

void CSpectrumSlider::ColorChanged() const
{
    switch (m_nKind) {
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue:
        m_unColor.RGBtoHSV();
        break;
    case SPEC_HSV_Hue:
    case SPEC_HSV_Saturation:
    case SPEC_HSV_Brightness:
        m_unColor.HSVtoRGB();
        break;
    }
    m_unColor.SetUpdated(true);
}

void CSpectrumSlider::SetValue(long nPos, long nMax) const
{
    if (nPos < 0) {
        nPos = 0;
    }
    else if (nPos > nMax) {
        nPos = nMax;
    }
    switch (m_nKind) {
    case SPEC_RGB_Red:        m_unColor.m_R = nPos; break;
    case SPEC_RGB_Green:      m_unColor.m_G = nPos; break;
    case SPEC_RGB_Blue:       m_unColor.m_B = nPos; break;
    case SPEC_HSV_Hue:        m_unColor.m_H = nPos; break;
    case SPEC_HSV_Saturation: m_unColor.m_S = nPos; break;
    case SPEC_HSV_Brightness: m_unColor.m_V = nPos; break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    ColorChanged();
}

void CSpectrumSlider::SetValueByY(long nY) const
{
    CRect rc{};
    GetClientRect(rc);
    if (nY > rc.bottom) {
        nY = rc.bottom;
    }
    else if (nY < rc.top) {
        nY = rc.top;
    }
    switch (m_nKind) {
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue: {
        auto const dScale{static_cast<double>(rc.Height()) / static_cast<double>(RGB_MAX)};
        auto const   dPos{static_cast<double>(rc.bottom - nY) / dScale};
        switch (m_nKind) {
        case SPEC_RGB_Red:   m_unColor.m_R = static_cast<int>(dPos); break;
        case SPEC_RGB_Green: m_unColor.m_G = static_cast<int>(dPos); break;
        case SPEC_RGB_Blue:  m_unColor.m_B = static_cast<int>(dPos); break;
        default:
            ATLASSERT(FALSE);
            break;
        }
        break;
    }
    case SPEC_HSV_Hue:{
        auto const dScale{static_cast<double>(rc.Height()) / static_cast<double>(HSV_HUE_MAX)};
        auto const   dPos{static_cast<double>(rc.bottom - nY) / dScale};
        m_unColor.m_H = static_cast<int>(dPos);
        break;
    }
    case SPEC_HSV_Saturation:{
        auto const dScale{static_cast<double>(rc.Height()) / static_cast<double>(HSV_SAT_MAX)};
        auto const   dPos{static_cast<double>(rc.bottom - nY) / dScale};
        m_unColor.m_S = static_cast<int>(dPos);
        break;
    }
    case SPEC_HSV_Brightness:{
        auto const dScale{static_cast<double>(rc.Height()) / static_cast<double>(HSV_VAL_MAX)};
        auto const   dPos{static_cast<double>(rc.bottom - nY) / dScale};
        m_unColor.m_V = static_cast<int>(dPos);
        break;
    }
    default:
        ATLASSERT(FALSE);
        break;
    }
    ColorChanged();
}

BOOL CSpectrumSlider::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        MSG_WM_CREATE(OnCreate)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

int CSpectrumSlider::OnCreate(LPCREATESTRUCT pCS)
{
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
}

void CSpectrumSlider::DrawRaster(WTL::CDCHandle dc, CRect const& rc) const
{
    BITMAPINFO const bmpInfo{
        {
        /* biSize;          */ sizeof(bmpInfo.bmiHeader),
        /* biWidth;         */ m_Dib.GetHeight(),
        /* biHeight;        */ m_Dib.GetWidth(),
        /* biPlanes;        */ 1,
        /* biBitCount;      */ m_Dib.GetBitCount(),
        /* biCompression;   */ 0,
        /* biSizeImage;     */ m_Dib.GetImageSize(),
        /* biXPelsPerMeter; */ 0,
        /* biYPelsPerMeter; */ 0,
        /* biClrUsed;       */ 0,
        /* biClrImportant;  */ 0
        },
        {0}
    };
    dc.StretchDIBits(rc.left, rc.top, rc.Width(), rc.Height(),
                        0, 0, m_Dib.GetHeight(), m_Dib.GetWidth(),
                        m_Dib.GetData(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
 }

void CSpectrumSlider::DrawPosition(WTL::CDCHandle dc, CRect const& rc) const
{
    constexpr long BarCY{8};
    long            nPos{0};
    double const  dRange{GetValueAndRange(nPos)};
    double const  dScale{static_cast<double>(rc.Height()) / dRange};
    long const        nY{static_cast<long>(nPos * dScale)};
    CRect          rcPos{rc.left, rc.bottom-nY-BarCY, rc.right, rc.bottom-nY};
    if (rcPos.top < rc.top) {
        rcPos.top = rc.top;
        rcPos.bottom = rcPos.top+BarCY;
    }
    dc.FrameRect(rcPos, WTL::AtlGetStockBrush(BLACK_BRUSH));
    dc.InvertRect(rcPos);
}

void CSpectrumSlider::OnPaint(WTL::CDCHandle /*dc*/) const
{
    WTL::CPaintDC const dcPaint{m_hWnd};
    WTL::CDCHandle           dc{dcPaint.m_hDC};
    CRect const          rcDest{dcPaint.m_ps.rcPaint};
    int const           iSaveDC{dc.SaveDC()};
    DrawRaster(dc, rcDest);
    DrawPosition(dc, rcDest);
    dc.RestoreDC(iSaveDC);
}

void CSpectrumSlider::NotifySend() const
{
    NMHDR nmHeader;
    nmHeader.code = NM_SLIDER_CLR_SEL;
    nmHeader.idFrom = GetDlgCtrlID();
    nmHeader.hwndFrom = m_hWnd;
    ::SendMessageW(GetParent(), WM_NOTIFY, (WPARAM)nmHeader.idFrom, reinterpret_cast<LPARAM>(&nmHeader));
}

void CSpectrumSlider::OnLButtonUp(UINT, CPoint) const
{
    if (m_hWnd != GetCapture()) {
        return ;
    }
    ReleaseCapture();
}

void CSpectrumSlider::OnMouseMove(UINT, CPoint pt)
{
    if (m_hWnd != GetCapture()) {
        return;
    }
    SetValueByY(pt.y);
    NotifySend();
    InvalidateRect(nullptr, FALSE);
}

void CSpectrumSlider::OnLButtonDown(UINT, CPoint pt)
{
    if (m_hWnd != GetCapture()) {
        SetCapture();
    }
    else {
        ReleaseCapture();
    }
}

BOOL CSpectrumSlider::OnMouseWheel(UINT nFlags, short zDelta, CPoint)
{
    long        nPos{0};
    long const  nMax{static_cast<long>(GetValueAndRange(nPos))};
    long const nStep{nFlags & MK_CONTROL ? 15 : nFlags & MK_SHIFT ? 5 : 1};
    if (zDelta < 0) {
        nPos -= nStep;
    }
    else {
        nPos += nStep;
    }
    SetValue(nPos, nMax);
    NotifySend();
    InvalidateRect(nullptr, FALSE);
    return TRUE;
}
