#include "stdafx.h"
#include "CMagnifierCtrl.h"
#include <dev.assistance/dev.assist.h>
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <atlcrack.h>
#include <Magnification.h>
#include <string>

#pragma comment(lib, "Magnification.lib")

CMagnifierInit& CMagnifierInit::Instance()
{
    static CMagnifierInit gs_MagnifierInit;
    return gs_MagnifierInit;
}

CMagnifierInit::CMagnifierInit()
{
    if (MagInitialize()) {
        DH::TPrintf(LTH_GLOBALS L" MagInitialize OK\n");
        return ;
    }
    auto const hCode{static_cast<HRESULT>(GetLastError())};
    auto const  sMsg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(LTH_GLOBALS L" MagInitialize failed: 0x%08x %s\n", hCode, sMsg.GetString());
}

CMagnifierInit::~CMagnifierInit()
{
    if (MagUninitialize()) {
        DH::TPrintf(LTH_GLOBALS L" MagUninitialize OK\n");
        return ;
    }
    auto const hCode{static_cast<HRESULT>(GetLastError())};
    auto const  sMsg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(LTH_GLOBALS L" MagUninitialize failed: 0x%08x %s\n", hCode, sMsg.GetString());
}

CMagnifierCtrl::~CMagnifierCtrl() = default;

CMagnifierCtrl::CMagnifierCtrl()
    :  m_rcMag{0, 0, 256, 256}
    ,  m_ptPos{0, 0}
    ,   m_ePos{MAGPOS_FOLLOW_MICE}
    , m_ctlMag{}
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
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(bHandled);
    switch(dwMsgMapID) {
    case 0:
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
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

bool CMagnifierCtrl::Initialize(HWND hWnd, float fXFactor, float fYFactor)
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
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT
        )};
        if (!hWndRes) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc = L"CreateWindowEx('" + std::wstring{GetWndClassInfo().m_wc.lpszClassName} + L"')";
            goto raiseError;
        }
    }
    if (!SetMagnification(fXFactor, fYFactor)) {
        hCode = static_cast<HRESULT>(GetLastError());
        DH::TPrintf(LTH_CONTROL L" SetMagnification failed: 0x%08x\n", sFunc.c_str(), hCode);
    }
    return true;
raiseError:
    auto const msg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(LTH_CONTROL L" %s failed: 0x%08x %s\n", sFunc.c_str(), hCode, msg.GetString());
    return false;
}

int CMagnifierCtrl::OnCreate(LPCREATESTRUCT pCS)
{
    UNREFERENCED_PARAMETER(pCS);
    HRESULT      hCode{S_OK};
    std::wstring sFunc{L"NONE"};
    if (!SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA)) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"SetLayeredWindowAttributes";
        goto raiseError;
    }
    if (!m_ctlMag.Create(WC_MAGNIFIER, m_hWnd, m_rcMag, nullptr, WS_CHILD | MS_SHOWMAGNIFIEDCURSOR | WS_VISIBLE)) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"CreateWindowEx('" + std::wstring{WC_MAGNIFIERW} + L"')";
        goto raiseError;
    }
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
raiseError:
    auto const msg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(LTH_CONTROL L" %s failed: 0x%08x %s\n", sFunc.c_str(), hCode, msg.GetString());
    return -1;
}

void CMagnifierCtrl::OnDestroy()
{
    m_ctlMag.DestroyWindow();
    SetMsgHandled(FALSE);
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
    CRect          rcSource{m_rcMag};
    int const   borderWidth{GetSystemMetrics(SM_CXFIXEDFRAME)};
    int const captionHeight{GetSystemMetrics(SM_CYCAPTION)};
    MAGTRANSFORM   trMatrix{0};
    MagGetWindowTransform(m_ctlMag.m_hWnd, &trMatrix);
    float const fXFactor{trMatrix.v[0][0]};
    float const fYFactor{trMatrix.v[1][1]};
    rcSource.left = (m_ptPos.x - static_cast<int>((m_rcMag.Width() / 2) / fXFactor)) +
                            static_cast<int>(borderWidth / fXFactor);
    rcSource.top = (m_ptPos.y -  static_cast<int>((m_rcMag.Height() / 2) / fYFactor)) +
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
