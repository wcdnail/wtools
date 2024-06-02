#include "stdafx.h"
#include "luicMainView.h"
#include "luicBackground.h"
#include "luicScreenSaver.h"
#include "luicAppearance.h"
#include "luicEffects.h"
#include "luicPageDllIcons.h"
#include "luicWeb.h"
#include "luicSettings.h"
#include "luicUtils.h"
#include "resz/resource.h"
#include <string.utils.format.h>
#include <UT/debug.assistance.h>
#include <dh.tracing.defs.h>
#include <ranges>

enum PageIndex: int
{
    PageBeforBegin = -1,
  //PageBackground,
  //PageScreenSaver,
    PageAppearance,
  //PageEffects,
  //PageWeb,
    PageDllIcons,
    PageSettings,
    PageEnd
};

void CMainView::PagesCreate()
{
    m_TabCtrl.Attach(GetDlgItem(IDC_TAB1));

  //auto   pBackground = std::make_unique<CPageBackground> (L"Background");
  //auto  pScreenSaver = std::make_unique<CPageScreenSaver>(L"Screen Saver");
    auto   pAppearance = std::make_unique<CPageAppearance> (L"Appearance");
  //auto  pPageEffects = std::make_unique<CPageEffects>    (L"Effects");
  //auto      pPageWeb = std::make_unique<CPageWeb>        (L"Web");
    auto pPageDllIcons = std::make_unique<CPageDllIcons>   (L"DLL Icons");
    auto pPageSettings = std::make_unique<CPageSettings>   (L"Settings");

  //PagesAppend(PageBackground,  std::move(pBackground));
  //PagesAppend(PageScreenSaver, std::move(pScreenSaver));
    PagesAppend(std::move(pAppearance));
  //PagesAppend(PageEffects,     std::move(pPageEffects));
  //PagesAppend(PageWeb,         std::move(pPageWeb));
    PagesAppend(std::move(pPageDllIcons));
    PagesAppend(std::move(pPageSettings));
}

CMainView::~CMainView()
{
}

CMainView::CMainView()
    :     CPageImpl{IDD_LEGACY_UI_MAIN_VIEW, L"MainView"}
    ,     m_TabCtrl{}
    ,    m_PagesMap{}
    , m_rcTabClient{}
{
}

BOOL CMainView::PreTranslateMessage(MSG* pMsg)
{
    for (const auto& page : m_PagesMap | std::views::values) {
        if (page->PreTranslateMessage(pMsg)) {
            return TRUE;
        }
    }
    return CPageImpl::PreTranslateMessage(pMsg);
}

void CMainView::TabShift(int num)
{
    const int count{m_TabCtrl.GetItemCount() - 1};
    const int  curr{m_TabCtrl.GetCurSel()};
    int        next{curr + num};
    if (next < 0) {
        next = count;
    }
    else if (next > count) {
        next = 0;
    }
    m_TabCtrl.SetCurSel(next);
    PagesShow(curr, false);
    PagesShow(next, true);
}

void CMainView::SelectAll()
{
    switch (m_TabCtrl.GetCurSel()) {
    case PageDllIcons: {
        PagesGetCurrent()->SelectAll();
        break;
    }
    default:
        break;
    }
}

void CMainView::NotifySchemesChanged() const
{
    auto* pAppearance{PagesGetT<CPageAppearance>(PageAppearance)};
    if (!pAppearance) {
        return ;
    }
    pAppearance->NotifySchemesChanged();
}

template <typename PageType>
PageType* CMainView::PagesGetT(int numba) const
{
    const auto& it = m_PagesMap.find(numba);
    if (it == m_PagesMap.cend()) {
        DBGTPrint(LTH_CONTROL L" Page #%d does not EXIST!\n", numba);
        return nullptr;
    }
    return static_cast<PageType*>(it->second.get());
}

CPageImpl* CMainView::PagesGet(int numba) const
{
    return PagesGetT<CPageImpl>(numba);
}

CPageImpl* CMainView::PagesGetCurrent() const
{
    return PagesGet(m_TabCtrl.GetCurSel());
}

void CMainView::PagesGetRect()
{
    CRect rcTab;
    if constexpr (false) { // ##FIXME: is m_TabCtrl owns page ?
        CRect rcMy;
        GetWindowRect(rcMy);
        m_TabCtrl.GetWindowRect(rcTab);
        OffsetRect(rcTab, -rcMy.left, -rcMy.top);
    }
    else {
        m_TabCtrl.GetClientRect(rcTab);
    }
    m_rcTabClient = rcTab;
#if !defined(_DEBUG_TAB_RECT)
    TabCtrl_AdjustRect(m_TabCtrl.m_hWnd, FALSE, m_rcTabClient);
    m_rcTabClient.InflateRect(4, 4);
#else
    m_TabCtrl.ShowWindow(SW_HIDE);
    {
        CWindowDC dc(m_hWnd);
        CRect rcEdge = m_rcTabClient;
        rcEdge.InflateRect(4, 4);
        dc.FillSolidRect(rcEdge, 0x005f5fff);

        CRect rcAdj = m_rcTabClient;
        TabCtrl_AdjustRect(m_TabCtrl.m_hWnd, FALSE, rcAdj);
        dc.FillSolidRect(m_rcTabClient, 0x001f1f1f);
        dc.FillSolidRect(rcAdj, 0x003f3f1f);
    }
    m_TabCtrl.Invalidate(TRUE);
    m_TabCtrl.ShowWindow(SW_SHOW);
#endif
}

void CMainView::PagesShow(int numba, bool show) const
{
    if (auto* page = PagesGet(numba)) {
        if (!show) {
            page->ShowWindow(SW_HIDE);
            page->EnableWindow(FALSE);
        }
        else {
            page->EnableWindow(TRUE);
            page->ShowWindow(SW_SHOW);
            page->SetFocus();
        }
    }
}

void CMainView::PagesAppend(CPageImplPtr&& pagePtr)
{
    HRESULT code = S_FALSE;
    const int number = m_TabCtrl.InsertItem(m_TabCtrl.GetItemCount(), TCIF_TEXT, pagePtr->GetCaption(), 0, 0l);
    if (number < 0) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Append dialog page '%s' failed!", pagePtr->GetCaption()), code, true, MB_ICONERROR);
        return ;
    }
    if (!pagePtr->CreateDlg(m_TabCtrl.m_hWnd)) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Append dialog page '%s' failed!", pagePtr->GetCaption()), code, true, MB_ICONERROR);
        return ;
    }
    if (m_rcTabClient.IsRectEmpty()) {
        PagesGetRect();
    }
    pagePtr->ShowWindow(SW_HIDE);
    pagePtr->MoveWindow(m_rcTabClient, FALSE);
    m_PagesMap[number] = std::move(pagePtr);
}

void CMainView::OnResizeNotify()
{
    if (!m_TabCtrl.m_hWnd) {
        return ;
    }
    PagesGetRect();
    for (const auto& page: m_PagesMap | std::views::values) {
        page->MoveWindow(m_rcTabClient, FALSE);
    }
}

BOOL CMainView::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);

#ifdef _DEBUG_CONTROLS
    ShowWindow(SW_SHOW);
#endif
#if 0 || defined(_DEBUG_TAB_RECT)
    MoveToMonitor{}.Move(m_hWnd, 3);
#endif
    DoForEach(CtlDisable);

    GetParent().SetWindowTextW(L"Display Properties");
    PagesCreate();

    constexpr int initialPage = 0;
    m_TabCtrl.SetCurSel(initialPage);
    m_TabCtrl.EnableWindow(TRUE);
    PagesShow(initialPage, true);

    DlgResizeAdd(IDC_TAB1, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    DlgResizeAdd(IDC_BN_OK, DLSZ_MOVE_X | DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_BN_CANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_BN_APPLY, DLSZ_MOVE_X | DLSZ_MOVE_Y);

    WTL::CButton{GetDlgItem(IDC_BN_OK)}.EnableWindow(TRUE);
    WTL::CButton{GetDlgItem(IDC_BN_CANCEL)}.EnableWindow(TRUE);
    WTL::CButton{GetDlgItem(IDC_BN_APPLY)}.EnableWindow(TRUE);

    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

void CMainView::OnDestroy()
{
    m_TabCtrl.DeleteAllItems();
    m_TabCtrl.DestroyWindow();
    CPageImpl::OnDestroy();
}

void CMainView::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    CPageImpl::OnCommand(uNotifyCode, nID, wndCtl);
}

LRESULT CMainView::OnNotify(int, LPNMHDR pnmh)
{
    switch (pnmh->code) {
    case TCN_SELCHANGE: {
        const int numba = TabCtrl_GetCurSel(pnmh->hwndFrom);
        PagesShow(numba, true);
        break;
    }
    case TCN_SELCHANGING: {
        const int numba = TabCtrl_GetCurSel(pnmh->hwndFrom);
        PagesShow(numba, false);
        break;
    }
    default:
        break;
    }
    return 0;
}

void CMainView::OnSetFocus(HWND hWndOld)
{
    auto* current = PagesGetCurrent();
    if (!current) {
        CPageImpl::OnSetFocus(hWndOld);
        return ;
    }
    current->SetFocus();
}
