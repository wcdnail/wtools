#include "stdafx.h"
#include "luicPageImpl.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "debug.dump.msg.h"
#include "dh.tracing.defs.h"
#include "dev.assistance/dev.assist.h"

CPageImpl::~CPageImpl()
{
}

CPageImpl::CPageImpl(UINT idd, std::wstring&& caption)
    :         IDD{ idd }
    ,   m_Caption{ std::move(caption) }
    , m_ResiseMap{}
{
}

BOOL CPageImpl::PreTranslateMessage(MSG* pMsg)
{
    if (IsDialogMessageW(pMsg)) {
        DBG_DUMP_WMESSAGE(LTH_CONTROL, m_Caption.c_str(), pMsg);
        return TRUE;
    }
    return FALSE;
}

HWND CPageImpl::CreateDlg(HWND hWndParent, LPARAM dwInitParam)
{
    return Super::Create(hWndParent, dwInitParam);
}

void CPageImpl::SelectAll()
{
    // nothign to do
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

HBRUSH CPageImpl::OnCtlColorStatic(CDCHandle dc, HWND wndStatic)
{
    CTheme& theme = CLUIApp::App()->CurrentTheme();
    dc.SetTextColor(theme.GetColor(COLOR_CAPTIONTEXT));
    dc.SetBkColor(theme.GetColor(COLOR_MENU));
    return theme.GetBrush(COLOR_MENU);
}

HBRUSH CPageImpl::OnEraseBkgnd(CDCHandle dc)
{
    CTheme& theme = CLUIApp::App()->CurrentTheme();
    CRect rc;
    GetClientRect(rc);
    dc.FillSolidRect(rc, theme.GetColor(COLOR_MENU));
    return theme.GetBrush(COLOR_MENU);
}

void CPageImpl::OnResizeNotify()
{
}

BOOL CPageImpl::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DBG_DUMP_WMESSAGE_EXT(LTH_CONTROL, m_Caption.c_str(), m_hWnd, WM_INITDIALOG, 0, lInitParam);

    ModifyStyle(WS_BORDER | DS_CONTROL, 0);
    ModifyStyleEx(WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE, 0);

    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);

    m_ResiseMap.emplace_back(_AtlDlgResizeMap{ -1, 0 });
    DlgResize_Init(false, false);
    return TRUE;
}

void CPageImpl::OnDestroy()
{
    SetMsgHandled(FALSE);
}

void CPageImpl::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    UNREFERENCED_ARG(uNotifyCode);
    UNREFERENCED_ARG(nID);
    UNREFERENCED_ARG(wndCtl);
    switch(nID) {
    case IDCANCEL:
        DBG_DUMP_WMESSAGE_EXT(LTH_CANCEL, m_Caption.c_str(), m_hWnd, 0, 0, 0);
        DebugThreadPrintf(LTH_WM_COMMAND L" ** CANCEL ** request\n");
        // ##TODO: PREVENT destroy this!
        // ##TODO: No Nicht Nein DestroyWindow() here
        return;
    default:
        //DebugThreadPrintf(LTH_WM_COMMAND L" Unhandled: id:%-4d nc:%-4d %s\n", nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
        break;
    }
    SetMsgHandled(FALSE);
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

void CPageImpl::OnSetFocus(HWND hWndOld)
{
    UNREFERENCED_PARAMETER(hWndOld);
    SetMsgHandled(FALSE);
}