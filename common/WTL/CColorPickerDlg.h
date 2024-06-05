#pragma once

#include "CColorPicker.h"
#include <wcdafx.api.h>
#include <rect.putinto.h>
#include <atltypes.h>

struct CColorPickerDlg: private WTL::CIndirectDialogImpl<CColorPickerDlg>,
                        private WTL::CDialogResize<CColorPickerDlg>,
                        private WTL::CMessageFilter
{
    DELETE_COPY_MOVE_OF(CColorPickerDlg);

    WCDAFX_API ~CColorPickerDlg() override;
    WCDAFX_API CColorPickerDlg(COLORREF crColor);

    WCDAFX_API HRESULT Initialize();
    WCDAFX_API bool Show(HWND hWndMaster, unsigned nPosFlags, bool bModal = false);

    COLORREF GetColor() const;
    IColor& GetMasterColor() const;
    IColorObserver& GetMasterObserver() const;

private:
    friend WTL::CIndirectDialogImpl<CColorPickerDlg>;
    friend WTL::CDialogResize<CColorPickerDlg>;

    ATL::CWindow     m_wndMaster;
    CColorPicker   m_ColorPicker;
    CRect              m_rcPlace;
    bool            m_bModalLoop;
    unsigned         m_nPosFlags;

    const WTL::_AtlDlgResizeMap* GetDlgResizeMap() const;
    void PrepareRect(ATL::CWindow wndParent);
    void DoInitTemplate();
    void DoInitControls();
    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL ProcessWindowMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_ LRESULT& lResult, _In_ DWORD dwMsgMapID = 0) override;
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
    void OnCommand(UINT uNotifyCode, int nID, HWND);
};

inline HRESULT CColorPickerDlg::Initialize()
{
    return m_ColorPicker.PreCreateWindow();
}

inline COLORREF CColorPickerDlg::GetColor() const
{
    return m_ColorPicker.GetColorRef();
}

inline IColor& CColorPickerDlg::GetMasterColor() const
{
    return m_ColorPicker.GetMasterColor();
}

inline IColorObserver& CColorPickerDlg::GetMasterObserver() const
{
    return m_ColorPicker.GetMasterObserver();
}
