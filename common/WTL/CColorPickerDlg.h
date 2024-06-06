#pragma once

#include "CColorPicker.h"
#include <wcdafx.api.h>
#include <rect.putinto.h>
#include <atltypes.h>

#include "IColorObserver.h"

struct CColorPickerDlg: private WTL::CIndirectDialogImpl<CColorPickerDlg>,
                        private WTL::CDialogResize<CColorPickerDlg>,
                        private WTL::CMessageFilter,
                        private IColorObserver
{
    using WndSuper = WTL::CIndirectDialogImpl<CColorPickerDlg>;

    DELETE_COPY_MOVE_OF(CColorPickerDlg);

    WCDAFX_API ~CColorPickerDlg() override;
    WCDAFX_API CColorPickerDlg(COLORREF crColor);

    WCDAFX_API HRESULT Initialize();
    WCDAFX_API bool Show(HWND hWndMaster, unsigned nPosFlags, bool bModal = false);

    COLORREF GetColor() const;
    IColor& GetMasterColor() const;
    IColorObserver& GetMasterObserver() const;

    WCDAFX_API void MoveWindow(unsigned nPosFlags);

private:
    friend WTL::CIndirectDialogImpl<CColorPickerDlg>;
    friend WTL::CDialogResize<CColorPickerDlg>;

    ATL::CWindow     m_wndMaster;
    CColorPicker   m_ColorPicker;
    CRect              m_rcPlace;
    bool            m_bModalLoop;
    unsigned         m_nPosFlags;
    WTL::CFont           m_vFont;

    const WTL::_AtlDlgResizeMap* GetDlgResizeMap() const;
    void PrepareRect(ATL::CWindow wndParent);
    void DoInitTemplate();
    void DoInitControls();
    void OnColorUpdate(IColor const& clrSource) override;
    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, _Inout_ LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
    void OnCommand(UINT uNotifyCode, int nID, HWND);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI) const;
    UINT OnNcHitTest(CPoint point) const;
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
