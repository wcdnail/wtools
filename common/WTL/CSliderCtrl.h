#pragma once

#include <atltypes.h>
#include <atlwin.h>

namespace WTL
{
template <class TBase>
struct CSliderCtrlT: TBase
{
    ~CSliderCtrlT() = default;

    CSliderCtrlT(HWND hWnd = nullptr)
        : TBase(hWnd)
    {
    }

    CSliderCtrlT<TBase>& operator = (HWND hWnd)
    {
        this->m_hWnd = hWnd;
        return *this;
    }

    HWND Create(HWND hWndParent, ATL::_U_RECT rect = nullptr, LPCTSTR szWindowName = nullptr,
            DWORD dwStyle = 0, DWORD dwExStyle = 0,
            ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = nullptr)
    {
        return TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
    }

    static LPCTSTR GetWndClassName()
    {
        return _T("msctls_trackbar32");
    }

    int GetPos() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETPOS, 0, 0);
    }

    int GetCurSel() const // DDX compat
    {
        return GetPos();
    }

    int GetRangeMin() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETRANGEMIN, 0, 0);
    }

    int GetRangeMax() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETRANGEMAX, 0, 0);
    }

    int GetTick(int nTick) const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETTIC, static_cast<WPARAM>(nTick), 0);
    }

    BOOL SetTick(int nTick)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (BOOL)::SendMessage(this->m_hWnd, TBM_SETTIC, 0, static_cast<LPARAM>(nTick));
    }

    void SetPos(int nPos, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETPOS, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nPos));
    }

    void SetCurSel(int nPos) // DDX compat
    {
        SetPos(nPos, TRUE);
    }

    void SetRange(int nMin, int nMax, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETRANGE, static_cast<WPARAM>(bRedraw), MAKELONG(nMin, nMax));
    }

    void SetRangeMin(int nMin, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETRANGEMIN, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMin));
    }

    void SetRangeMax(int nMax, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETRANGEMAX, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMax));
    }

    void ClearTics(BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_CLEARTICS, static_cast<WPARAM>(bRedraw), 0);
    }

    void SetSel(int nStart, int nStop, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETSEL, static_cast<WPARAM>(bRedraw), MAKELONG(nStart, nStop));
    }

    void SetSelStart(int nMin, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETSELSTART, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMin));
    }

    void SetSelEnd(int nMax, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETSELEND, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMax));
    }

    DWORD const* GetPTics() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return static_cast<DWORD const*>(::SendMessage(this->m_hWnd, TBM_GETPTICS, 0, 0));
    }

    int GetTicPos(int nTick) const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETTICPOS, static_cast<WPARAM>(nTick), 0);
    }

    int GetNumTics() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETNUMTICS, 0, 0);
    }

    int GetSelStart() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETSELSTART, 0, 0);
    }

    int GetSelEnd() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETSELEND, 0, 0);
    }

    void CearSel(BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_CLEARSEL, static_cast<WPARAM>(bRedraw), 0);
    }

    void SetTicFreq(int nFreq)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETTICFREQ, static_cast<WPARAM>(nFreq), 0);
    }

    int SetPageSize(int nNewPageSize)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_SETPAGESIZE, 0, static_cast<LPARAM>(nNewPageSize));
    }

    int GetPageSize() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETPAGESIZE, 0, 0);
    }

    int SetLineSize(int nNewLineSize)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_SETLINESIZE, 0, static_cast<LPARAM>(nNewLineSize));
    }

    int GetLineSize() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETLINESIZE, 0, 0);
    }

    CRect GetThumbRect() const
    {
        CRect rcTemp{};
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_GETTHUMBRECT, 0, reinterpret_cast<LPARAM>(&rcTemp));
        return rcTemp;
    }

    CRect GetChannelRect() const
    {
        CRect rcTemp{};
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_GETCHANNELRECT, 0, reinterpret_cast<LPARAM>(&rcTemp));
        return rcTemp;
    }

    void SetThumbLength(int nNewThumbLength)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETTHUMBLENGTH, static_cast<WPARAM>(nNewThumbLength), 0);
    }

    int GetThumbLength() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_GETTHUMBLENGTH, 0, 0);
    }

    HWND SetToolTips(HWND hTooltipCtl)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        // FIXME: maybe it is NOT HWND in return value!
        return static_cast<HWND>(::SendMessage(this->m_hWnd, TBM_SETTOOLTIPS, reinterpret_cast<WPARAM>(hTooltipCtl), 0));
    }

    HWND GetToolTips() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return static_cast<HWND>(::SendMessage(this->m_hWnd, TBM_GETTOOLTIPS, 0, 0));
    }

    // TBTS_TOP | TBTS_LEFT | TBTS_BOTTOM | TBTS_RIGHT
    int SetTipSide(int nSide)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (int)::SendMessage(this->m_hWnd, TBM_SETTIPSIDE, static_cast<WPARAM>(nSide), 0);
    }
};

using CSliderCtrl = CSliderCtrlT<ATL::CWindow>;
}
