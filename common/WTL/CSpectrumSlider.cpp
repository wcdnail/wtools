//
// https://www.codeproject.com/articles/8985/customizing-the-appearance-of-csliderctrl-using-cu
// Mike O'Neill's Customizing the Appearance of CSliderCtrl Using Custom Draw
// adopted to WTL
//
#include "stdafx.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <atlmisc.h>

CSpectrumSlider::~CSpectrumSlider() = default;

CSpectrumSlider::CSpectrumSlider()
    :    m_bMsgHandled{FALSE}
    ,      m_crPrimary{0x00000000}
    ,       m_crShadow{0x00000000}
    ,       m_crHilite{0x00000000}
    ,    m_crMidShadow{0x00000000}
    , m_crDarkerShadow{0x00000000}
    ,    m_normalBrush{}
    ,     m_focusBrush{}
    ,            m_Dib{}
{
}

bool CSpectrumSlider::Initialize()
{
    if (!m_Dib.Create(SPECTRUM_SLIDER_CX, 1, 32)) {
        return false;
    }
    return true;
}

void CSpectrumSlider::UpdateRaster(SpectrumKind spKind, double dHue)
{
    UNREFERENCED_PARAMETER(dHue);
    switch (spKind) {
    case SPEC_RGB_Red:        break;
    case SPEC_RGB_Green:      break;
    case SPEC_RGB_Blue:       break;
    case SPEC_HSV_Hue:        DDraw_HSV_HUE(m_Dib, 1, 1); break;
    case SPEC_HSV_Saturation: break;
    case SPEC_HSV_Brightness: break;
    default: break;
    }
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

void CSpectrumSlider::SetPrimaryColor(COLORREF crPrimary)
{
    m_crPrimary = crPrimary;

    m_crHilite       = ColorAdjustLuma(crPrimary,  500, TRUE); // increase by 50%
    m_crMidShadow    = ColorAdjustLuma(crPrimary, -210, TRUE); // decrease by 21.0%
    m_crShadow       = ColorAdjustLuma(crPrimary, -445, TRUE); // decrease by 44.5%
    m_crDarkerShadow = ColorAdjustLuma(crPrimary, -500, TRUE); // decrease by 50.0%

    m_normalBrush.Attach(CreateSolidBrush(crPrimary));

    static const WORD bitsBrush1[8] = { 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa };
    WTL::CBitmap bm{};
    bm.CreateBitmap(8, 8, 1, 1, bitsBrush1);
    LOGBRUSH logBrush{0};
    logBrush.lbStyle = BS_PATTERN;
    logBrush.lbHatch = reinterpret_cast<ULONG_PTR>(bm.m_hBitmap);
    m_focusBrush.Attach(CreateBrushIndirect(&logBrush));
}

CRect CSpectrumSlider::GetRatserRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const
{
    CRect        rcRast{nmcd.rc};
    if (rcClient.IsRectEmpty()) {
        GetClientRect(rcClient);
    }
    if (dwStyle & TBS_VERT) {
        rcRast.left = rcClient.left + 14;
        rcRast.right = rcClient.right - 8;
        rcRast.DeflateRect(8, 5);
    }
    else {
        rcRast.top = rcClient.top + 8;
        rcRast.bottom = rcClient.bottom - 8;
        rcRast.DeflateRect(5, 8);
    }
    return rcRast;
}

CRect CSpectrumSlider::GetThumbRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const
{
    if (rcClient.IsRectEmpty()) {
        GetClientRect(rcClient);
    }
    CRect rcThumb{nmcd.rc};
    if (dwStyle & TBS_VERT) {
        rcThumb.left = rcClient.left + 12;
        rcThumb.right = rcClient.right - 12;
    }
    else {
        rcThumb.top = rcClient.top + 8;
        rcThumb.bottom = rcClient.bottom - 8;
    }
    return rcThumb;
}

void CSpectrumSlider::DrawRasterChannel(NMCUSTOMDRAW const& nmcd)
{
    CRect                rcClient{};
    const DWORD           dwStyle{GetStyle()};
    CRect                  rcDest{GetRatserRect(dwStyle, nmcd, rcClient)};
    WTL::CDCHandle             dc{nmcd.hdc};
    const int             iSaveDC{dc.SaveDC()};
    WTL::CBrushHandle const brBlk{WTL::AtlGetStockBrush(BLACK_BRUSH)};

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
        dc.StretchDIBits(rcDest.left, rcDest.top,
                         rcDest.Width(), rcDest.Height(),
                         0, 0, m_Dib.GetHeight(), m_Dib.GetWidth(),
                         m_Dib.GetData(), &bmpInfo, 
                         DIB_RGB_COLORS, SRCCOPY);
    }
    else {
        WTL::CDCHandle const dcSource{m_Dib.GetDC(dc)};
        dc.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
            dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);
    }
    dc.FrameRect(rcDest, brBlk);
    dc.RestoreDC(iSaveDC);
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
            DrawRasterChannel(nmcd);
            return CDRF_DODEFAULT;
        }
        case TBCD_THUMB: {
            if constexpr (false) {
                CRect      rcClient{};
                DWORD const dwStyle{GetStyle()};
                CRect       rcThumb{GetThumbRect(dwStyle, nmcd, rcClient)};
                WTL::CDCHandle   dc{nmcd.hdc};
                const int   iSaveDC{dc.SaveDC()};
                dc.FrameRect(rcThumb, WTL::AtlGetStockBrush(BLACK_BRUSH));
                dc.RestoreDC(iSaveDC);
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
