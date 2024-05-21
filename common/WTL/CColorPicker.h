#pragma once

#include "CCustomCtrl.h"
#include <atlgdi.h>
#include <atlapp.h>
#include <memory>

struct CColorPicker: CCustomControl<CColorPicker>,
                     WTL::CMessageFilter
{
    using Super = CCustomControl<CColorPicker>;

    DECLARE_WND_CLASS(_T("WCCF::CColorPicker"))

    ~CColorPicker() override;
    CColorPicker();

    HRESULT PreCreateWindow() override;
    BOOL PreTranslateMessage(MSG* pMsg) override;

private:
    friend Super;
    struct Impl;

    std::unique_ptr<Impl> m_pImpl;

    static ATOM& GetWndClassAtomRef();
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    void OnNcPaint(WTL::CRgnHandle rgn);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnSize(UINT nType, CSize size) const;
};
