#pragma once

#include "CCustomCtrl.h"
#include "CSliderCtrl.h"
#include <DDraw.DGI/CeXDib.h>
#include <wcdafx.api.h>

#define CSPECSLD_CLASS _T("WCCF::CSpectrumSlider")

enum SpectrumKind: int;

struct CSpectrumSlider: CCustomControl<CSpectrumSlider, WTL::CSliderCtrl>
{
    using Super = CCustomControl<CSpectrumSlider, WTL::CSliderCtrl>;

    DELETE_COPY_MOVE_OF(CSpectrumSlider);
    DECLARE_WND_SUPERCLASS2(CSPECSLD_CLASS, CSpectrumSlider, WTL::CSliderCtrl::GetWndClassName())

    ~CSpectrumSlider() override;
    CSpectrumSlider();

    bool Initialize();
    void UpdateRaster(SpectrumKind spKind, double dHue);

private:
    friend Super;

    BOOL            m_bMsgHandled;
    COLORREF          m_crPrimary;
    COLORREF           m_crShadow;
    COLORREF           m_crHilite;
    COLORREF        m_crMidShadow;
    COLORREF     m_crDarkerShadow;
    WTL::CBrush     m_normalBrush;
    WTL::CBrush      m_focusBrush;
    CDIBitmap               m_Dib;

    static ATOM& GetWndClassAtomRef();

    void SetPrimaryColor(COLORREF crPrimary);

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    int OnCreate(LPCREATESTRUCT pCS);
    CRect GetRatserRect(DWORD dwStyle, NMCUSTOMDRAW const& nmcd, CRect& rcClient) const;
    CRect& SetThumbRect(NMCUSTOMDRAW const& nmcd, CRect const& rcClient) const;
    void DrawRasterChannel(NMCUSTOMDRAW const& nmcd);
    LRESULT OnCustomDraw(LPNMHDR pNMHDR);
};
