#pragma once

#include "CCustomCtrl.h"
#include <wcdafx.api.h>
#include <atlapp.h>
#include <memory>

class IColorObserver;
class IColor;

struct CColorPicker: private CCustomControl<CColorPicker>,
                     private WTL::CMessageFilter
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
    WCDAFX_API CColorPicker(COLORREF crColor);

    WCDAFX_API HRESULT PreCreateWindow() override;
    WCDAFX_API BOOL PreTranslateMessage(MSG* pMsg) override;

    WCDAFX_API COLORREF GetColorRef() const;
    WCDAFX_API int GetAlpha() const;
    WCDAFX_API void SetColor(COLORREF crColor, int nAlpha, bool bStoreToHistory, bool bNotifyObservers) const;

    WCDAFX_API IColor& GetMasterColor() const;
    WCDAFX_API IColorObserver& GetMasterObserver() const;

    WCDAFX_API static int& RasterCX();
    WCDAFX_API static void OnWindowPosChanging(LPWINDOWPOS pWPos);

    using WndSuper::m_hWnd;
    using WndSuper::IsWindow;
    using WndSuper::InvalidateRect;

private:
    friend CCustomControl<CColorPicker>;
    friend WndSuper;

    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnSize(UINT nType, CSize size) const;
};
