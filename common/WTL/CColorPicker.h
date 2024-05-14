#pragma once

#include <atlwin.h>
#include <memory>

struct CColorPicker: ATL::CWindowImpl<CColorPicker>
{
    using Super = ATL::CWindowImpl<CColorPicker>;

    DECLARE_WND_CLASS(_T("WCCF::CColorPicker"))

    ~CColorPicker() override;
    CColorPicker();

    HRESULT PreCreateWindow();

private:
    friend Super;
    struct Impl;

    static ATOM           gs_Atom;
    std::unique_ptr<Impl> m_pImpl;
    BOOL            m_bMsgHandled;

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    void OnNcPaint(WTL::CRgnHandle rgn);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnSize(UINT nType, CSize size);
};
