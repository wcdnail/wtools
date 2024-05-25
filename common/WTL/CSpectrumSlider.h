#pragma once

#include "CColorPickerDefs.h"
#include "CDDCtl.h"
#include <wcdafx.api.h>

enum SpectrumKind: int;

struct CSpectrumSlider: CDDCtrl
{
    DELETE_COPY_MOVE_OF(CSpectrumSlider);

    DECLARE_WND_SUPERCLASS2(CSPECSLD_CLASS, CSpectrumSlider, nullptr)

    WCDAFX_API ~CSpectrumSlider() override;
    WCDAFX_API CSpectrumSlider(SpectrumKind const& nKind, CColorUnion& unColor);

    WCDAFX_API HRESULT PreCreateWindow() override;
    WCDAFX_API bool Initialize(long cx, HBRUSH brBackground);
    WCDAFX_API void UpdateRaster();

private:
    friend WndSuper;

    SpectrumKind const& m_nKind;
    CColorUnion&      m_unColor;
    CDIBitmap          m_bmVert;

    double GetValueAndRange(long& nPos) const;
    void ColorChanged() const;
    void SetValue(long nPos, long nMax) const;
    void SetValueByY(long nY) const;
    void DrawPosition(WTL::CDCHandle dc, CRect const& rc) const;
    void DoPaint(WTL::CDCHandle dc, CRect const& rc) override;
    void DoNotifyMouseOver(CRect const& rc, CPoint pt) override;
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    UINT OnGetDlgCode(LPMSG lpMsg) const;
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
