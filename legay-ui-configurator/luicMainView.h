#pragma once

#include "luicPageImpl.h"
#include <map>

struct CMainView: CPageImpl
{
    using  PagesMap = std::map<int, CPageImplPtr>;

    ~CMainView() override;
    CMainView();

    BOOL PreTranslateMessage(MSG* pMsg) override;
    void TabShift(int num);
    void SelectAll();

private:
    WTL::CTabCtrl m_TabCtrl;
    PagesMap     m_PagesMap;
    CRect     m_rcTabClient;

    CPageImplPtr const& PagesGet(int numba) const;
    CPageImplPtr const& PagesGetCurrent() const;
    
    void PagesGetRect();
    void PagesAppend(CPageImplPtr&& pagePtr);
    void PagesCreate();
    void PagesShow(int numba, bool show);

    void OnResizeNotify() override;
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    LRESULT OnNotify(int idCtrl, LPNMHDR pnmh) override;
    void OnSetFocus(HWND hWndOld) override;
};
