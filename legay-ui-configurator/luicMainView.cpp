#include "stdafx.h"
#include "luicMainView.h"
#include "luicMain.h"
#include "luicBackground.h"
#include "luicScreenSaver.h"
#include "luicAppearance.h"
#include "luicEffects.h"
#include "luicPageDllIcons.h"
#include "luicWeb.h"
#include "luicSettings.h"
#include "string.utils.format.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

enum PageIndex: int
{
    PageBegin = -1,
    PageBackground,
    PageScreenSaver,
    PageAppearance,
    PageEffects,
    PageWeb,
    PageSettings,
    PageDllIcons,
    PageEnd
};

CMainView::~CMainView()
{
}

CMainView::CMainView()
    :      CPageImpl{ IDD_LEGACY_UI_MAIN_VIEW, L"MainView" }
    ,         m_Tab {}
    ,    m_PagesMap {}
    , m_rcTabClient {}
{
}

BOOL CMainView::PreTranslateMessage(MSG* pMsg)
{
    for (const auto& it: m_PagesMap) {
        if (it.second->PreTranslateMessage(pMsg)) {
            return TRUE;
        }
    }
    return CPageImpl::PreTranslateMessage(pMsg);
}

CPageImplPtr const& CMainView::PagesGet(int numba) const
{
    const auto& it = m_PagesMap.find(numba);
    if (it == m_PagesMap.cend()) {
        DH::ThreadPrintfc(DH::Category::Module(), L"Page #%d does not EXIST!\n", numba);
        static const CPageImplPtr dmyPtr;
        return dmyPtr;
    }
    return it->second;
}

CPageImplPtr const& CMainView::PagesGetCurrent() const
{
    return PagesGet(m_Tab.GetCurSel());
}

void CMainView::PagesGetRect()
{
    CRect rcTab;
    if (0) { // ##FIXME: is m_Tab owns page ?
        CRect rcMy;
        GetWindowRect(rcMy);
        m_Tab.GetWindowRect(rcTab);
        OffsetRect(rcTab, -rcMy.left, -rcMy.top);
    }
    else {
        m_Tab.GetClientRect(rcTab);
    }
    m_rcTabClient = rcTab;
#if !defined(_DEBUG_TAB_RECT)
    TabCtrl_AdjustRect(m_Tab.m_hWnd, FALSE, m_rcTabClient);
    m_rcTabClient.InflateRect(4, 4);
#else
    m_Tab.ShowWindow(SW_HIDE);
    {
        CWindowDC dc(m_hWnd);
        CRect rcEdge = m_rcTabClient;
        rcEdge.InflateRect(4, 4);
        dc.FillSolidRect(rcEdge, 0x005f5fff);

        CRect rcAdj = m_rcTabClient;
        TabCtrl_AdjustRect(m_Tab.m_hWnd, FALSE, rcAdj);
        dc.FillSolidRect(m_rcTabClient, 0x001f1f1f);
        dc.FillSolidRect(rcAdj, 0x003f3f1f);
    }
    m_Tab.Invalidate(TRUE);
    m_Tab.ShowWindow(SW_SHOW);
#endif
}

void CMainView::PagesCreate()
{
    m_Tab.Attach(GetDlgItem(IDC_TAB1));

    auto   pBackground = std::make_unique<CPageBackground> (L"Background");
    auto  pScreenSaver = std::make_unique<CPageScreenSaver>(L"Screen Saver");
    auto   pAppearance = std::make_unique<CPageAppearance> (L"Appearance");
    auto  pPageEffects = std::make_unique<CPageEffects>    (L"Effects");
    auto      pPageWeb = std::make_unique<CPageWeb>        (L"Web");
    auto pPageSettings = std::make_unique<CPageSettings>   (L"Settings");
    auto pPageDllIcons = std::make_unique<CPageDllIcons>   (L"DLL Icons");

    PagesAppend(PageBackground,  std::move(pBackground));
    PagesAppend(PageScreenSaver, std::move(pScreenSaver));
    PagesAppend(PageAppearance,  std::move(pAppearance));
    PagesAppend(PageEffects,     std::move(pPageEffects));
    PagesAppend(PageWeb,         std::move(pPageWeb));
    PagesAppend(PageSettings,    std::move(pPageSettings));
    PagesAppend(PageDllIcons,    std::move(pPageDllIcons));
}

void CMainView::PagesShow(int numba, bool show)
{
    const auto& page = PagesGet(numba);
    if (page) {
        page->ShowWindow(show ? SW_SHOW : SW_HIDE);
    }
}

void CMainView::PagesAppend(int desiredIndex, CPageImplPtr&& pagePtr)
{
    HRESULT code = S_FALSE;
    int  tabIcon = desiredIndex;
    UINT tabMask = TCIF_TEXT;
    if (0 /*&& tabIcon < m_ImList.GetImageCount()*/) { // ##FIXME: tab control has icons?
        tabIcon = desiredIndex;
        tabMask |= TCIF_IMAGE;
    }
    int number = m_Tab.InsertItem(m_Tab.GetItemCount(), tabMask, pagePtr->GetCaption(), tabIcon, 0l);
    if (number < 0) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Append dialog page '%s' failed!", pagePtr->GetCaption()), code, true, MB_ICONERROR);
        return ;
    }
    if (desiredIndex != number) {
        code = ERROR_REVISION_MISMATCH;
        ReportError(Str::ElipsisW::Format(
                        L"Append dialog page '%s' failed!\r\n"
                        L"Desired index %d != %d actual index\r\n",
                        pagePtr->GetCaption(), desiredIndex, number),
                    code, true, MB_ICONSTOP);
        return ;
    }
    if (!pagePtr->CreateDlg(m_Tab.m_hWnd)) {
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
    if (!m_Tab.m_hWnd) {
        return ;
    }
    PagesGetRect();
    for (const auto& it: m_PagesMap) {
        it.second->MoveWindow(m_rcTabClient, FALSE);
    }
}

BOOL CMainView::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
#if 0 || defined(_DEBUG_TAB_RECT)
    MoveToMonitor{}.Move(m_hWnd, 3);
    ShowWindow(SW_SHOW);
#endif

    GetParent().SetWindowTextW(L"Display Properties");
    PagesCreate();

#ifdef _DEBUG
    const int initialPage = PageAppearance;
#else
    const int initialPage = PageBackground;
#endif
    m_Tab.SetCurSel(initialPage);
    PagesShow(initialPage, true);

    DlgResizeAdd(IDC_TAB1, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    DlgResizeAdd(IDC_BN_CANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_BN_APPLY, DLSZ_MOVE_X | DLSZ_MOVE_Y);
    BOOL rv = CPageImpl::OnInitDialog(wndFocus, lInitParam);
    return rv;
}

void CMainView::OnDestroy()
{
    m_Tab.DeleteAllItems();
    m_Tab.DestroyWindow();
    CPageImpl::OnDestroy();
}

LRESULT CMainView::OnNotify(int idCtrl, LPNMHDR pnmh)
{
    switch (pnmh->code) {
    case TCN_SELCHANGE: {
        int numba = TabCtrl_GetCurSel(pnmh->hwndFrom);
        PagesShow(numba, true);
        break;
    }
    case TCN_SELCHANGING: {
        int numba = TabCtrl_GetCurSel(pnmh->hwndFrom);
        PagesShow(numba, false);
        break;
    }
    default:
        //DebugThreadPrintf(LTH_WM_NOTIFY L" Unknown: c:%4d n:%04x\n", idCtrl, pnmh->code);
        break;
    }
    return 0;
}

void CMainView::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    CPageImpl::OnCommand(uNotifyCode, nID, wndCtl);
}
