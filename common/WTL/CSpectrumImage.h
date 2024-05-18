#pragma once

#include <DDraw.DGI/CeXDib.h>
#include <atlwin.h>

enum SpectrumIndex: int
{
    SPEC_RGB_Red = 0,
    SPEC_RGB_Green,
    SPEC_RGB_Blue,
    SPEC_HSV_Hue,
    SPEC_HSV_Saturation,
    SPEC_HSV_Brightness,
};

struct CSpectrumImage: ATL::CWindowImpl<CSpectrumImage>
{
    using Super = ATL::CWindowImpl<CSpectrumImage>;

    DECLARE_WND_CLASS(_T("WCCF::CSpectrumImage"))

    ~CSpectrumImage() override;
    CSpectrumImage();

    HRESULT PreCreateWindow();

private:
    friend Super;

    static ATOM           gs_Atom;
    CDIBitmap               m_Dib;
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
