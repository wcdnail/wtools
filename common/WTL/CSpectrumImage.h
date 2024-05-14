#pragma once

#include <atlwin.h>

struct CSpectrumImage: ATL::CWindowImpl<CSpectrumImage>
{
    using Super = ATL::CWindowImpl<CSpectrumImage>;

    DECLARE_WND_CLASS(_T("WCCF::CSpectrumImage"))

    ~CSpectrumImage() override;
    CSpectrumImage();

    HRESULT PreCreateWindow();

private:
    friend Super;

    static ATOM           gs_Atom;
    BOOL            m_bMsgHandled;

    BOOL IsMsgHandled() const { return m_bMsgHandled; }
    void SetMsgHandled(BOOL bHandled) { m_bMsgHandled = bHandled; }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
};
