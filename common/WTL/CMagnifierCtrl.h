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

enum EMagnifierPos: int
{
    MAGPOS_FIXED = 0,
    MAGPOS_FOLLOW_MICE,
};

struct CMagnifierCtrl: private CCustomControl
{
    DECLARE_WND_CLASS(_T("WCCF::CMagnifierCtrl"))
    DELETE_MOVE_OF(CMagnifierCtrl);

    WCDAFX_API ~CMagnifierCtrl() override;
    WCDAFX_API CMagnifierCtrl();
    
    WCDAFX_API HRESULT PreCreateWindow() override;

    WCDAFX_API bool Initialize(HWND hWnd, float fXFactor, float fYFactor);

    void SetSourcePos(CPoint pt);
    void SetRect(CRect const& rc);
    void SetSize(LONG cx, LONG cy);
    WCDAFX_API BOOL SetMagnification(float fXFactor, float fYFactor) const;
    WCDAFX_API void UpdatePosition();

    using WndSuper::m_hWnd;
    using WndSuper::ShowWindow;
    using WndSuper::DestroyWindow;

protected:
    CRect         m_rcMag;
    CPoint        m_ptPos;
    EMagnifierPos  m_ePos;
    ATL::CWindow m_ctlMag;

    WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    int OnCreate(LPCREATESTRUCT pCS);
    void OnDestroy();
};

inline void CMagnifierCtrl::SetSourcePos(CPoint pt)
{
    if (MAGPOS_FIXED == m_ePos) {
        m_ptPos = pt;
    }
    UpdatePosition();
}

inline void CMagnifierCtrl::SetRect(CRect const& rc)
{
    m_rcMag = rc;
    UpdatePosition();
}

inline void CMagnifierCtrl::SetSize(LONG cx, LONG cy)
{
    m_rcMag.right = m_rcMag.left + cx;
    m_rcMag.bottom = m_rcMag.top + cy;
    UpdatePosition();
}
