#pragma once

#include "CCustomCtrl.h"
#include "CColorTarget.h"
#include <wcdafx.api.h>
#include <atlgdi.h>
#include <atlapp.h>
#include <memory>


struct CColorPicker: private CCustomControl,
                     private WTL::CMessageFilter,
                     public  IColorTarget
{
    enum Sizes: short
    {
        HOST_DLG_CX  = 364,
        HOST_DLG_CY  = 182,
    };

    static constexpr auto    szDlgFont{_T("Cascadia Mono Light")};
    static constexpr WORD wDlgFontSize{8};

    DECLARE_WND_CLASS(_T("WCCF::CColorPicker"))
    DELETE_COPY_MOVE_OF(CColorPicker);

    WCDAFX_API ~CColorPicker() override;
    WCDAFX_API CColorPicker();

    WCDAFX_API HRESULT PreCreateWindow() override;
    WCDAFX_API BOOL PreTranslateMessage(MSG* pMsg) override;

    WCDAFX_API COLORREF GetColorRef() const override;
    WCDAFX_API int GetAlpha() const override;
    WCDAFX_API void SetColor(COLORREF crColor, int nAlpha = 255) override;

    WCDAFX_API bool SetColorTarget(CColorTarget crTarget);

    WCDAFX_API static int& RasterCX();

    using WndSuper::IsWindow;
    using WndSuper::InvalidateRect;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    void OnNcPaint(WTL::CRgnHandle rgn);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnSize(UINT nType, CSize size) const;

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
};
