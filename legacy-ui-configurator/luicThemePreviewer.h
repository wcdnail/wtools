#pragma once

#include "luicDrawings.h"
//#include "windows.wtl.buffered.paint.h"

using SelectedPair = std::pair<int, int>;

struct CThemePreviewer: ATL::CWindow //, CF::DoubleBuffered
{
    static ATOM Register(HRESULT& code);

    ~CThemePreviewer();
    CThemePreviewer();

    void SubclassIt(HWND hWnd);
    void OnSelectTheme(CTheme* pTheme, WTL::CComboBox* pcbItem);
    void OnSelectItem(int nItem);

private:
    static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    enum WND_Index : int
    {
        WND_Invalid = -1,
        WND_MsgBox,
        WND_Front,
        WND_Back,
        WND_Count
    };

    CTheme*                 m_pTheme;
    WTL::CComboBox*        m_pcbItem;
    SelectedPair        m_prSelected;
    bool                 m_bLBtnDown;
    WindowRects m_WndRect[WND_Count];

    static void CalcRects(CRect const& rcClient, CRect& rcFront, CRect& rcBack, CRect& rcMsg, CRect& rcIcon);
    void DrawDesktop(WTL::CDCHandle dc, CRect const& rc);
    CRect GetSeletcedRect();
    void SetSelectedRect(int wr, int ri);
    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(WTL::CDCHandle dc);
    void OnLButton(UINT nFlags, CPoint point);
    int RectIndexToElementId() const;
    void NotifyParent() const;
};
