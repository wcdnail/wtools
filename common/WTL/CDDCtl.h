#pragma once

#include "CCustomCtrl.h"
#include <wcdafx.api.h>
#include <DDraw.DGI/CeXDib.h>
#include <atlgdi.h>

struct CDDCtrl: CCustomControl
{
    DELETE_COPY_MOVE_OF(CDDCtrl);

    WCDAFX_API ~CDDCtrl() override;
    WCDAFX_API CDDCtrl();

    WCDAFX_API bool Initialize(long cx, long cy, long bpp, HBRUSH brBackground);
    WCDAFX_API void NotifySend(UINT code) const;
    WCDAFX_API void OnPaint(int nAlpha);

protected:
    CDIBitmap      m_Dib;
    WTL::CBrush m_brBack;
    bool    m_bBackOwner;

    virtual void DoPaint(WTL::CDCHandle dc, CRect const& rc);
    virtual void DoNotifyMouseOver(CRect const& rc, CPoint pt);
    WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

private:
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point) const;
    void OnMouseMove(UINT nFlags, CPoint pt);
};
