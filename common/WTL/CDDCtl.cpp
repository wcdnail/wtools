#include "stdafx.h"
#include "CDDCtl.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <rect.putinto.h>
#include <color.stuff.h>
#include <dh.tracing.h>
#include <atlcrack.h>


constexpr int             CHECKERS_CX{18};
constexpr COLORREF CLR_CHECKERS_WHITE{RGB(210, 210, 210)};
constexpr COLORREF CLR_CHECKERS_BLACK{RGB( 92,  92,  92)};

CDDCtrl::~CDDCtrl()
{
    if (!m_bBackOwner) {
        m_brBack.m_hBrush = nullptr;
    }
}

CDDCtrl::CDDCtrl()
    :        m_Dib{}
    ,     m_brBack{}
    , m_bBackOwner{true}
{
}

bool CDDCtrl::Initialize(long cx, long cy, long bpp, HBRUSH brBackground)
{
    if (!m_Dib.Create(cx, cy, bpp)) {
        return false;
    }
    if (brBackground) {
        m_brBack.Attach(brBackground);
        m_bBackOwner = false;
    }
    else {
        CDIBitmap bmCheckers{};
        if (!bmCheckers.Create(CHECKERS_CX, CHECKERS_CX, bpp)) {
            return false;
        }
        DDraw_Checkers(bmCheckers, CLR_CHECKERS_WHITE, CLR_CHECKERS_BLACK);
        WTL::CClientDC const dc{m_hWnd};
        m_brBack.CreatePatternBrush(bmCheckers.GetBitmap(dc.m_hDC));
    }
    return true;
}

void CDDCtrl::DrawRaster(WTL::CDCHandle dc, CRect const& rc, int nAlpha, CDIBitmap& diRaster) const
{
    HDC const dcSource{diRaster.GetDC(dc)};
    if (nAlpha < RGB_MAX_INT) {
        BLENDFUNCTION const blend{AC_SRC_OVER, 0, static_cast<BYTE>(nAlpha), 0};
        dc.SelectBrush(m_brBack);
        dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
        dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(),
                      dcSource, 0, 0, diRaster.GetWidth(), diRaster.GetHeight(), blend);
    }
    else {
        dc.StretchBlt(rc.left, rc.top, rc.Width(), rc.Height(),
                     dcSource, 0, 0, diRaster.GetWidth(), diRaster.GetHeight(), SRCCOPY);
    }
}

void CDDCtrl::DoPaint(WTL::CDCHandle dc, CRect const& rc)
{
    UNREFERENCED_PARAMETER(dc);
    UNREFERENCED_PARAMETER(rc);
    // ...
}

void CDDCtrl::DoNotifyMouseOver(CRect const& rc, CPoint pt)
{
    UNREFERENCED_PARAMETER(rc);
    UNREFERENCED_PARAMETER(pt);
    // ...
}

void CDDCtrl::NotifySend(UINT code) const
{
    NMHDR nmHeader;
    nmHeader.code = code;
    nmHeader.idFrom = GetDlgCtrlID();
    nmHeader.hwndFrom = m_hWnd;
    ::SendMessageW(GetParent(), WM_NOTIFY, nmHeader.idFrom, reinterpret_cast<LPARAM>(&nmHeader));
}

void CDDCtrl::OnLButtonUp(UINT, CPoint) const
{
    if (GetCapture() != m_hWnd) {
        return ;
    }
    ReleaseCapture();
}

void CDDCtrl::OnMouseMove(UINT, CPoint pt)
{
    if (GetCapture() != m_hWnd) {
        return;
    }
    CRect rcClient;
    GetClientRect(rcClient);
    DoNotifyMouseOver(rcClient, pt);
}

void CDDCtrl::OnLButtonDown(UINT, CPoint)
{
    if (m_hWnd != GetCapture()) {
        SetFocus();
        SetCapture();
    }
    else {
        ReleaseCapture();
    }
}

void CDDCtrl::OnNcPaint(HRGN) const
{
    CRect          rc{};
    WTL::CWindowDC dc{m_hWnd};
    GetClipBox(dc, rc);

    HWND const hWndFocus{GetFocus()};
    //DBGTPrint(L"FOCUS == %p [%p]\n", hWndFocus, m_hWnd);
    if (hWndFocus == m_hWnd) {
        dc.FillSolidRect(rc, RGB(255, 0, 0));
        //dc.DrawFocusRect(rc);
    }
    else {
        dc.DrawEdge(rc, EDGE_ETCHED, BF_FLAT | BF_RECT);
    }
}

void CDDCtrl::OnSetFocus(HWND hOldFocus)
{
    SendMessage(WM_NCPAINT);
    RedrawWindow();
    //DBGTPrint(L"FOCUS >> %p [%p]\n", m_hWnd, hOldFocus);
}

BOOL CDDCtrl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_KILLFOCUS(OnSetFocus)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        CHAIN_MSG_MAP(WTL::CBufferedPaintImpl<CDDCtrl>)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}
