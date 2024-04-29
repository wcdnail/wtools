#pragma once

#include <atltheme.h>
#include <dwmapi.h>

///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CAeroImpl<T> - enables Aero translucency (when available) for any window
// CAeroDialogImpl<T, TBase> - dialog implementation of Aero translucency (when available)
// CAeroFrameImpl<T, TBase, TWinTraits> - frame implementation of Aero translucency (when available) 
// CAeroCtrlImpl - base implementation of Aero opacity for controls
// CAeroStatic - Aero opaque Static control
// CAeroButton - Aero opaque Button control
// CAeroEdit - Aero opaque Edit control

namespace WTL
{

template <class T>
class CAeroImpl: public CBufferedPaintImpl<T>,
                 public CThemeImpl<T>
{
public:
    using Super = CBufferedPaintImpl<T>;
    using Theme = CThemeImpl<T>;

    CAeroImpl(LPCWSTR lpstrThemeClassList = L"globals")
    {
        this->m_PaintParams.dwFlags = BPPF_ERASE;
        this->SetThemeClassList(lpstrThemeClassList);
        MARGINS m = {0};
        m_Margins = m;
    }

    static bool IsAeroSupported() 
    {
        return Super::IsBufferedPaintSupported();
    }

    bool IsCompositionEnabled() const
    {
        BOOL bEnabled = FALSE;
        return IsAeroSupported() ? SUCCEEDED(DwmIsCompositionEnabled(&bEnabled)) && bEnabled : false;
    }    
    
    bool IsTheming() const
    {
        return nullptr != this->m_hTheme;
    }    
    
    MARGINS m_Margins;

    bool SetMargins(MARGINS& m)
    {
        m_Margins = m;
        T* pT = static_cast<T*>(this);
        return pT->IsWindow() && IsAeroSupported() ? SUCCEEDED(DwmExtendFrameIntoClientArea(pT->m_hWnd, &m_Margins)) : true;
    }

    void DoPaint(CDCHandle dc, RECT& rDest)
    {
        T* pT = static_cast<T*>(this);

        RECT rClient;
        pT->GetClientRect(&rClient);

        RECT rView = {rClient.left  + m_Margins.cxLeftWidth,  rClient.top    + m_Margins.cyTopHeight, 
                      rClient.right - m_Margins.cxRightWidth, rClient.bottom - m_Margins.cyBottomHeight};

        if (IsCompositionEnabled()) {
            dc.FillSolidRect(&rClient, RGB(0, 0, 0));
        }
        else {
            if (IsTheming()) {
                pT->DrawThemeBackground(dc, WP_FRAMEBOTTOM, pT->m_hWnd == GetFocus() ? 1 : 2, &rClient, &rDest);
            }
            else {
                dc.FillSolidRect(&rClient, GetSysColor(COLOR_MENUBAR));
            }
        }
        if (m_Margins.cxLeftWidth != -1) {
            dc.FillSolidRect(&rView, GetSysColor(COLOR_WINDOW));
        }
        else {
            ::SetRectEmpty(&rView);
        }
        pT->AeroDoPaint(dc, rClient, rView, rDest);
    }

    void AeroDrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DTTOPTS &dto)
    {
        if(IsTheming())
            if (IsAeroSupported()) {
                this->DrawThemeTextEx(dc, TEXT_BODYTITLE, 0, pStr, -1, uFormat, prText, &dto );
            }
            else {
                this->DrawThemeText(dc, TEXT_BODYTITLE, 0, pStr, -1, uFormat, 0, prText);
            }
        else {
            CDCHandle(dc).DrawText(pStr, -1, prText, uFormat);
        }
    }

    void AeroDrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DWORD dwFlags, int iGlowSize)
    {
        DTTOPTS dto = { sizeof(DTTOPTS) };
        dto.dwFlags = dwFlags;
        dto.iGlowSize = iGlowSize;
        AeroDrawText(dc, pStr, prText, uFormat, dto);
    }

    void AeroDoPaint(CDCHandle dc, RECT& rClient, RECT& rView, RECT& rDest)
    {
    }

    BEGIN_MSG_MAP(CAeroImpl)
        CHAIN_MSG_MAP(Theme)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, OnCompositionChanged)
        MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
        CHAIN_MSG_MAP(Super)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if (Theme::IsThemingSupported()) {
            this->OpenThemeData();
        }
        if (IsCompositionEnabled()) {
            DwmExtendFrameIntoClientArea(static_cast<T*>(this)->m_hWnd, &m_Margins);
        }
        return bHandled = FALSE;
    }

    LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if (!IsCompositionEnabled() && IsTheming()) {
            static_cast<T*>(this)->Invalidate(FALSE);
        }
        return bHandled = FALSE;
    }

    LRESULT OnCompositionChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (IsCompositionEnabled()) {
            SetMargins(m_Margins);
        }
        return 0;
    }

    LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        T* pT = static_cast<T*>(this);
        return ::DefWindowProc(pT->m_hWnd, uMsg, wParam, lParam);
    }
};


///////////////////////////////////////////////////////////////////////////////
// CAeroDialogImpl - dialog implementation of Aero translucency (when available)

template <class T, class TBase  = CWindow>
class ATL_NO_VTABLE CAeroDialogImpl : public CDialogImpl<T, TBase>,
                                      public CAeroImpl<T>
{
public:
    CAeroDialogImpl(LPCWSTR lpstrThemeClassList = L"dialog")
        : CAeroImpl<T>(lpstrThemeClassList)
    {
    }

    void AeroDoPaint(CDCHandle dc, RECT& rClient, RECT& rView, RECT& rDest)
    {
        if (!::IsRectEmpty(&rView)) {
            if (this->IsTheming()) {
                this->DrawThemeBackground(dc, WP_DIALOG, 1, &rView, &rDest);
            }
            else {
                dc.FillSolidRect(&rView, GetSysColor(COLOR_BTNFACE));
            }
        }
    }

    BEGIN_MSG_MAP(CAeroDialogImpl)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        CHAIN_MSG_MAP(CAeroImpl<T>)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if (this->IsThemingSupported()) {
            this->OpenThemeData();
            this->EnableThemeDialogTexture(ETDT_ENABLE);
        }
        if (this->IsCompositionEnabled()) {
            DwmExtendFrameIntoClientArea(static_cast<T*>(this)->m_hWnd, &this->m_Margins);
        }
        return bHandled = FALSE;
    }

};

///////////////////////////////////////////////////////////////////////////////
// CAeroFrameImpl - frame implementation of Aero translucency (when available)

template <class T, class TBase = ATL::CWindow, class TWinTraits = ATL::CFrameWinTraits>
class ATL_NO_VTABLE CAeroFrameImpl : public CFrameWindowImpl<T, TBase, TWinTraits>,
                                     public CAeroImpl<T>
{
    typedef CFrameWindowImpl<T, TBase, TWinTraits> _baseClass;

public:
    CAeroFrameImpl(LPCWSTR lpstrThemeClassList = L"window")
        : CAeroImpl<T>(lpstrThemeClassList)
    {
    }

    void UpdateLayout(BOOL bResizeBars = TRUE)
    {
        RECT rect = { 0 };
        this->GetClientRect(&rect);
        // position margins
        if (this->m_Margins.cxLeftWidth != -1) {
            rect.left += this->m_Margins.cxLeftWidth;
            rect.top += this->m_Margins.cyTopHeight;
            rect.right -= this->m_Margins.cxRightWidth;
            rect.bottom -= this->m_Margins.cyBottomHeight;
        }
        // position bars and offset their dimensions
        UpdateBarsPosition(rect, bResizeBars);
        // resize client window
        if (this->m_hWndClient != NULL) {
            ::SetWindowPos(this->m_hWndClient, NULL, rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        this->Invalidate(FALSE);
    }

    void UpdateBarsPosition(RECT& rect, BOOL bResizeBars = TRUE)
    {
        // resize toolbar
        if(this->m_hWndToolBar != NULL && ((DWORD)::GetWindowLong(this->m_hWndToolBar, GWL_STYLE) & WS_VISIBLE)) {
            RECT rectTB = { 0 };
            ::GetWindowRect(this->m_hWndToolBar, &rectTB);
            if(bResizeBars) {
                ::SetWindowPos(this->m_hWndToolBar, NULL, rect.left, rect.top,
                    rect.right - rect.left, rectTB.bottom - rectTB.top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
                ::InvalidateRect(this->m_hWndToolBar, NULL, FALSE);
            }
            rect.top += rectTB.bottom - rectTB.top;
        }
        // resize status bar
        if(this->m_hWndStatusBar != NULL && ((DWORD)::GetWindowLong(this->m_hWndStatusBar, GWL_STYLE) & WS_VISIBLE))
        {
            RECT rectSB = { 0 };
            ::GetWindowRect(this->m_hWndStatusBar, &rectSB);
            rect.bottom -= rectSB.bottom - rectSB.top;
            if(bResizeBars) {
                ::SetWindowPos(this->m_hWndStatusBar, NULL, rect.left, rect.bottom,
                    rect.right - rect.left, rectSB.bottom - rectSB.top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }

    BOOL CreateSimpleStatusBar(LPCTSTR lpstrText, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , UINT nID = ATL_IDW_STATUS_BAR)
    {
        ATLASSERT(!::IsWindow(this->m_hWndStatusBar));
        this->m_hWndStatusBar = ::CreateStatusWindow(dwStyle | CCS_NOPARENTALIGN , lpstrText, this->m_hWnd, nID);
        return (this->m_hWndStatusBar != NULL);
    }

    BOOL CreateSimpleStatusBar(UINT nTextID = ATL_IDS_IDLEMESSAGE, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , UINT nID = ATL_IDW_STATUS_BAR)
    {
        const int cchMax = 128; // max text length is 127 for status bars (+1 for null)
        TCHAR szText[cchMax];
        szText[0] = 0;
        ::LoadString(ModuleHelper::GetResourceInstance(), nTextID, szText, cchMax);
        return CreateSimpleStatusBar(szText, dwStyle, nID);
    }

    static HWND CreateSimpleReBarCtrl(HWND hWndParent, DWORD dwStyle = ATL_SIMPLE_REBAR_STYLE, UINT nID = ATL_IDW_TOOLBAR)
    {
        return _baseClass::CreateSimpleReBarCtrl(hWndParent, dwStyle | CCS_NOPARENTALIGN, nID);
    }

    BOOL CreateSimpleReBar(DWORD dwStyle = ATL_SIMPLE_REBAR_STYLE, UINT nID = ATL_IDW_TOOLBAR)
    {
        ATLASSERT(!::IsWindow(this->m_hWndToolBar));
        this->m_hWndToolBar = _baseClass::CreateSimpleReBarCtrl(this->m_hWnd, dwStyle | CCS_NOPARENTALIGN, nID);
        return (this->m_hWndToolBar != NULL);
    }

    BEGIN_MSG_MAP(CAeroFrameImpl)
        CHAIN_MSG_MAP(CAeroImpl<T>)
        CHAIN_MSG_MAP(_baseClass)
    END_MSG_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// CAeroCtrlImpl - implementation of Aero opacity for controls

template <class TBase>
class CAeroCtrlImpl : public CBufferedPaintWindowImpl<CAeroCtrlImpl<TBase>, TBase>
{
public:
    using Super = CBufferedPaintWindowImpl<CAeroCtrlImpl<TBase>, TBase>;

    static ATL::CWndClassInfoW& GetWndClassInfo()
    {
        static ATL::CWndClassInfoW wc = {
            {
                sizeof(WNDCLASSEX), 0, Super::StartWindowProc, 0, 0,
                nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            },
            TBase::GetWndClassName(), 0, 0, 1, 0, L""
        };
        return wc;
    }

    void DoBufferedPaint(CDCHandle dc, RECT& rect)
    {
        HDC hDCPaint = NULL;
        if(this->IsBufferedPaintSupported()) {
            this->m_BufferedPaint.Begin(dc, &rect, this->m_dwFormat, &this->m_PaintParams, &hDCPaint);
        }
        if(hDCPaint != NULL) {
            DoPaint(hDCPaint, rect);
        }
        else {
            DoPaint(dc.m_hDC, rect);
        }
        if(this->IsBufferedPaintSupported() && !this->m_BufferedPaint.IsNull()) {
            this->m_BufferedPaint.MakeOpaque(&rect);
            this->m_BufferedPaint.End();
        }
    }

    void DoPaint(CDCHandle dc, RECT& /*rect*/)
    {
        this->DefWindowProc(WM_PRINTCLIENT, reinterpret_cast<WPARAM>(dc.m_hDC), PRF_CLIENT);
    }
};

///////////////////////////////////////////////////////////////////////////////
// CAeroStatic - Aero opaque Static control
// CAeroButton - Aero opaque Button control

typedef CAeroCtrlImpl<CStatic> CAeroStatic;
typedef CAeroCtrlImpl<CButton> CAeroButton;

///////////////////////////////////////////////////////////////////////////////
// CAeroEdit - Aero opaque Edit control
class CAeroEdit : public CAeroCtrlImpl<CEdit>
{
    BEGIN_MSG_MAP(CAeroEdit)
        REFLECTED_COMMAND_CODE_HANDLER(EN_CHANGE, OnChange)
        CHAIN_MSG_MAP(CAeroCtrlImpl<CEdit>)
    END_MSG_MAP()

    LRESULT OnChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        Invalidate(FALSE); 
        return bHandled = FALSE;
    } 
};

}; // namespace WTL
