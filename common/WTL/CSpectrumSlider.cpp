//
// https://www.codeproject.com/articles/8985/customizing-the-appearance-of-csliderctrl-using-cu
// Mike O'Neill's Customizing the Appearance of CSliderCtrl Using Custom Draw
// adopted to WTL from MFC
//
#include "stdafx.h"
#include "CSpectrumSlider.h"
#include "CSpectrumImage.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <atlmisc.h>

CSpectrumSlider::~CSpectrumSlider() = default;

CSpectrumSlider::CSpectrumSlider()
    :    m_bMsgHandled{FALSE}
    ,       m_bCapture{false}
    ,    m_pimSpectrum{nullptr}
    ,       m_pstColor{nullptr}
    ,            m_Dib{}
    ,       m_rcRaster{}
{
}

bool CSpectrumSlider::Initialize(CSpectrumImage& imSpectrum, WTL::CStatic& stColor, long cx /*= SPECTRUM_SLIDER_CX*/)
{
    if (!m_Dib.Create(cx, 1, 32)) {
        return false;
    }
    m_pimSpectrum = &imSpectrum;
    m_pstColor = &stColor;
    SetRange(SPECTRUM_SLIDER_MIN, SPECTRUM_SLIDER_MAX, TRUE);
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

ATOM& CSpectrumSlider::GetWndClassAtomRef()
{
    static ATOM gs_CSpectrumSlider_Atom{0};
    return gs_CSpectrumSlider_Atom;
}

BOOL CSpectrumSlider::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL const bSave{m_bMsgHandled};
    BOOL const  bRet{_ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)};
    m_bMsgHandled = bSave;
    return bRet;
}

BOOL CSpectrumSlider::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        MSG_WM_CREATE(OnCreate)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
        NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
    ALT_MSG_MAP(1)
        REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
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

CRect CSpectrumSlider::GetRatserRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const
{
    CRect        rcRast{nmcd.rc};
    if (rcClient.IsRectEmpty()) {
        GetClientRect(rcClient);
    }
    if (dwStyle & TBS_VERT) {
        if (dwStyle & TBS_NOTHUMB) {
            rcRast.left = rcClient.left;
            rcRast.right = rcClient.right;
            rcRast.DeflateRect(2, 0);
        }
        else {
            rcRast.right = rcClient.right - 8;
            rcRast.left = rcRast.right - SPECTRUM_CHANNEL_CX;
            rcRast.DeflateRect(0, 5);
        }
    }
    else {
        rcRast.bottom = rcClient.bottom - 8;
        rcRast.top = rcRast.bottom - SPECTRUM_CHANNEL_CY;
        rcRast.DeflateRect(5, 0);
    }
    return rcRast;
}

void CSpectrumSlider::DrawRasterChannel(NMCUSTOMDRAW const& nmcd, DWORD dwStyle)
{
    CRect     rcClient{};
    CRect       rcDest{GetRatserRect(dwStyle, nmcd, rcClient)};
    WTL::CDCHandle  dc{nmcd.hdc};
    const int  iSaveDC{dc.SaveDC()};
    HBRUSH const brBlk{WTL::AtlGetStockBrush(BLACK_BRUSH)};

    if (dwStyle & TBS_VERT) {
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
        dc.StretchDIBits(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
                         0, 0, m_Dib.GetHeight(), m_Dib.GetWidth(),
                         m_Dib.GetData(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    }
    else {
        WTL::CDCHandle const dcSource{m_Dib.GetDC(dc)};
        dc.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
            dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);
    }
    dc.FrameRect(rcDest, brBlk);
    dc.RestoreDC(iSaveDC);
    m_rcRaster = rcDest;
 }

CRect CSpectrumSlider::GetThumbRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const
{
    if (rcClient.IsRectEmpty()) {
        GetClientRect(rcClient);
    }
    CRect rcThumb{nmcd.rc};
    if (dwStyle & TBS_VERT) {
        rcThumb.left = rcClient.left;
        rcThumb.right = rcClient.right;
        if (dwStyle & TBS_NOTHUMB) {
            rcThumb.DeflateRect(3, 1);
        }
        else {
            rcThumb.DeflateRect(6, 1);
        }
    }
    else {
        rcThumb.top = rcClient.top + 8;
        rcThumb.bottom = rcClient.bottom - 8;
    }
    return rcThumb;
}

LRESULT CSpectrumSlider::OnCustomDraw(LPNMHDR pNMHDR)
{
    NMCUSTOMDRAW const& nmcd{*reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR)};
    int const         itemId{static_cast<int>(nmcd.dwItemSpec)};
    switch(nmcd.dwDrawStage) {
    case CDDS_PREPAINT: {
        return CDRF_NOTIFYITEMDRAW;
    }
    case CDDS_ITEMPREPAINT: {
        switch (itemId) {
        case TBCD_CHANNEL: {
            DWORD const dwStyle{GetStyle()};
            DrawRasterChannel(nmcd, dwStyle);
            return dwStyle & TBS_NOTHUMB ? CDRF_SKIPDEFAULT : CDRF_DODEFAULT;
        }
        case TBCD_THUMB: {
            DWORD const dwStyle{GetStyle()};
            if (dwStyle & TBS_NOTHUMB) {
                CRect      rcClient{};
                CRect       rcThumb{GetThumbRect(dwStyle, nmcd, rcClient)};
                WTL::CDCHandle   dc{nmcd.hdc};
                const int   iSaveDC{dc.SaveDC()};
                //dc.FrameRect(rcThumb, WTL::AtlGetStockBrush(BLACK_BRUSH));
                dc.InvertRect(rcThumb);
                dc.RestoreDC(iSaveDC);
                InvalidateRect(nullptr, FALSE);
                return CDRF_SKIPDEFAULT;
            }
            break;
        }
        }
        break;
    }
    }
    return CDRF_DODEFAULT;
}

void CSpectrumSlider::OnLButtonUp(UINT, CPoint)
{
    if (!m_bCapture) {
        SetMsgHandled(FALSE);
        return ;
    }
    ReleaseCapture();
    m_bCapture = false;
}

void CSpectrumSlider::OnMouseMove(UINT nFlags, CPoint pt)
{
    if (!m_bCapture) {
        SetMsgHandled(FALSE);
        return;
    }
    DWORD const dwStyle{GetStyle()};
    long const     nMin{GetRangeMin()};
    long const     nMax{GetRangeMax()};
    long const   nRange{nMax-nMin};
    LONG           nPos{0};
    if (dwStyle & TBS_VERT) {
        float const nScale{static_cast<float>(m_rcRaster.Height()) / static_cast<float>(nRange)};
        nPos = static_cast<LONG>(static_cast<float>(pt.y - m_rcRaster.top) / nScale);
    }
    else {
        float const nScale{static_cast<float>(m_rcRaster.Width()) / static_cast<float>(nRange)};
        nPos = static_cast<LONG>(static_cast<float>(pt.x - m_rcRaster.left) / nScale);
    }
    if (nPos < nMin) {
        nPos = nMin;
    }
    else if (nPos > nMax) {
        nPos = nMax;
    }
    SetPos(nPos, TRUE);
    if (m_pimSpectrum) {
        m_pimSpectrum->OnSliderChanged(nPos);
    }
    else {
        NMTRBTHUMBPOSCHANGING tPosCh;
        tPosCh.dwPos = nPos;
        tPosCh.nReason = TB_THUMBPOSITION;
        tPosCh.hdr.code = TRBN_THUMBPOSCHANGING;
        tPosCh.hdr.idFrom = GetDlgCtrlID();
        tPosCh.hdr.hwndFrom = m_hWnd;
        ::SendMessageW(GetParent(), WM_NOTIFY, (WPARAM)tPosCh.hdr.idFrom, reinterpret_cast<LPARAM>(&tPosCh));
    }
    if (m_pstColor) {
        m_pstColor->InvalidateRect(nullptr, FALSE);
    }
    if (!m_rcRaster.IsRectEmpty() && !m_rcRaster.PtInRect(pt)) {
        OnLButtonUp(nFlags, pt);
    }
}

void CSpectrumSlider::OnLButtonDown(UINT, CPoint pt)
{
    if (m_rcRaster.IsRectEmpty() || !m_rcRaster.PtInRect(pt)) {
        SetMsgHandled(FALSE);
        return;
    }
    if (!m_bCapture) {
        SetCapture();
        m_bCapture = true;
    }
    else {
        ReleaseCapture();
        m_bCapture = false;
    }
}
