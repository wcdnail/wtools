//
// https://www.codeproject.com/articles/8985/customizing-the-appearance-of-csliderctrl-using-cu
// Mike O'Neill's Customizing the Appearance of CSliderCtrl Using Custom Draw
// adopted to WTL
//
#include "stdafx.h"
#include "CSpectrumSlider.h"
#include "CSpectrumImage.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <atlmisc.h>

enum : int
{
    SPECTRUM_SLIDER_CX = 2,
    SPECTRUM_SLIDER_CY = 32,
};

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
    if (!m_Dib.Create(SPECTRUM_SLIDER_CX, SPECTRUM_SLIDER_CY, 32)) {
        return false;
    }
    return true;
}

void CSpectrumSlider::UpdateRaster(SpectrumKind spKind, double dHue)
{
    switch (spKind) {
    case SPEC_RGB_Red:
        break;
    case SPEC_RGB_Green:
        break;
    case SPEC_RGB_Blue:
        break;
    case SPEC_HSV_Hue:
        DDraw_HSV_Hue(m_Dib, dHue);
        break;
    case SPEC_HSV_Saturation:
        break;
    case SPEC_HSV_Brightness:
        break;
    default: 
        break;
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
    // sets primary color of control, and derives shadow and hilite colors
    // also initializes brushes that are used in custom draw functions
    m_crPrimary = crPrimary;

    // get hilite and shadow colors
    // uses the very-nice shell function of ColorAdjustLuma, described at
    // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/shlwapi/gdi/coloradjustluma.asp
    // for which we need at least IE version 5.0 and above, and a link to shlwapi.lib
    m_crHilite       = ColorAdjustLuma(crPrimary,  500, TRUE); // increase by 50%
    m_crMidShadow    = ColorAdjustLuma(crPrimary, -210, TRUE); // decrease by 21.0%
    m_crShadow       = ColorAdjustLuma(crPrimary, -445, TRUE); // decrease by 44.5%
    m_crDarkerShadow = ColorAdjustLuma(crPrimary, -500, TRUE); // decrease by 50.0%

    // create normal (solid) brush 
    m_normalBrush.Attach(CreateSolidBrush(crPrimary));

    // create a hatch-patterned pixel pattern for patterned brush (used when thumb has focus/is selected)
    // see http://www.codeproject.com/gdi/custom_pattern_brush.asp
    // or look for BrushTool.exe for the code that generates these bits
    static const WORD bitsBrush1[8] = { 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa };
    WTL::CBitmap bm{};
    bm.CreateBitmap(8, 8, 1, 1, bitsBrush1);
    LOGBRUSH logBrush;
    logBrush.lbStyle = BS_PATTERN;
    logBrush.lbHatch = reinterpret_cast<ULONG_PTR>(bm.m_hBitmap);
    logBrush.lbColor = 0; // ignored anyway; must set DC background and text colors
    m_focusBrush.Attach(CreateBrushIndirect(&logBrush));
}

CRect& CSpectrumSlider::SetChannelRect(NMCUSTOMDRAW& nmcd, CRect const& rcClient) const
{
    const DWORD dwStyle{GetStyle()};
    CRect&    rcChannel{static_cast<CRect&>(nmcd.rc)};
    if (dwStyle & TBS_VERT) {
        rcChannel.left = rcClient.left + 14;
        rcChannel.right = rcClient.right - 8;
        rcChannel.DeflateRect(6, 4);
    }
    else {
        rcChannel.top = rcClient.top + 8;
        rcChannel.bottom = rcClient.bottom - 8;
        rcChannel.DeflateRect(4, 6);
    }
    return rcChannel;
}

CRect& CSpectrumSlider::SetThumbRect(NMCUSTOMDRAW& nmcd, CRect const& rcClient) const
{
    const DWORD dwStyle{GetStyle()};
    CRect&      rcThumb{static_cast<CRect&>(nmcd.rc)};
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

LRESULT CSpectrumSlider::OnCustomDraw(LPNMHDR pNMHDR)
{
    NMCUSTOMDRAW& nmcd{*reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR)};
    int const   itemId{static_cast<int>(nmcd.dwItemSpec)};
    LRESULT       lRes{CDRF_DODEFAULT};
    switch(nmcd.dwDrawStage) {
    case CDDS_PREPAINT: {
        return CDRF_NOTIFYITEMDRAW;
    }
    case CDDS_ITEMPREPAINT: {
        switch (itemId) {
        case TBCD_CHANNEL: {
            CRect    rcClient{};
            GetClientRect(rcClient);
            CRect   rcChannel{SetChannelRect(nmcd, rcClient)};
            WTL::CDCHandle dc{nmcd.hdc};
            const int iSaveDC{dc.SaveDC()};
            dc.FrameRect(rcChannel, m_normalBrush);
            rcChannel.DeflateRect(2, 2);
            dc.FillSolidRect(rcChannel, m_crPrimary);
            dc.RestoreDC(iSaveDC);
            return CDRF_SKIPDEFAULT;
        }
        case TBCD_THUMB: {
            if constexpr (false) {
                CRect    rcClient{};
                GetClientRect(rcClient);
                CRect     rcThumb{SetThumbRect(nmcd, rcClient)};
                WTL::CDCHandle dc{nmcd.hdc};
                const int iSaveDC{dc.SaveDC()};
                WTL::CBrushHandle brCurrent{WTL::AtlGetStockBrush(BLACK_BRUSH)};
                dc.FrameRect(rcThumb, brCurrent);
                dc.RestoreDC(iSaveDC);
                Invalidate(FALSE);
                return CDRF_SKIPDEFAULT;
            }
            break;
        }
        }
        break;
    }
    }
    return lRes;
}
