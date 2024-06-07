#pragma once

#include "CColorPicker.h"
#include "IColorObserver.h"
#include <wcdafx.api.h>
#include <rect.putinto.h>
#include <atltypes.h>
#include <atltheme.h>

struct CColorPickerDlg: private WTL::CIndirectDialogImpl<CColorPickerDlg>,
                        private WTL::CDialogResize<CColorPickerDlg>,
                        private WTL::CThemeImpl<CColorPickerDlg>,
                        private WTL::CBufferedPaintImpl<CColorPickerDlg>,
                        private WTL::CMessageFilter,
                        public  IColorObserver
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
    WCDAFX_API void FollowMaster(LPWINDOWPOS pWndPos);

private:
    friend WTL::CIndirectDialogImpl<CColorPickerDlg>;
    friend WTL::CDialogResize<CColorPickerDlg>;

    CColorPicker   m_ColorPicker;
    CRect              m_rcPlace;
    bool            m_bModalLoop;
    unsigned         m_nPosFlags;
    WTL::CFont           m_vFont;
    ATL::CWindow     m_wndMaster;
    CRect             m_rcMaster;

    const WTL::_AtlDlgResizeMap* GetDlgResizeMap() const;
    bool PrepareRect(ATL::CWindow wndParent);
    void DoInitTemplate();
    void DoInitControls();
    void OnColorUpdate(IColor const& clrSource) override;
    BOOL PreTranslateMessage(MSG* pMsg) override;
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, _Inout_ LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
    void OnCommand(UINT uNotifyCode, int nID, HWND);
    HRESULT ThemedInit();
    void DrawDragBar(int nID, WTL::CDCHandle dc, CRect& rcItem);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI);
    UINT OnNcHitTest(CPoint point) const;
    void OnWindowPosx(LPWINDOWPOS pWPos);
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
