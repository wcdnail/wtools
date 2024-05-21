#pragma once

#include "CColorPickerDefs.h"
#include "CCustomCtrl.h"
#include <DDraw.DGI/CeXDib.h>
#include <atlctrls.h>
#include <atlgdi.h>

struct CRGBSpecRect;

struct CSpectrumImage: CCustomControl<CSpectrumImage>
{
    using Super = CCustomControl<CSpectrumImage>;

    DECLARE_WND_SUPERCLASS2(CSPECIMG_CLASS, CSpectrumImage, nullptr)

    ~CSpectrumImage() override;
    CSpectrumImage(COLORREF crInit, SpectrumKind kind);

    bool Initialize(long cx = SPECTRUM_CX, long cy = SPECTRUM_CY);
    COLORREF GetColorRef() const;
    CRGBSpecRect GetRGBSpectrumRect() const;
    CColorUnion& GetColorUnion();
    void SetSpectrumKind(SpectrumKind kind);
    SpectrumKind GetSpectrumKind() const;
    void OnSliderChanged(long nPos);
    void OnColorChanged(long xPos, long yPos);

private:
    friend Super;

    CDIBitmap               m_Dib;
    CColorUnion           m_Color;
    CPoint                m_ptSel;
    SpectrumKind   m_SpectrumKind;

    static ATOM& GetWndClassAtomRef();

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    void UpdateRaster();
    void DrawMarker(WTL::CDCHandle dc) const;

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(WTL::CDCHandle dc);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point) const;
    void NotifySend() const;
    void NotifyColorChanged(CRect const& rcClient);
    void OnMouseMove(UINT nFlags, CPoint pt);
};

inline SpectrumKind CSpectrumImage::GetSpectrumKind() const
{
    return m_SpectrumKind;
}

inline COLORREF CSpectrumImage::GetColorRef() const
{
    return m_Color.GetColorRef();
}

inline CColorUnion& CSpectrumImage::GetColorUnion()
{
    return m_Color;
}
