#pragma once

#include "luicDrawings.h"
#include <wcdafx.api.h>
#include <atltheme.h>

using SelectedPair = std::pair<int, int>;

struct CThemePreviewer: WTL::CBufferedPaintWindowImpl<CThemePreviewer>
{
    using      Super = WTL::CBufferedPaintWindowImpl<CThemePreviewer>;
    using SuperPaint = WTL::CBufferedPaintImpl<CThemePreviewer>;

    DELETE_COPY_MOVE_OF(CThemePreviewer);
    DECLARE_WND_CLASS_EX(_T("CThemePreviewer"), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_APPWORKSPACE)

    static ATOM gs_Atom;

    ~CThemePreviewer() override;
    CThemePreviewer();

    HRESULT PreCreateWindow();
    void OnSchemeChanged(CScheme& pScheme, CSizePair& pSizePair, WTL::CComboBox& pcbItem);
    void OnItemSelected(int nItem);

private:
    friend Super;
    friend SuperPaint;

    enum WND_Index : int
    {
        WND_Invalid = -1,
        WND_MsgBox,
        WND_Front,
        WND_Back,
        WND_Count
    };

    CScheme*               m_pScheme;
    CSizePair*           m_pSizePair;
    WTL::CComboBox*        m_pcbItem;
    SelectedPair        m_prSelected;
    bool                 m_bLBtnDown;
    WindowRects m_WndRect[WND_Count];

    static void RectsBuild(CRect const& rcClient, CRect& rcFront, CRect& rcBack, CRect& rcMsg, CRect& rcIcon);
    void DrawDesktop(WTL::CDCHandle dc, CRect const& rc);
    CRect GetSeletcedRect();
    void SetSelectedRect(int wr, int ri);
    int OnCreate(LPCREATESTRUCT pCS);
    void DoPaint(WTL::CDCHandle dc, RECT& rc);
    void OnLButton(UINT nFlags, CPoint point);
    int RectIndexToElementId() const;
    void NotifyParent() const;

    BEGIN_MSG_MAP_EX(CThemePreviewer)
        CHAIN_MSG_MAP(Super)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_LBUTTONDOWN(OnLButton)
        MSG_WM_LBUTTONDBLCLK(OnLButton)
    END_MSG_MAP()
};
