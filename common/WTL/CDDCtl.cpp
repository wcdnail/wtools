#include "stdafx.h"
#include "CDDCtl.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <rect.putinto.h>
#include <atlcrack.h>

constexpr int             CHECKERS_CX{24};
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

void CDDCtrl::OnPaint(int nAlpha)
{
    WTL::CPaintDC const dcPaint{m_hWnd};
    WTL::CDCHandle           dc{dcPaint.m_hDC};
    HDC const          dcSource{m_Dib.GetDC(dc)};
    CRect const          rcDest{dcPaint.m_ps.rcPaint};
    int const             iSave{dc.SaveDC()};
    dc.SetStretchBltMode(COLORONCOLOR);
    if (nAlpha < 255) {
        BLENDFUNCTION const blend{/*AC_SRC_OVER*/0, 0, static_cast<BYTE>(nAlpha), 0};
        LONG const            dCY{1 + rcDest.Height() / m_Dib.GetHeight()};
        dc.SelectBrush(m_brBack);
        dc.PatBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), PATCOPY);
        dc.AlphaBlend(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height() + dCY,
                      dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), blend);
    }
    else {
        dc.StretchBlt(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
                      dcSource, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), SRCCOPY);
    }
    DoPaint(dc, rcDest);
    dc.RestoreDC(iSave);
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
        SetCapture();
    }
    else {
        ReleaseCapture();
    }
}

BOOL CDDCtrl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
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
