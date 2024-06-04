#pragma once

#include "CCustomCtrl.h"
#include <wcdafx.api.h>
#include <DDraw.DGI/CeXDib.h>
#include <atlgdi.h>
#include <atltheme.h>

constexpr int             CHECKERS_CX{18};
constexpr COLORREF CLR_CHECKERS_WHITE{RGB(210, 210, 210)};
constexpr COLORREF CLR_CHECKERS_BLACK{RGB( 92,  92,  92)};

struct CDDCtrl: public  CCustomControl<CDDCtrl>,
                private WTL::CBufferedPaintImpl<CDDCtrl>
{
    DELETE_COPY_MOVE_OF(CDDCtrl);

    WCDAFX_API ~CDDCtrl() override;
    WCDAFX_API CDDCtrl();

    WCDAFX_API bool Initialize(long cx, long cy, long bpp, HBRUSH brBackground, HCURSOR hCursor);
    WCDAFX_API void NotifyMaster(UINT code) const;

    HBRUSH GetBackBrush() const;

protected:
    friend WTL::CBufferedPaintImpl<CDDCtrl>;

    CDIBitmap      m_Dib;
    WTL::CBrush m_brBack;
    bool    m_bBackOwner;
    HCURSOR    m_hCursor;

    void DrawRaster(WTL::CDCHandle dc, CRect const& rc, int nAlpha, CDIBitmap& diRaster) const;
    virtual void DoPaint(WTL::CDCHandle dc, CRect const& rc);
    virtual void DoNotifyMouseOver(CRect const& rc, CPoint pt);
    WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

private:
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point) const;
    void OnMouseMove(UINT nFlags, CPoint pt);
    BOOL OnEraseBkgnd(HDC) const;
    void OnNcPaint(HRGN) const;
    void OnSetFocus(HWND);
    BOOL OnSetCursor(HWND, UINT nHitTest, UINT message) const;
};

inline HBRUSH CDDCtrl::GetBackBrush() const
{
    return m_brBack.m_hBrush;
}
