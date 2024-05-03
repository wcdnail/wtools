#include "stdafx.h"
#include "luicPageImpl.h"
#include "luicMain.h"
#include "UT/debug.assistance.h"

#ifdef _DEBUG
#  define DEBUG_FALSE false
#else
#  define DEBUG_FALSE true
#endif

CPageImpl::~CPageImpl()
{
}

CPageImpl::CPageImpl(UINT idd)
    :         IDD(idd)
    , m_ResiseMap()
{
}

HWND CPageImpl::CreateDlg(HWND hWndParent, LPARAM dwInitParam)
{
    return Super::Create(hWndParent, dwInitParam);
}

WTL::_AtlDlgResizeMap const* CPageImpl::GetDlgResizeMap() const
{
    static const WTL::_AtlDlgResizeMap emptyMap[] = { { -1, 0 } };
    return m_ResiseMap.empty() ? emptyMap : &m_ResiseMap[0];
}

void CPageImpl::DlgResizeAdd(int nCtlID, DWORD dwResizeFlags)
{
    CWindow item = GetDlgItem(nCtlID);
    _ASSERT_EXPR(item.Detach() != nullptr, _CRT_WIDE("Attempting to append not existing control to ResizeMap!"));
    m_ResiseMap.emplace_back(_AtlDlgResizeMap{ nCtlID, dwResizeFlags });
}

void CPageImpl::DlgResizeAdd(WTL::_AtlDlgResizeMap const* vec, size_t count)
{
    ResizeVec newItems(m_ResiseMap.size() + count + 1);
    newItems.assign(vec, vec + count);
    newItems.append_range(m_ResiseMap);
    newItems.swap(m_ResiseMap);
}

BOOL CPageImpl::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);
    m_ResiseMap.emplace_back(_AtlDlgResizeMap{ -1, 0 });
    DlgResize_Init(false, DEBUG_FALSE);
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

void CPageImpl::OnDropFiles(HDROP hDropInfo)
{
    UNREFERENCED_PARAMETER(hDropInfo);
}

void CPageImpl::OnDestroy()
{
    SetMsgHandled(FALSE);
}

HBRUSH CPageImpl::OnCtlColorStatic(CDCHandle dc, HWND wndStatic)
{
    CTheme& theme = CLegacyUIConfigurator::App()->CurrentTheme();
    dc.SetTextColor(theme.GetColor(COLOR_CAPTIONTEXT));
    dc.SetBkColor(theme.GetColor(COLOR_MENU));
    return theme.GetBrush(COLOR_MENU);
}

HBRUSH CPageImpl::OnEraseBkgnd(CDCHandle dc)
{
    CTheme& theme = CLegacyUIConfigurator::App()->CurrentTheme();
    CRect rc;
    GetClientRect(rc);
    dc.FillSolidRect(rc, theme.GetColor(COLOR_MENU));
    return theme.GetBrush(COLOR_MENU);
}
