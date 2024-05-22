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
CSpectrumSlider::CSpectrumSlider() = default;

bool CSpectrumSlider::Initialize(long cx /*= SPECTRUM_SLIDER_CX*/)
{
    if (!m_Dib.Create(cx, 1, 32)) {
        return false;
    }
    return true;
}

void CSpectrumSlider::UpdateRaster(SpectrumKind spKind, CColorUnion const& unColor)
{
    switch (spKind) {
    case SPEC_RGB_Red:        DDraw_RGB_Grad(m_Dib, unColor.GetRedFirst(), unColor.GetRedLast()); break;
    case SPEC_RGB_Green:      DDraw_RGB_Grad(m_Dib, unColor.GetGreenFirst(), unColor.GetGreenLast()); break;
    case SPEC_RGB_Blue:       DDraw_RGB_Grad(m_Dib, unColor.GetBlueFirst(), unColor.GetBlueLast()); break;
    case SPEC_HSV_Hue:        DDraw_HSV_H(m_Dib, 1., 1.); break;
    case SPEC_HSV_Saturation: DDraw_HSV_S(m_Dib, unColor.m_dH, unColor.m_dV * 0.01); break;
    case SPEC_HSV_Brightness: DDraw_HSV_V(m_Dib, unColor.m_dH, unColor.m_dS * 0.01); break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    m_Dib.FreeResources();
    Invalidate();
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

CRect CSpectrumSlider::GetThumbRect(CRect const& rcClient) const
{
    CRect rcThumb{rcClient};
    rcThumb.top = rcThumb.bottom - 2;
    rcThumb.left = rcClient.left;
    rcThumb.right = rcClient.right;
    return rcThumb;
}

void CSpectrumSlider::DrawPosition(WTL::CDCHandle dc, CRect const& rc) const
{
    CRect rcPos{GetThumbRect(rc)};
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

void CSpectrumSlider::OnLButtonUp(UINT, CPoint) const
{
    if (m_hWnd != GetCapture()) {
        return ;
    }
    ReleaseCapture();
}

void CSpectrumSlider::OnMouseMove(UINT nFlags, CPoint pt)
{
    if (m_hWnd != GetCapture()) {
        return;
    }
#if 0
    long const     nMin{GetRangeMin()};
    long const     nMax{GetRangeMax()};
    long const   nRange{nMax-nMin};
    LONG           nPos{0};
    float const  nScale{static_cast<float>(m_rcRaster.Height()) / static_cast<float>(nRange)};
    nPos = static_cast<LONG>(static_cast<float>(pt.y - m_rcRaster.top) / nScale);
    if (nPos < nMin) {
        nPos = nMin;
    }
    else if (nPos > nMax) {
        nPos = nMax;
    }
    SetPos(nPos, TRUE);

    NMTRBTHUMBPOSCHANGING nmPosChanged;
    nmPosChanged.dwPos = nPos;
    nmPosChanged.nReason = TB_THUMBPOSITION;
    nmPosChanged.hdr.code = TRBN_THUMBPOSCHANGING;
    nmPosChanged.hdr.idFrom = GetDlgCtrlID();
    nmPosChanged.hdr.hwndFrom = m_hWnd;
    ::SendMessageW(GetParent(), WM_NOTIFY, (WPARAM)nmPosChanged.hdr.idFrom, reinterpret_cast<LPARAM>(&nmPosChanged));

    if constexpr (false) { // ##TODO: loose capture on rect leave
        if (!m_rcRaster.IsRectEmpty() && !m_rcRaster.PtInRect(pt)) {
            OnLButtonUp(nFlags, pt);
        }
    }
#endif
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
