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

    void OnDataChanged(SpectrumKind kind, CSpectrumSlider& imSlider);
    SpectrumKind GetSpectrumKind() const;

private:
    friend Super;

    CDIBitmap               m_Dib;
    SpectrumKind   m_SpectrumKind;
    double                 m_dHue;
    double             m_dPrevHue;
    CSpectrumSlider*  m_pimSlider;
    BOOL            m_bMsgHandled;

    static ATOM& GetWndClassAtomRef();

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    void UpdateRaster() const;

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(WTL::CDCHandle dc);
};

inline SpectrumKind CSpectrumImage::GetSpectrumKind() const
{
    return m_SpectrumKind;
}
