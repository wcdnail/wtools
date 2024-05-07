#pragma once

#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <functional>
#include <vector>
#include <memory>

struct CTheme;
struct CPageImpl;

using  CPageImplPtr = std::unique_ptr<CPageImpl>;

struct CPageImpl: ATL::CDialogImpl<CPageImpl>,
                  WTL::CDialogResize<CPageImpl>,
                  WTL::CMessageFilter
{
    using     Super = ATL::CDialogImpl<CPageImpl>;
    using   Resizer = WTL::CDialogResize<CPageImpl>;
    using ResizeVec = std::vector<WTL::_AtlDlgResizeMap>;
    using ForeachFn = std::function<bool(HWND)>;

    ~CPageImpl() override;

    BOOL PreTranslateMessage(MSG* pMsg) override;
    virtual HWND CreateDlg(HWND hWndParent, LPARAM dwInitParam = NULL);
    virtual void SelectAll();
    PCWSTR GetCaption() const;

protected:
    friend class Super;
    friend class Resizer;

    UINT               IDD;
    std::wstring m_Caption;
    CTheme*       m_pTheme;

    CPageImpl(UINT idd, std::wstring&& caption, CTheme* pTheme = nullptr);

    static bool ComboSetCurSelByData(WTL::CComboBox& cbControl, DWORD_PTR nData);
    static bool CtlDisable(HWND hWndCtl);
    static BOOL DoForEachImpl(HWND hWndCtl, ForeachFn const& routine);
    void DoForEach(ForeachFn const& routine);

    void DlgResizeAdd(int nCtlID, DWORD dwResizeFlags);
    void DlgResizeAdd(WTL::_AtlDlgResizeMap const* vec, size_t count);

    HBRUSH OnCtlColorStatic(WTL::CDCHandle dc, HWND wndStatic);
    HBRUSH OnEraseBkgnd(WTL::CDCHandle dc);

    virtual void OnResizeNotify();
    virtual BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
    virtual void OnDestroy();
    virtual void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
    virtual LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
    virtual void OnDropFiles(HDROP hDropInfo);
    virtual void OnSetFocus(HWND hWndOld);

private:
    ResizeVec m_ResiseMap;

    BEGIN_MSG_MAP_EX(CColorsPage)
      //if (nullptr != m_pTheme) {
      //    MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
      //    MSG_WM_ERASEBKGND(OnEraseBkgnd)
      //}
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
    END_MSG_MAP()

    WTL::_AtlDlgResizeMap const* GetDlgResizeMap() const;

    CPageImpl(CPageImpl const&) = delete;
    CPageImpl(CPageImpl&&) = delete;
    CPageImpl& operator = (CPageImpl const&) = delete;
    CPageImpl& operator = (CPageImpl&&) = delete;
};

inline PCWSTR CPageImpl::GetCaption() const { return m_Caption.c_str(); }

