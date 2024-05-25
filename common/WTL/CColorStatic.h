#pragma once

#include <wcdafx.api.h>

void DrawColorRect(WTL::CDCHandle dc, CRect const& rc, COLORREF crColor, int nAlpha, HBRUSH brBack);

struct CColorStatic: private ATL::CWindowImpl<CColorStatic, WTL::CStatic>,
                     private WTL::CBufferedPaintImpl<CColorStatic>
{
    using WndSuper = ATL::CWindowImpl<CColorStatic, WTL::CStatic>;

    DELETE_COPY_MOVE_OF(CColorStatic);

    WCDAFX_API ~CColorStatic() override;
    WCDAFX_API CColorStatic(COLORREF crColor = 0xffffff);

    void Reset(COLORREF crColor, int nAlpha = 255, HBRUSH brBack = nullptr);

    using WndSuper::m_hWnd;
    using WndSuper::SubclassWindow;
    using WndSuper::Create;

protected:
    friend WTL::CBufferedPaintImpl<CColorStatic>;

    COLORREF         m_crColor;
    int               m_nAlpha;
    WTL::CBrushHandle m_brBack;

    void DoPaint(WTL::CDCHandle dc, CRect const& rc) const;
    WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
};

inline void CColorStatic::Reset(COLORREF crColor, int nAlpha, HBRUSH brBack)
{
    m_crColor = crColor;
    m_nAlpha  = nAlpha;
    if (brBack) {
        m_brBack = brBack;
    }
    InvalidateRect(nullptr, FALSE);
}
