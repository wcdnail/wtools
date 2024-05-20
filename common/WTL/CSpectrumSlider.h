#pragma once

#include "CColorPickerDefs.h"
#include "CCustomCtrl.h"
#include "CSliderCtrl.h"
#include <DDraw.DGI/CeXDib.h>
#include <wcdafx.api.h>

enum SpectrumKind: int;

struct CSpectrumSlider: CCustomControl<CSpectrumSlider, WTL::CSliderCtrl>
{
    using Super = CCustomControl<CSpectrumSlider, WTL::CSliderCtrl>;

    DELETE_COPY_MOVE_OF(CSpectrumSlider);
    DECLARE_WND_SUPERCLASS2(CSPECSLD_CLASS, CSpectrumSlider, WTL::CSliderCtrl::GetWndClassName())

    ~CSpectrumSlider() override;
    CSpectrumSlider();

    bool Initialize(CSpectrumImage& imSpectrum);
    void UpdateRaster(SpectrumKind spKind, CColorUnion const& unColor);

private:
    friend Super;

    BOOL            m_bMsgHandled;
    CDIBitmap               m_Dib;
    CSpectrumImage* m_pimSpectrum;
    CRect              m_rcRaster;
    bool               m_bCapture;

    static ATOM& GetWndClassAtomRef();

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    int OnCreate(LPCREATESTRUCT pCS);
    CRect GetRatserRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const;
    CRect GetThumbRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const;
    void DrawRasterChannel(NMCUSTOMDRAW const& nmcd, DWORD dwStyle);
    LRESULT OnCustomDraw(LPNMHDR pNMHDR);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnWMScroll(UINT nSBCode, UINT nPos, WTL::CScrollBar ctlScrollBar);
};
