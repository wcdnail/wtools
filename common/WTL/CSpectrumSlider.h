#pragma once

#include "CColorPickerDefs.h"
#include "CCustomCtrl.h"
#include <DDraw.DGI/CeXDib.h>
#include <wcdafx.api.h>

enum SpectrumKind: int;

struct CSpectrumSlider: CCustomControl<CSpectrumSlider>
{
    using Super = CCustomControl<CSpectrumSlider>;

    DELETE_COPY_MOVE_OF(CSpectrumSlider);
    DECLARE_WND_SUPERCLASS2(CSPECSLD_CLASS, CSpectrumSlider, nullptr)

    ~CSpectrumSlider() override;
    CSpectrumSlider(SpectrumKind const& nKind, CColorUnion& unColor);

    bool Initialize(long cx = SPECTRUM_SLIDER_CX);
    void UpdateRaster();

private:
    friend Super;

    SpectrumKind const& m_nKind;
    CColorUnion&      m_unColor;
    CDIBitmap             m_Dib;

    static ATOM& GetWndClassAtomRef();
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    int OnCreate(LPCREATESTRUCT pCS);
    double GetValueAndRange(long& nPos) const;
    void ColorChanged() const;
    void SetValue(long nPos, long nMax) const;
    void SetValueByY(long nY) const;
    void NotifySend() const;
    void DrawRaster(WTL::CDCHandle dc, CRect const& rc) const;
    void DrawPosition(WTL::CDCHandle dc, CRect const& rc) const;
    void OnPaint(WTL::CDCHandle dc) const;
    void OnLButtonDown(UINT nFlags, CPoint pt);
    void OnLButtonUp(UINT nFlags, CPoint point) const;
    void OnMouseMove(UINT nFlags, CPoint pt);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
