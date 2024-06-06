#include "stdafx.h"
#include "luicPageImpl.h"
#include "dh.tracing.h"
#include "debug.dump.msg.h"
#include "dh.tracing.defs.h"
#include "dev.assistance/dev.assist.h"

CPageImpl::~CPageImpl() = default;

CPageImpl::CPageImpl(UINT idd, std::wstring&& caption)
    :         IDD{idd}
    ,   m_Caption{std::move(caption)}
    , m_ResiseMap{}
{
}

bool CPageImpl::ComboSetCurSelByData(WTL::CComboBox& cbControl, DWORD_PTR nData)
{
    const int nCount = cbControl.GetCount();
    for (int n = 0; n < nCount; n++) {
        if (nData == cbControl.GetItemData(n)) {
            cbControl.SetCurSel(n);
            return true;
        }
    }
    return false;
}

bool CPageImpl::CtlShow(HWND hWndCtl)
{
    ::ShowWindow(hWndCtl, SW_SHOW);
    return true;
}

bool CPageImpl::CtlDisable(HWND hWndCtl)
{
    ::EnableWindow(hWndCtl, FALSE);
    return true;
}

BOOL CPageImpl::DoForEachImpl(HWND hWndCtl, ForeachFn const& routine)
{
    if (!routine(hWndCtl)) {
        return FALSE;
    }
    return TRUE;
}

void CPageImpl::DoForEach(ForeachFn const& routine) const
{
    EnumChildWindows(m_hWnd, reinterpret_cast<WNDENUMPROC>(DoForEachImpl), reinterpret_cast<LPARAM>(&routine));
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

BOOL CPageImpl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled = TRUE;
    UNREFERENCED_PARAMETER(hWnd);
    DBG_DUMP_WMESSAGE(LTH_CONTROL, m_Caption.c_str(), (PMSG)GetCurrentMessage());
    switch (dwMsgMapID) {
    case 0:
        // FIXME: temporary TURN OFF colorizing
        //MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
        //MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_DROPFILES(OnDropFiles)
        MSG_WM_SETFOCUS(OnSetFocus)
        REFLECT_NOTIFICATIONS_EX()
        if (Resizer::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {
            OnResizeNotify();
            return TRUE;
        }
        break;
    default: ATLTRACE(static_cast<int>(ATL::atlTraceWindowing), 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

WTL::_AtlDlgResizeMap const* CPageImpl::GetDlgResizeMap() const
{
    static const WTL::_AtlDlgResizeMap emptyMap[] = { { -1, 0 } };
    return m_ResiseMap.empty() ? emptyMap : &m_ResiseMap[0];
}

void CPageImpl::DlgResizeAdd(int nCtlID, DWORD dwResizeFlags)
{
    if (-1 != nCtlID) { // is terminator ?
        CWindow item = GetDlgItem(nCtlID);
        _ASSERT_EXPR(item.Detach() != nullptr, _CRT_WIDE("Attempting to append not existing control to ResizeMap!"));
    }
    m_ResiseMap.emplace_back(WTL::_AtlDlgResizeMap{ nCtlID, dwResizeFlags });
}

void CPageImpl::DlgResizeAdd(WTL::_AtlDlgResizeMap const* vec, size_t count)
{
    ResizeVec newItems(m_ResiseMap.size() + count + 1);
    newItems.assign(vec, vec + count);
    newItems.append_range(m_ResiseMap);
    newItems.swap(m_ResiseMap);
}

HBRUSH CPageImpl::OnCtlColorStatic(WTL::CDCHandle dc, HWND)
{
#if 0
    dc.SetTextColor(m_pTheme->GetColor(COLOR_CAPTIONTEXT));
    dc.SetBkColor(m_pTheme->GetColor(COLOR_MENU));
    return m_pTheme->GetBrush(COLOR_MENU);
#else
    return nullptr;
#endif
}

HBRUSH CPageImpl::OnEraseBkgnd(WTL::CDCHandle dc)
{
#if 0
    CRect rc;
    GetClientRect(rc);
    dc.FillSolidRect(rc, m_pTheme->GetColor(COLOR_MENU));
    return m_pTheme->GetBrush(COLOR_MENU);
#else
    return nullptr;
#endif
}

void CPageImpl::OnResizeNotify()
{
}

BOOL CPageImpl::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);

#ifdef _DEBUG_CONTROLS
    ShowWindow(SW_SHOW);
#endif
    DBG_DUMP_WMESSAGE_EXT(LTH_CONTROL, m_Caption.c_str(), m_hWnd, WM_INITDIALOG, 0, lInitParam);

    ModifyStyle(WS_BORDER | DS_CONTROL, 0);
    ModifyStyleEx(WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE, 0);

    DlgResizeAdd(-1, 0);
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
        DBGTPrint(LTH_WM_COMMAND, L"** CANCEL ** request\n");
        // ##TODO: PREVENT destroy this!
        // ##TODO: No - Nicht - Nein - DestroyWindow() here!!!
        return;
    default:
        //DBGTPrint(LTH_WM_COMMAND L" Unhandled: id:%-4d nc:%-4d %s\n", nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
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
