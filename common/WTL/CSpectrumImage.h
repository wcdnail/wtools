#pragma once

#include "CColorPickerDefs.h"
#include "CCustomCtrl.h"
#include <wcdafx.api.h>
#include <DDraw.DGI/CeXDib.h>
#include <atlgdi.h>

struct CRGBSpecRect;

struct CSpectrumImage: CCustomControl<CSpectrumImage>
{
    using Super = CCustomControl<CSpectrumImage>;

    DELETE_COPY_MOVE_OF(CSpectrumImage);
    DECLARE_WND_SUPERCLASS2(CSPECIMG_CLASS, CSpectrumImage, nullptr)

    ~CSpectrumImage() override;
    CSpectrumImage(COLORREF crInit, SpectrumKind kind);

    bool Initialize(long cx = SPECTRUM_CX, long cy = SPECTRUM_CY);
    COLORREF GetColorRef() const;
    CColorUnion& GetColor();
    void SetSpectrumKind(SpectrumKind kind);
    SpectrumKind GetSpectrumKind() const;
    SpectrumKind const& GetSpectrumKindRef() const;
    void OnColorChanged(double xPos, double yPos);
    void NotifySend() const;

private:
    friend Super;

    CDIBitmap             m_Dib;
    CColorUnion         m_Color;
    SpectrumKind m_SpectrumKind;
    WTL::CBrush    m_brCheckers;

    static ATOM& GetWndClassAtomRef();

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    void UpdateRaster();
    void DrawMarker(WTL::CDCHandle dc, CRect const& rc);

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(WTL::CDCHandle dc);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point) const;
    void NotifyColorChanged(CRect const& rc, CPoint pt);
    void OnMouseMove(UINT nFlags, CPoint pt);
};

inline SpectrumKind CSpectrumImage::GetSpectrumKind() const
{
    return m_SpectrumKind;
}

inline SpectrumKind const& CSpectrumImage::GetSpectrumKindRef() const
{
    return m_SpectrumKind;
}

inline COLORREF CSpectrumImage::GetColorRef() const
{
    return m_Color.GetColorRef();
}

inline CColorUnion& CSpectrumImage::GetColor()
{
    return m_Color;
}
