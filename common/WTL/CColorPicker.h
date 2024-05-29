#pragma once

#include "CCustomCtrl.h"
#include <wcdafx.api.h>
#include <atlgdi.h>
#include <atlapp.h>
#include <memory>

struct CColorPicker: CCustomControl, WTL::CMessageFilter
{
    enum Sizes: short
    {
        DLG_CX  = 360,
        DLG_CY  = 180,
    };

    DECLARE_WND_CLASS(_T("WCCF::CColorPicker"))
    DELETE_COPY_MOVE_OF(CColorPicker);

    WCDAFX_API ~CColorPicker() override;
    WCDAFX_API CColorPicker();

    WCDAFX_API HRESULT PreCreateWindow() override;
    WCDAFX_API BOOL PreTranslateMessage(MSG* pMsg) override;

    WCDAFX_API COLORREF GetColorRef() const;
    WCDAFX_API bool SetTracked(COLORREF& crTarget) const;

    WCDAFX_API static int& RasterCX();

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    void OnNcPaint(WTL::CRgnHandle rgn);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnSize(UINT nType, CSize size) const;

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
};
