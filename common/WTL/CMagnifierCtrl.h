#pragma once

#include "CCustomCtrl.h"
#include <wcdafx.api.h>

struct CMagnifierInit
{
    static CMagnifierInit& Instance();

private:
    CMagnifierInit();
    ~CMagnifierInit();
};

struct CMagnifierCtrl: private CCustomControl
{
    DECLARE_WND_CLASS(_T("WCCF::CMagnifierCtrl"))
    DELETE_MOVE_OF(CMagnifierCtrl);

    WCDAFX_API ~CMagnifierCtrl() override;
    WCDAFX_API CMagnifierCtrl();
    
    WCDAFX_API HRESULT PreCreateWindow() override;

    WCDAFX_API bool Initialize();

    BOOL SetSize(LONG cx, LONG cy);
    WCDAFX_API BOOL SetMagnificationFactor(float fXFactor, float fYFactor) const;
    WCDAFX_API void MoveToMousePos();

    using WndSuper::ShowWindow;
    using WndSuper::DestroyWindow;

protected:
    CRect         m_rcMag;
    ATL::CWindow m_ctlMag;

    WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    int OnCreate(LPCREATESTRUCT pCS);
    void OnDestroy();
};

inline BOOL CMagnifierCtrl::SetSize(LONG cx, LONG cy)
{
    m_rcMag.right = m_rcMag.left + cx;
    m_rcMag.bottom = m_rcMag.top + cy;
    MoveToMousePos();
}
