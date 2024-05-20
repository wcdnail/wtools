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

    long GetPos() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETPOS, 0, 0);
    }

    long GetCurSel() const // DDX compat
    {
        return GetPos();
    }

    long GetRangeMin() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETRANGEMIN, 0, 0);
    }

    long GetRangeMax() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETRANGEMAX, 0, 0);
    }

    long GetTick(long nTick) const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETTIC, static_cast<WPARAM>(nTick), 0);
    }

    BOOL SetTick(long nTick)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (BOOL)::SendMessage(this->m_hWnd, TBM_SETTIC, 0, static_cast<LPARAM>(nTick));
    }

    void SetPos(long nPos, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETPOS, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nPos));
    }

    void SetCurSel(long nPos) // DDX compat
    {
        SetPos(nPos, TRUE);
    }

    void SetRange(long nMin, long nMax, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETRANGE, static_cast<WPARAM>(bRedraw), MAKELONG(nMin, nMax));
    }

    void SetRangeMin(long nMin, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETRANGEMIN, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMin));
    }

    void SetRangeMax(long nMax, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETRANGEMAX, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMax));
    }

    void ClearTics(BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_CLEARTICS, static_cast<WPARAM>(bRedraw), 0);
    }

    void SetSel(long nStart, long nStop, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETSEL, static_cast<WPARAM>(bRedraw), MAKELONG(nStart, nStop));
    }

    void SetSelStart(long nMin, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETSELSTART, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMin));
    }

    void SetSelEnd(long nMax, BOOL bRedraw)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETSELEND, static_cast<WPARAM>(bRedraw), static_cast<LPARAM>(nMax));
    }

    DWORD const* GetPTics() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return static_cast<DWORD const*>(::SendMessage(this->m_hWnd, TBM_GETPTICS, 0, 0));
    }

    long GetTicPos(long nTick) const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETTICPOS, static_cast<WPARAM>(nTick), 0);
    }

    long GetNumTics() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETNUMTICS, 0, 0);
    }

    long GetSelStart() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETSELSTART, 0, 0);
    }

    long GetSelEnd() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETSELEND, 0, 0);
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

    long SetPageSize(long nNewPageSize)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_SETPAGESIZE, 0, static_cast<LPARAM>(nNewPageSize));
    }

    long GetPageSize() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETPAGESIZE, 0, 0);
    }

    long SetLineSize(long nNewLineSize)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_SETLINESIZE, 0, static_cast<LPARAM>(nNewLineSize));
    }

    long GetLineSize() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (long)::SendMessage(this->m_hWnd, TBM_GETLINESIZE, 0, 0);
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

    //
    // uFlags
    // TRUE     The buddy will appear to the left of the trackbar if the trackbar control uses the TBS_HORZ style.
    //          If the trackbar uses the TBS_VERT style, the buddy appears above the trackbar control.
    // FALSE    The buddy will appear to the right of the trackbar if the trackbar control uses the TBS_HORZ style.
    //          If the trackbar uses the TBS_VERT style, the buddy appears below the trackbar control.
    //
    HWND SetBuddy(UINT uFlags, HWND hWndBuddy)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return reinterpret_cast<HWND>(::SendMessage(this->m_hWnd, TBM_SETBUDDY,
            static_cast<WPARAM>(uFlags), reinterpret_cast<LPARAM>(hWndBuddy)));
    }

    //
    // uFlags
    // TRUE     Retrieves the handle to the buddy to the left of the trackbar.
    //          If the trackbar control uses the TBS_VERT style, the message will retrieve the buddy above the trackbar.
    // FALSE    Retrieves the handle to the buddy to the right of the trackbar.
    //          If the trackbar control uses the TBS_VERT style, the message will retrieve the buddy below the trackbar.
    //
    HWND GetBuddy(UINT uFlags) const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return reinterpret_cast<HWND>(::SendMessage(this->m_hWnd, TBM_GETBUDDY, static_cast<WPARAM>(uFlags), 0));
    }

    //
    // Calling TBM_SETPOSNOTIFY will set the trackbar slider location like TBM_SETPOS would,
    // but it will also cause the trackbar to notify its parent of a move via a WM_HSCROLL or WM_VSCROLL message
    //
    void SetPosNotify(long nPos)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        ::SendMessage(this->m_hWnd, TBM_SETPOSNOTIFY, 0, static_cast<LPARAM>(nPos));
    }

    UINT SetUnicodeFormat(UINT uFlags)
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (UINT)::SendMessage(this->m_hWnd, TBM_SETUNICODEFORMAT, static_cast<WPARAM>(uFlags), 0);
    }

    UINT GetUnicodeFormat() const
    {
        ATLASSERT(::IsWindow(this->m_hWnd));
        return (UINT)::SendMessage(this->m_hWnd, TBM_GETUNICODEFORMAT, 0, 0);
    }

    // Invalidate/InvalidateRect did not work correctly...
    void Invalidate()
    {
        BOOL const bEnabled = this->IsWindowEnabled();
        this->EnableWindow(!bEnabled);
        this->EnableWindow(bEnabled);
    }
};

using CSliderCtrl = CSliderCtrlT<ATL::CWindow>;
}
