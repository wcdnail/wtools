#include "stdafx.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <atlmisc.h>
#include <atlcrack.h>
#include <atlgdi.h>

CSpectrumSlider::~CSpectrumSlider() = default;

CSpectrumSlider::CSpectrumSlider(SpectrumKind const& nKind, CColorUnion& unColor)
    :   m_nKind{nKind}
    , m_unColor{unColor}
{
}

HRESULT CSpectrumSlider::PreCreateWindow()
{
    static ATOM gs_CSpectrumSlider_Atom{0};
    return CCustomControl::PreCreateWindowImpl(gs_CSpectrumSlider_Atom, GetWndClassInfo());
}

bool CSpectrumSlider::Initialize(long cx, HBRUSH brBackground, HCURSOR hCursor)
{
    return CDDCtrl::Initialize(cx, 1, SPECTRUM_BPP, brBackground, hCursor);
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
    CDIBitmap bmVert;
    bmVert.Create(m_Dib.GetHeight(), m_Dib.GetWidth(), m_Dib.GetBitCount());
    memcpy(bmVert.GetData(), m_Dib.GetData(), m_Dib.GetDataSize());
    bmVert.Swap(m_bmVert);
    InvalidateRect(nullptr, FALSE);
}

double CSpectrumSlider::GetValueAndRange(long& nPos) const
{
    switch (m_nKind) {
    case SPEC_RGB_Red:        nPos = m_unColor.GetRed(); break;
    case SPEC_RGB_Green:      nPos = m_unColor.GetGreen(); break;
    case SPEC_RGB_Blue:       nPos = m_unColor.GetBlue(); break;
    case SPEC_HSV_Hue:        nPos = m_unColor.m_H; return HSV_HUE_MAX;
    case SPEC_HSV_Saturation: nPos = m_unColor.m_S; return HSV_100PERC;
    case SPEC_HSV_Brightness: nPos = m_unColor.m_V; return HSV_100PERC;
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
        auto const dScale{static_cast<double>(rc.Height()) / static_cast<double>(HSV_100PERC)};
        auto const   dPos{static_cast<double>(rc.bottom - nY) / dScale};
        m_unColor.m_S = static_cast<int>(dPos);
        break;
    }
    case SPEC_HSV_Brightness:{
        auto const dScale{static_cast<double>(rc.Height()) / static_cast<double>(HSV_100PERC)};
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

UINT CSpectrumSlider::OnGetDlgCode(LPMSG) const
{
    return DLGC_WANTARROWS;
}

BOOL CSpectrumSlider::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_GETDLGCODE(OnGetDlgCode)
        MSG_WM_KEYDOWN(OnKeyDown)
        CHAIN_MSG_MAP(CDDCtrl)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

void CSpectrumSlider::DrawPosition(WTL::CDCHandle dc, CRect const& rc) const
{
    constexpr long BarCY{4};
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

void CSpectrumSlider::DoPaint(WTL::CDCHandle dc, CRect const& rc)
{
    int const iSave{dc.SaveDC()};
    dc.SetStretchBltMode(COLORONCOLOR);
    DrawRaster(dc, rc, m_unColor.m_A, m_bmVert);
    DrawPosition(dc, rc);
    dc.RestoreDC(iSave);
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
    NotifySend(NM_SLIDER_CLR_SEL);
    InvalidateRect(nullptr, FALSE);
    return TRUE;
}

void CSpectrumSlider::DoNotifyMouseOver(CRect const& /*rc*/, CPoint pt)
{
    SetValueByY(pt.y);
    NotifySend(NM_SLIDER_CLR_SEL);
    InvalidateRect(nullptr, FALSE);
}

void CSpectrumSlider::OnKeyDown(UINT nChar, UINT, UINT nFlags)
{
    switch (nChar) {
    case VK_LEFT:
    case VK_UP:    OnMouseWheel(0,  1, {}); break;
    case VK_RIGHT:
    case VK_DOWN:  OnMouseWheel(0, -1, {}); break;
    }
}
