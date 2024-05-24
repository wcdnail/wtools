#pragma once

#include "CCustomCtrl.h"
#include <wcdafx.api.h>
#include <DDraw.DGI/CeXDib.h>
#include <atlgdi.h>
#include <atltheme.h>

struct CDDCtrl: public  CCustomControl,
                private WTL::CBufferedPaintImpl<CDDCtrl>
{
    DELETE_COPY_MOVE_OF(CDDCtrl);

    WCDAFX_API ~CDDCtrl() override;
    WCDAFX_API CDDCtrl();

    WCDAFX_API bool Initialize(long cx, long cy, long bpp, HBRUSH brBackground);
    WCDAFX_API void NotifySend(UINT code) const;

    HBRUSH GetBackBrush() const;

protected:
    friend WTL::CBufferedPaintImpl<CDDCtrl>;

    CDIBitmap      m_Dib;
    WTL::CBrush m_brBack;
    bool    m_bBackOwner;

    void DrawRaster(WTL::CDCHandle dc, CRect const& rc, int nAlpha, CDIBitmap& diRaster) const;
    virtual void DoPaint(WTL::CDCHandle dc, CRect const& rc);
    virtual void DoNotifyMouseOver(CRect const& rc, CPoint pt);
    WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

private:
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point) const;
    void OnMouseMove(UINT nFlags, CPoint pt);
};

inline HBRUSH CDDCtrl::GetBackBrush() const
{
    return m_brBack.m_hBrush;
}
