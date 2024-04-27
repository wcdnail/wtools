#pragma once

#if 0
#include "win-5-6-7.features.h"

#define _WTL_NO_THEME_DELAYLOAD
#include <atltheme.h>

namespace Cf
{
    static inline bool IsSupported() 
    {
        return RunTimeHelper::IsVista() || RunTimeHelper::IsWin7();
    }

    static inline bool IsComposing() 
    {
        return IsSupported() ? Win567().IsCompositionEnabled() : false;
    }    

    template <class T>
    static inline BOOL Subclass(T& control, HWND handle)
    {
        ATLASSERT(::IsWindow(handle));
        return IsSupported() ? control.SubclassWindow(handle) : FALSE;
    }

    template <class T>
    class BasicAero: public WTL::CThemeImpl<T>
    {
    public:
        virtual ~BasicAero()
        {
        }

        BasicAero(PCWSTR name = L"globals") 
            : WTL::CThemeImpl<T>()
        {
            SetThemeClassList(name);
        }

        bool IsTheming() const
        {
            return m_hTheme != 0;
        } 

        template <class U>
        BOOL Subclass(U& control, UINT id)
        {
            return Cf::Subclass<U>(control, static_cast<T*>(this)->GetDlgItem(id));
        }

#if 0
        bool DrawPartText(HDC dc, int iPartID, int iStateID, LPCTSTR pStr, LPRECT prText, UINT uFormat, DTTOPTS &dto)
        {
            HRESULT hr = S_FALSE;

            if(IsTheming())
            {
                if (IsSupported())
                    hr = DrawThemeTextEx(dc, iPartID, iStateID, pStr, -1, uFormat, prText, &dto);
                else
                    hr = DrawThemeText(dc, iPartID, iStateID, pStr, -1, uFormat, 0, prText);
            }
            else
                hr = CDCHandle(dc).DrawText(pStr, -1, prText, uFormat) != 0 ? S_OK : S_FALSE;

            return SUCCEEDED(hr);
        }

        bool DrawPartText(HDC dc, int iPartID, int iStateID, LPCTSTR pStr, LPRECT prText, UINT uFormat, DWORD dwFlags = DTT_COMPOSITED, int iGlowSize = 0)
        {
            DTTOPTS dto = {sizeof(DTTOPTS)};
            dto.dwFlags = dwFlags;
            dto.iGlowSize = iGlowSize;
            return DrawPartText(dc, iPartID, iStateID, pStr, prText, uFormat, dto);
        }

        bool DrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DTTOPTS &dto)
        {
            return DrawPartText(dc, 1, 1, pStr, prText, uFormat, dto);
        }

        bool DrawText(HDC dc, LPCTSTR pStr, LPRECT prText, UINT uFormat, DWORD dwFlags = DTT_COMPOSITED, int iGlowSize = 0)
        {
            return DrawPartText(dc, 1, 1, pStr, prText, uFormat, dwFlags, iGlowSize);
        }
#endif
    };

    template <class T>
    class AeroImpl: public WTL::CBufferedPaintImpl<T>
                  , public BasicAero<T>
    {
    public:

#if 0
        MARGINS m_Margins;

        virtual ~AeroImpl()
        {
        }

        AeroImpl(PCWSTR name = L"globals") 
            : BasicAero<T>(name)
        {
            m_PaintParams.dwFlags = BPPF_ERASE;
            MARGINS m = {-1};
            m_Margins = m;
        }

        bool SetMargins(MARGINS& m)
        {
            m_Margins = m;
            T* pT = static_cast<T*>(this);
            return pT->IsWindow() && IsComposing() ? SUCCEEDED(Win567().ExtendFrameIntoClientArea(pT->m_hWnd, &m_Margins)) : true;
        }

        bool SetOpaque(bool bOpaque = true)
        {
            MARGINS m = {bOpaque - 1};
            return SetMargins(m);
        }

        bool SetOpaque(RECT &rOpaque)
        {
            T* pT = static_cast<T*>(this);
            RECT rClient;
            pT->GetClientRect(&rClient);
            MARGINS m = {rOpaque.left, rClient.right - rOpaque.right, rOpaque.top, rClient.bottom - rOpaque.bottom};
            return SetMargins(m);
        }

        bool SetOpaqueUnder(ATL::CWindow wChild)
        {
            T* pT = static_cast<T*>(this);
            ATLASSERT(wChild.IsWindow());
            ATLASSERT(pT->IsChild(wChild));

            RECT rChild;
            wChild.GetWindowRect(&rChild);
            pT->ScreenToClient(&rChild);

            return SetOpaque(rChild);
        }

        bool SetOpaqueUnder(UINT uID)
        {
            return SetOpaqueUnder(static_cast<T*>(this)->GetDlgItem(uID));
        }

        // implementation
        void DoPaint(CDCHandle dc, RECT& rDest)
        {
            T* pT = static_cast<T*>(this);

            RECT rClient;
            pT->GetClientRect(&rClient);

            RECT rView = {rClient.left + m_Margins.cxLeftWidth, rClient.top + m_Margins.cyTopHeight, 
                rClient.right - m_Margins.cxRightWidth, rClient.bottom - m_Margins.cyBottomHeight};

            if (!IsComposing())
                if (IsTheming())
                    pT->DrawThemeBackground(dc, WP_FRAMEBOTTOM, pT->m_hWnd == GetFocus() ? FS_ACTIVE : FS_INACTIVE, &rClient, &rDest);
                else
                    dc.FillSolidRect(&rClient, ::GetSysColor(COLOR_MENUBAR));

            if ((m_Margins.cxLeftWidth != -1) && !::IsRectEmpty(&rView))
            {
                dc.FillSolidRect(&rView, ::GetSysColor(COLOR_WINDOW));
                if (!m_BufferedPaint.IsNull())
                    m_BufferedPaint.MakeOpaque(&rView);
            }
            else 
                ::SetRectEmpty(&rView);

            pT->Paint(dc, rClient, rView, rDest);
        }

        void Paint(CDCHandle /*dc*/, RECT& /*rClient*/, RECT& /*rView*/, RECT& /*rDest*/)
        {}

        void OnComposition()
        {}

        void OnColorization()
        {}

        BEGIN_MSG_MAP(AeroWindow)
            CHAIN_MSG_MAP(CThemeImpl<T>)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
            MESSAGE_HANDLER(0x031e /*WM_DWMCOMPOSITIONCHANGED*/, OnCompositionChanged)
            MESSAGE_HANDLER(0x0320 /*WM_DWMCOLORIZATIONCOLORCHANGED*/, OnColorizationChanged)
            CHAIN_MSG_MAP(CBufferedPaintImpl<T>)
        END_MSG_MAP()

        LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            if (IsThemingSupported())
                OpenThemeData();

            if (IsComposing())
                Win567().ExtendFrameIntoClientArea(static_cast<T*>(this)->m_hWnd, &m_Margins);

            return bHandled = FALSE;
        }

        LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            if (!IsComposing() && IsTheming())
                static_cast<T*>(this)->Invalidate(FALSE);

            return bHandled = FALSE;
        }

        LRESULT OnCompositionChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            if (IsComposing())
                SetMargins(m_Margins);

            static_cast<T*>(this)->OnComposition();
            return bHandled = FALSE;
        }

        LRESULT OnColorizationChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            static_cast<T*>(this)->OnColorization();
            return bHandled = FALSE;
        }
#endif
    };
}
#endif 
