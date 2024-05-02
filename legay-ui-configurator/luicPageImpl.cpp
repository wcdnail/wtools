#include "stdafx.h"
#include "luicPageImpl.h"
#include "luicMain.h"
#include "UT/debug.assistance.h"

CPageImpl::~CPageImpl()
{
}

CPageImpl::CPageImpl(UINT idd)
    :         IDD(idd)
    , m_resiseMap()
{
}

HWND CPageImpl::CreateDlg(HWND hWndParent, LPARAM dwInitParam)
{
    return Super::Create(hWndParent, dwInitParam);
}

WTL::_AtlDlgResizeMap const* CPageImpl::GetDlgResizeMap() const
{
    static const WTL::_AtlDlgResizeMap emptyMap[] = { { -1, 0 } };
    return m_resiseMap.empty() ? emptyMap : &m_resiseMap[0];
}

void CPageImpl::DlgResizeAdd(int nCtlID, DWORD dwResizeFlags)
{
    CWindow item = GetDlgItem(nCtlID);
    _ASSERT_EXPR(item.Detach() != nullptr, _CRT_WIDE("Attempting to append not existing control to ResizeMap!"));
    m_resiseMap.emplace_back(_AtlDlgResizeMap{ nCtlID, dwResizeFlags });
}

BOOL CPageImpl::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);
    m_resiseMap.emplace_back(_AtlDlgResizeMap{ -1, 0 });
    DlgResize_Init(false, true);
    return TRUE;
}

void CPageImpl::OnResizeNotify() {}

void CPageImpl::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    UNREFERENCED_ARG(uNotifyCode);
    UNREFERENCED_ARG(nID);
    UNREFERENCED_ARG(wndCtl);
    SetMsgHandled(FALSE);
    DebugThreadPrintf(LTH_WM_COMMAND L" Unknown: n:%4d c:%4d w:%08x\n", uNotifyCode, nID, wndCtl);
}

LRESULT CPageImpl::OnNotify(int idCtrl, LPNMHDR pnmh)
{
    UNREFERENCED_ARG(idCtrl);
    UNREFERENCED_ARG(pnmh);
    SetMsgHandled(FALSE);
    return 0;
}

void CPageImpl::OnDestroy()
{
    SetMsgHandled(FALSE);
}

HBRUSH CPageImpl::OnEraseBkgnd(CDCHandle dc)
{
    CTheme const& theme = CLegacyUIConfigurator::m_ThemeNative;

    CRect rc;
    GetClientRect(rc);
    dc.FillSolidRect(rc, theme.m_Color[COLOR_APPWORKSPACE]);
    return theme.m_Brush[COLOR_APPWORKSPACE];
}
