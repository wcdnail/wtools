#include "stdafx.h"
#include "CColorPicker.h"
#include "CColorPickerImpl.h"
#include "CAppModuleRef.h"
#include <dev.assistance/dev.assist.h>
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>

CColorPicker::~CColorPicker() = default;

CColorPicker::CColorPicker()
    : m_pImpl{std::make_unique<Impl>()}
{
}

HRESULT CColorPicker::PreCreateWindow()
{
    auto const code = m_pImpl->PreCreateWindow();
    if (ERROR_SUCCESS != code) {
        return code;
    }
    static ATOM gs_CColorPicker_Atom{0};
    return CCustomControl::PreCreateWindowImpl(gs_CColorPicker_Atom, GetWndClassInfo());
}

BOOL CColorPicker::PreTranslateMessage(MSG* pMsg)
{
    if (m_pImpl->IsDialogMessageW(pMsg)) {
        return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// IColorTarget overrides
//
//-----------------------------------------------------------------------------
COLORREF CColorPicker::GetColorRef() const
{
    return m_pImpl->m_imSpectrum.GetColorRef();
}

int CColorPicker::GetAlpha() const
{
    return m_pImpl->m_imSpectrum.GetColor().m_A;
}

void CColorPicker::SetColor(COLORREF crColor, int nAlpha)
{
    m_pImpl->SetColor(crColor, nAlpha, false);
}

bool CColorPicker::SetColorTarget(CColorTarget crTarget)
{
    m_pImpl->m_ColorTarget = std::move(crTarget);
    if (!m_pImpl->m_ColorTarget.m_pHost) {
        m_pImpl->m_ColorTarget.m_pHost = this;
    }
    m_pImpl->m_ColorTarget.UpdateHostColor();
    return true;
}

int& CColorPicker::RasterCX()
{
    return Impl::gs_nRasterCX;
}

BOOL CColorPicker::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(bHandled);
    switch(dwMsgMapID) {
    case 0:
      //MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
        REFLECT_NOTIFICATIONS()
        if constexpr (false) {
            auto const msgStr = DH::MessageToStrignW((PMSG)GetCurrentMessage());
            DBGTPrint(LTH_CONTROL L" -WM- [[ %s ]]\n", msgStr.c_str());
        }
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

void CColorPicker::OnNcPaint(WTL::CRgnHandle rgn)
{
    CRect rc{};
    if constexpr (false) {
        WTL::CDCHandle dc{GetDCEx(rgn, DCX_INTERSECTRGN | DCX_WINDOW)};
        GetClipBox(dc, rc);
        dc.DrawEdge(rc, EDGE_ETCHED, BF_FLAT | BF_RECT);
        ReleaseDC(dc);
    }
    else {
        WTL::CClientDC dc{m_hWnd};
        GetWindowRect(rc);
        dc.DrawEdge(rc, EDGE_ETCHED, BF_FLAT | BF_RECT);
    }
}

int CColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);
    ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    if (!m_pImpl->Create(m_hWnd)) {
        return -1;
    }
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
    }
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
}

void CColorPicker::OnDestroy()
{
    SetMsgHandled(FALSE);
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
    }
}

void CColorPicker::OnSize(UINT nType, CSize size) const
{
    if constexpr (true) {
        CRect rc{0, 0, size.cx, size.cy};
        m_pImpl->MoveWindow(rc);
    }
}
