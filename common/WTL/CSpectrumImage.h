#pragma once

#include "CCustomCtrl.h"
#include <DDraw.DGI/CeXDib.h>

enum SpectrumKind: int
{
    SPEC_Begin = 0,
    SPEC_RGB_Red = SPEC_Begin,
    SPEC_RGB_Green,
    SPEC_RGB_Blue,
    SPEC_HSV_Hue,
    SPEC_HSV_Saturation,
    SPEC_HSV_Brightness,
    SPEC_End = SPEC_HSV_Brightness
};

struct CSpectrumImage: CCustomControl<CSpectrumImage>
{
    using Super = CCustomControl<CSpectrumImage>;

    DECLARE_WND_CLASS(_T("WCCF::CSpectrumImage"))

    ~CSpectrumImage() override;
    CSpectrumImage();

    void SetSpectrumKind(SpectrumKind kind);

private:
    friend Super;
   
    CDIBitmap               m_Dib;
    SpectrumKind   m_SpectrumKind;
    double                 m_dHue;
    double             m_dPrevHue;
    BOOL            m_bMsgHandled;

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(WTL::CDCHandle dc);
};
