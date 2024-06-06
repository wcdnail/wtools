#include "stdafx.h"
#include "CMagnifierCtrl.h"
#include "CMagnifierInit.h"
#include <dev.assistance/dev.assist.h>
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <atlcrack.h>
#include <Magnification.h>
#include <string>

constexpr LONG MAG_RADIUS{257};

CMagnifierCtrl::~CMagnifierCtrl() = default;

CMagnifierCtrl::CMagnifierCtrl()
    :   m_rcMag{0, 0, MAG_RADIUS, MAG_RADIUS}
    ,   m_ptPos{0, 0}
    ,    m_ePos{MAGPOS_FOLLOW_MICE}
    , m_hCursor{nullptr}
    ,  m_ctlMag{}
    , m_onClick{}
{
    UNREFERENCED_PARAMETER(CMagnifierInit::Instance());
}

HRESULT CMagnifierCtrl::PreCreateWindow()
{
    static ATOM gs_Atom{0};
    return CCustomControl::PreCreateWindowImpl(gs_Atom, GetWndClassInfo());
}

BOOL CMagnifierCtrl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) {
    case 0:
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SETCURSOR(OnSetCursor)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_KEYDOWN(OnKeyDown)
#ifdef _DEBUG_XTRA
        if constexpr (true) {
            auto const msgStr = DH::MessageToStrignW((PMSG)GetCurrentMessage());
            DBGTPrint(LTH_CONTROL L" -WM- [[ %s ]]\n", msgStr.c_str());
        }
#endif
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

void CMagnifierCtrl::OnRectUpdated()
{
    if (!m_hWnd) {
        return ;
    }
    WTL::CRgn rgnRound;
    if (!rgnRound.CreateEllipticRgn(m_rcMag.left+1, m_rcMag.top+1, m_rcMag.right-1, m_rcMag.bottom-1)) {
        return ;
    }
    SetWindowRgn(rgnRound, FALSE);
}

bool CMagnifierCtrl::Initialize(HWND hWnd, float fFactor, HCURSOR hCursor, OnClickFn&& onClick)
{
    std::wstring sFunc{L"NONE"};
    HRESULT      hCode{S_OK};
    if (hWnd) {
        m_ePos = MAGPOS_FIXED;
        m_ctlMag.Attach(hWnd);
    }
    else {
        HWND const hWndRes{Create(nullptr, nullptr, nullptr,
            WS_BORDER | WS_POPUP | WS_CLIPCHILDREN,
            WS_EX_TOPMOST | WS_EX_LAYERED
        )};
        if (!hWndRes) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc = L"CreateWindowEx('" + std::wstring{GetWndClassInfo().m_wc.lpszClassName} + L"')";
            goto raiseError;
        }
        OnRectUpdated();
    }
    if (!SetMagnification(fFactor, fFactor)) {
        hCode = static_cast<HRESULT>(GetLastError());
        DH::TPrintf(TL_Error, L"SetMagnification failed: 0x%08x\n", sFunc.c_str(), hCode);
    }
    m_onClick = std::move(onClick);
    if (!hCursor) {
        m_hCursor = LoadCursorW(nullptr, IDC_CROSS);
    }
    else {
        m_hCursor = hCursor;
    }
    SetCursor(m_hCursor);
    return true;
raiseError:
    auto const msg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(TL_Error, L"%s failed: 0x%08x %s\n", sFunc.c_str(), hCode, msg.GetString());
    return false;
}

int CMagnifierCtrl::OnCreate(LPCREATESTRUCT pCS)
{
    UNREFERENCED_PARAMETER(pCS);
    HRESULT       hCode{S_OK};
    std::wstring  sFunc{L"NONE"};
    if (!SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA)) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"SetLayeredWindowAttributes";
        goto raiseError;
    }
    if (!m_ctlMag.Create(WC_MAGNIFIER, m_hWnd, m_rcMag, nullptr, WS_CHILD | WS_VISIBLE)) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"CreateWindowEx('" + std::wstring{WC_MAGNIFIERW} + L"')";
        goto raiseError;
    }
    DH::TPrintf(0, _T("%s OK for %p\n"), __FUNCTIONW__, this);
    return 0;
raiseError:
    auto const msg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(TL_Error, L"%s failed: 0x%08x %s\n", sFunc.c_str(), hCode, msg.GetString());
    return -1;
}

void CMagnifierCtrl::OnDestroy()
{
    m_ctlMag.DestroyWindow();
    SetMsgHandled(FALSE);
}

BOOL CMagnifierCtrl::OnSetCursor(HWND, UINT nHitTest, UINT message) const
{
    UNREFERENCED_PARAMETER(nHitTest);
    UNREFERENCED_PARAMETER(message);
    SetCursor(m_hCursor);
    return TRUE;
}

void CMagnifierCtrl::OnLButtonDown(UINT nFlags, CPoint) const
{
    if (m_onClick) {
        m_onClick(nFlags, m_ptPos, true);
    }
}

BOOL CMagnifierCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) const
{
    UNREFERENCED_PARAMETER(nFlags);
    UNREFERENCED_PARAMETER(pt);
    if (!m_ctlMag.m_hWnd) {
        return FALSE;
    }
    MAGTRANSFORM trMatrix{0};
    if (!MagGetWindowTransform(m_ctlMag.m_hWnd, &trMatrix)) {
        return FALSE;
    }
    float&   fXFactor{trMatrix.v[0][0]};
    float&   fYFactor{trMatrix.v[1][1]};
    float const fStep{static_cast<float>(zDelta) / 1200.f};
    if (fStep < 0 && fXFactor < 0.5) {
        return TRUE;
    }
    fXFactor += fStep;
    fYFactor += fStep;
    BOOL const bRes{MagSetWindowTransform(m_ctlMag.m_hWnd, &trMatrix)};
    return bRes;
}

void CMagnifierCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) const
{
    UNREFERENCED_PARAMETER(nRepCnt);
    if (VK_ESCAPE == nChar) {
        if (m_onClick) {
            m_onClick(nFlags, m_ptPos, false);
        }
    }
}

BOOL CMagnifierCtrl::SetMagnification(float fXFactor, float fYFactor) const
{
    if (!m_ctlMag.m_hWnd) {
        return FALSE;
    }
    MAGTRANSFORM trMatrix{0};
    trMatrix.v[0][0] = fXFactor;
    trMatrix.v[1][1] = fYFactor;
    trMatrix.v[2][2] = 1.f;
    return MagSetWindowTransform(m_ctlMag.m_hWnd, &trMatrix);
}

void CMagnifierCtrl::UpdatePosition()
{
    if (!m_ctlMag.m_hWnd) {
        return ;
    }
    if (MAGPOS_FOLLOW_MICE == m_ePos) {
        GetCursorPos(&m_ptPos);
    }
    CRect            rcSource{m_rcMag};
    float const   borderWidth{static_cast<float>(GetSystemMetrics(SM_CXFIXEDFRAME))};
    float const captionHeight{static_cast<float>(GetSystemMetrics(SM_CYCAPTION))};
    MAGTRANSFORM     trMatrix{0};
    MagGetWindowTransform(m_ctlMag.m_hWnd, &trMatrix);
    float const fXFactor{trMatrix.v[0][0]};
    float const fYFactor{trMatrix.v[1][1]};
    rcSource.left = (m_ptPos.x - static_cast<int>((static_cast<float>(m_rcMag.Width()) / 2.f) / fXFactor)) +
                                 static_cast<int>(borderWidth / fXFactor);
    rcSource.top = (m_ptPos.y -  static_cast<int>((static_cast<float>(m_rcMag.Height()) / 2.f) / fYFactor)) +
                                 static_cast<int>(captionHeight / fYFactor) +
                                 static_cast<int>(borderWidth / fYFactor);
    MagSetWindowSource(m_ctlMag.m_hWnd, rcSource);
    if (MAGPOS_FOLLOW_MICE == m_ePos) {
        MoveWindow(m_ptPos.x - m_rcMag.Width() / 2,
                   m_ptPos.y - m_rcMag.Height() / 2,
                   m_rcMag.Width(),
                   m_rcMag.Height(),
                   FALSE);
    }
    else {
        MoveWindow(m_rcMag, FALSE);
    }
    m_ctlMag.InvalidateRect(nullptr, TRUE);
}
