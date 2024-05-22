#pragma once

#include "CColorPickerDefs.h"
#include "CCustomCtrl.h"
#include "CSliderCtrl.h"
#include <DDraw.DGI/CeXDib.h>
#include <wcdafx.api.h>
#include <atlctrls.h>

enum SpectrumKind: int;

struct CSpectrumSlider: CCustomControl<CSpectrumSlider, WTL::CSliderCtrl>
{
    using Super = CCustomControl<CSpectrumSlider, WTL::CSliderCtrl>;

    DELETE_COPY_MOVE_OF(CSpectrumSlider);
    DECLARE_WND_SUPERCLASS2(CSPECSLD_CLASS, CSpectrumSlider, WTL::CSliderCtrl::GetWndClassName())

    ~CSpectrumSlider() override;
    CSpectrumSlider();

    bool Initialize(long cx = SPECTRUM_SLIDER_CX);
    void UpdateRaster(SpectrumKind spKind, CColorUnion const& unColor);

private:
    friend Super;

    bool  m_bCapture;
    CDIBitmap  m_Dib;
    CRect m_rcRaster;

    static ATOM& GetWndClassAtomRef();
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    int OnCreate(LPCREATESTRUCT pCS);
    CRect GetRatserRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const;
    CRect GetThumbRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const;
    void DrawRasterChannel(NMCUSTOMDRAW const& nmcd, DWORD dwStyle);
    LRESULT OnCustomDraw(LPNMHDR pNMHDR);
    void OnLButtonDown(UINT nFlags, CPoint pt);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnMouseMove(UINT nFlags, CPoint pt);
};
