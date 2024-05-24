#include "stdafx.h"
#include "CSpectrumImage.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <rect.putinto.h>
#include <atlcrack.h>

constexpr COLORREF CLR_CHECKERS_WHITE{RGB(210, 210, 210)};
constexpr COLORREF CLR_CHECKERS_BLACK{RGB( 92,  92,  92)};

CSpectrumImage::~CSpectrumImage() = default;

CSpectrumImage::CSpectrumImage(COLORREF crInit, SpectrumKind kind)
    : m_Color{crInit}
    , m_nKind{kind}
{
}

HRESULT CSpectrumImage::PreCreateWindow()
{
    static ATOM gs_CSpectrumImage_Atom{0};
    return CCustomControl::PreCreateWindowImpl(gs_CSpectrumImage_Atom, GetWndClassInfo());
}

bool CSpectrumImage::Initialize(long cx, long cy, HBRUSH brBackground)
{
    return CDDCtrl::Initialize(cx, cy, SPECTRUM_BPP, brBackground);
}

void CSpectrumImage::SetSpectrumKind(SpectrumKind kind)
{
    if (kind < SPEC_Begin || kind > SPEC_End) {
        return ;
    }
    m_nKind = kind;
    CRect rc{};
    GetClientRect(rc);
    CPoint const pt{m_Color.GetColorPoint(m_nKind, rc)};
    DoNotifyMouseOver(rc, pt);
    m_Color.SetUpdated(true);
    InvalidateRect(nullptr, FALSE);
}

BOOL CSpectrumImage::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) {
    case 0:
        CHAIN_MSG_MAP(CDDCtrl)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

void CSpectrumImage::UpdateRaster()
{
    switch (m_nKind) {
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue:         DDraw_RGB(m_Dib, m_Color.GetRGBSpectrumRect(m_nKind)); break;
    case SPEC_HSV_Hue:          DDraw_HSV_Hue(m_Dib, m_Color.m_H); break;
    case SPEC_HSV_Saturation:   DDraw_HSV_Sat(m_Dib, m_Color.m_S / HSV_100PERC); break;
    case SPEC_HSV_Brightness:   DDraw_HSV_Val(m_Dib, m_Color.m_V / HSV_100PERC); break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    m_Dib.FreeResources();
}

void CSpectrumImage::OnColorChanged(double xPos, double yPos)
{
    switch (m_nKind) {
    case SPEC_RGB_Red:          m_Color.SetRGB(m_Color.GetRed(), static_cast<int>(xPos), static_cast<int>(yPos)); break;
    case SPEC_RGB_Green:        m_Color.SetRGB(static_cast<int>(xPos), m_Color.GetGreen(), static_cast<int>(yPos)); break;
    case SPEC_RGB_Blue:         m_Color.SetRGB(static_cast<int>(yPos), static_cast<int>(xPos), m_Color.GetBlue()); break;
    case SPEC_HSV_Hue:          m_Color.SetHSV(m_Color.m_H, xPos / RGB_MAX * HSV_100PERC, (RGB_MAX - yPos) / RGB_MAX * HSV_100PERC); break;
    case SPEC_HSV_Saturation:   m_Color.SetHSV(xPos / RGB_MAX * HSV_HUE_MAX, m_Color.m_S, (RGB_MAX - yPos) / RGB_MAX * HSV_100PERC); break;
    case SPEC_HSV_Brightness:   m_Color.SetHSV(xPos / RGB_MAX * HSV_HUE_MAX, (RGB_MAX - yPos) / RGB_MAX * HSV_100PERC, m_Color.m_V); break;
    default: 
        ATLASSERT(FALSE);
        break;
    }
    InvalidateRect(nullptr, FALSE);
}

void CSpectrumImage::DoPaint(WTL::CDCHandle dc, CRect const& rc)
{
    if (m_Color.IsUpdated()) {
        UpdateRaster();
        m_Color.SetUpdated(false);
    }
    DrawRaster(dc, rc, m_Color.m_A, m_Dib);
    DrawMarker(dc, rc);
}

void CSpectrumImage::DrawMarker(WTL::CDCHandle dc, CRect const& rcDest)
{
    CPoint const pt{m_Color.GetColorPoint(m_nKind, rcDest)};
    CRect const  rc{pt.x-1, pt.y-1, pt.x+1, pt.y+1};
    dc.InvertRect(CRect(rc.left - 5, rc.top, rc.left, rc.bottom));
    dc.InvertRect(CRect(rc.left, rc.top - 5, rc.right, rc.top));
    dc.InvertRect(CRect(rc.right, rc.top, rc.right + 5, rc.bottom));
    dc.InvertRect(CRect(rc.left, rc.bottom, rc.right, rc.bottom + 5));
}

void CSpectrumImage::DoNotifyMouseOver(CRect const& rc, CPoint pt)
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
    NotifySend(NM_SPECTRUM_CLR_SEL);
}
