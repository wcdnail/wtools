#pragma once

#include "luicPageImpl.h"
#include <map>

struct CLegacyUIConfigurator;

struct CMainFrame: CPageImpl
{
    using  PagesMap = std::map<int, CPageImplPtr>;

    ~CMainFrame() override;
    CMainFrame(CLegacyUIConfigurator& app);

private:
    CLegacyUIConfigurator& m_App;
    WTL::CTabCtrl          m_Tab;
    CImageList          m_ImList;
    PagesMap          m_PagesMap;
    CRect          m_rcTabClient;

    CPageImplPtr const& PagesGet(int numba) const;
    CPageImplPtr const& PagesGetCurrent() const;
    
    void ImListCreate();
    void PagesGetRect();
    void PagesAppend(int desiredIndex, ATL::CStringW&& str, CPageImplPtr&& pagePtr);
    void PagesCreate();
    void PagesShow(int numba, bool show);

    void OnResizeNotify() override;
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    LRESULT OnNotify(int idCtrl, LPNMHDR pnmh) override;
    void OnDestroy() override;
};
