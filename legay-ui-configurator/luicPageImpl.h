#pragma once

#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <vector>
#include <memory>

struct CPageImpl;

using  CPageImplPtr = std::unique_ptr<CPageImpl>;

struct CPageImpl: ATL::CDialogImpl<CPageImpl>,
                  WTL::CDialogResize<CPageImpl>,
                  CMessageFilter
{
    using     Super = ATL::CDialogImpl<CPageImpl>;
    using   Resizer = WTL::CDialogResize<CPageImpl>;
    using ResizeVec = std::vector<WTL::_AtlDlgResizeMap>;

    ~CPageImpl() override;

    BOOL PreTranslateMessage(MSG* pMsg) override;
    virtual HWND CreateDlg(HWND hWndParent, LPARAM dwInitParam = NULL);
    PCWSTR GetCaption() const;

protected:
    friend class Super;
    friend class Resizer;

    UINT               IDD;
    std::wstring m_Caption;

    CPageImpl(UINT idd, std::wstring&& caption);

    void DlgResizeAdd(int nCtlID, DWORD dwResizeFlags);
    void DlgResizeAdd(WTL::_AtlDlgResizeMap const* vec, size_t count);

    virtual void OnResizeNotify();
    virtual BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    virtual void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
    virtual LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
    virtual void OnDropFiles(HDROP hDropInfo);
    virtual void OnDestroy();

    HBRUSH OnCtlColorStatic(CDCHandle dc, HWND wndStatic);
    HBRUSH OnEraseBkgnd(CDCHandle dc);

private:
    ResizeVec m_ResiseMap;

    BEGIN_MSG_MAP_EX(CColorsPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_DROPFILES(OnDropFiles)
        MSG_WM_DESTROY(OnDestroy)
        if (Resizer::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {
            OnResizeNotify();
            return TRUE;
        }
    END_MSG_MAP()

    WTL::_AtlDlgResizeMap const* GetDlgResizeMap() const;

    CPageImpl(CPageImpl const&) = delete;
    CPageImpl(CPageImpl&&) = delete;
    CPageImpl& operator = (CPageImpl const&) = delete;
    CPageImpl& operator = (CPageImpl&&) = delete;
};

inline PCWSTR CPageImpl::GetCaption() const { return m_Caption.c_str(); }

