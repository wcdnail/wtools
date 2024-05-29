#pragma once

#include "CColorPicker.h"
#include <wcdafx.api.h>
#include <atltypes.h>

struct CColorPickerDlg: private WTL::CIndirectDialogImpl<CColorPickerDlg>,
                        private WTL::CDialogResize<CColorPickerDlg>,
                        private WTL::CMessageFilter
{
    DELETE_COPY_MOVE_OF(CColorPickerDlg);

    WCDAFX_API ~CColorPickerDlg() override;
    WCDAFX_API CColorPickerDlg();

    WCDAFX_API HRESULT Initialize();
    WCDAFX_API bool Show(HWND hWndMaster, COLORREF& crTarget, bool bModal = false);

    COLORREF GetColor() const;

private:
    friend WTL::CIndirectDialogImpl<CColorPickerDlg>;
    friend WTL::CDialogResize<CColorPickerDlg>;

    ATL::CWindow     m_wndMaster;
    CColorPicker m_ccColorPicker;
    CRect              m_rcPlace;

    static const WTL::_AtlDlgResizeMap* GetDlgResizeMap();
    void PrepareRect(ATL::CWindow wndParent);
    void DoInitTemplate();
    void DoInitControls();
    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL ProcessWindowMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_ LRESULT& lResult, _In_ DWORD dwMsgMapID = 0) override;
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

inline COLORREF CColorPickerDlg::GetColor() const
{
    return m_ccColorPicker.GetColorRef();
}
