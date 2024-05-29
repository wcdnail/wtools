#pragma once

#include "CColorPicker.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include "CColorStatic.h"
#include "CMagnifierCtrl.h"
#include "CColorHistory.h"
#include <wcdafx.api.h>
#include <Magnification.h>
#include <atlddx.h>

struct CColorPicker::Impl: private WTL::CIndirectDialogImpl<Impl>,
                           private WTL::CDialogResize<Impl>,
                           private WTL::CWinDataExchange<Impl>
{
    DELETE_COPY_MOVE_OF(Impl);

    ~Impl() override;
    Impl();

private:
    friend CColorPicker;
    friend WTL::CDialogResize<Impl>;
    friend WTL::CWinDataExchange<Impl>;
    friend WTL::CIndirectDialogImpl<Impl>;

    HRESULT PreCreateWindow();

    static int         gs_nRasterCX;
    static CColorHistory gs_History;

    bool                m_bSaveData;
    CSpectrumImage     m_imSpectrum;
    CSpectrumSlider      m_imSlider;
    CColorStatic          m_stColor;
    CMagnifierCtrl      m_Magnifier;
    COLORREF*       m_pTrackedColor;
    ATL::CWindow        m_stHistory;

    // DDX variables
    int             m_nSpectrumKind;
    ATL::CString        m_sColorHex;
    ATL::CString       m_sColorHtml;

    // Appearance
    WTL::CFont             m_fntHex;
    WTL::CCursor         m_curArrow;
    WTL::CCursor        m_curPicker;
    WTL::CCursor         m_curCross;

    BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = static_cast<UINT>(-1));
    void DoInitTemplate();
    void DoInitControls();
    static const WTL::_AtlDlgResizeMap* GetDlgResizeMap();
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    void SpectruKindChanged();
    LRESULT SliderChanged(bool bNotify);
    void UpdateHexStr();
    void UpdateHtmlStr();
    void UpdateDDX();
    void UpdateColorStatic();
    void DDXReloadEditsExcept(int nId);
    LRESULT ColorChanged(bool bUpdateDDX);
    void OnDDXLoading(UINT nID, BOOL bSaveAndValidate);
    LRESULT OnNotify(int nID, LPNMHDR pnmh);
    void ValidateHexInput(WTL::CEdit& edCtrl);
    void OnEditUpdate(int nID);
    void OnCommand(UINT uNotifyCode, int nID, HWND);
    void TogglePalette(BOOL bPalVisible) const;
    void ColorpickBegin();
    void HistoryStore();
    void HistoryPick();
    void SetColorRef(COLORREF crColor, bool bStoreToHistory);
    void GetColorFromWindowDC(CPoint const& pt, bool bStoreToHistory);
    void GetColorFromDesktopDC(CPoint const& pt, bool bStoreToHistory);
    void ColorpickEnd(UINT, CPoint const&, bool bSelect);
    void OnTimer(UINT_PTR nIDEvent);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI) const;
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
};
