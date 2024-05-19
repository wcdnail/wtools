#pragma once

#include "CColorPickerDefs.h"
#include "CCustomCtrl.h"
#include <DDraw.DGI/CeXDib.h>

struct CSpectrumImage: CCustomControl<CSpectrumImage>
{
    using Super = CCustomControl<CSpectrumImage>;

    DECLARE_WND_SUPERCLASS2(CSPECIMG_CLASS, CSpectrumImage, nullptr)

    ~CSpectrumImage() override;
    CSpectrumImage();

    bool Initialize(CSpectrumSlider& imSlider, long cx = SPECTRUM_CX, long cy = SPECTRUM_CY);
    COLORREF GetColorRef() const;
    void SetSpectrumKind(SpectrumKind kind);
    SpectrumKind GetSpectrumKind() const;
    void OnSliderChanged(long nPos);

private:
    friend Super;

    BOOL            m_bMsgHandled;
    CDIBitmap               m_Dib;
    CColorUnion           m_Color;
    SpectrumKind   m_SpectrumKind;
    CSpectrumSlider*  m_pimSlider;

    static ATOM& GetWndClassAtomRef();

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    void UpdateRaster();

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(WTL::CDCHandle dc);
};

inline SpectrumKind CSpectrumImage::GetSpectrumKind() const
{
    return m_SpectrumKind;
}

inline COLORREF CSpectrumImage::GetColorRef() const
{
    return m_Color.m_Comp.Color;
}
