#pragma once

#include <atlwin.h>
#include <atlcrack.h>
#include <memory>

struct CSpectrumColorPicker: ATL::CWindowImpl<CSpectrumColorPicker>
{
    using Super = ATL::CWindowImpl<CSpectrumColorPicker>;

    DECLARE_WND_CLASS(_T("CSpectrumColorPicker"))

    ~CSpectrumColorPicker() override;
    CSpectrumColorPicker();

    HRESULT PreCreateWindow();
    CSpectrumColorPicker& operator = (HWND hWnd); // make compatible with WTL/DDX

private:
    friend Super;
    struct Impl;

    static ATOM           gs_Atom;
    std::unique_ptr<Impl> m_pImpl;

    BEGIN_MSG_MAP_EX(CSpectrumColorPicker)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnSize(UINT nType, CSize size);
};
