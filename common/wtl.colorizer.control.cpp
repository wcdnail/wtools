#include "stdafx.h"
#include "wtl.colorizer.control.h"

namespace Cf
{
#pragma region Static specific

    template <>
    WCDAFX_API void Colorizer::Control<WTL::CStatic>::OnPaint(CDCHandle)
    {
        if (Details<WTL::CStatic>::Normal != Details<WTL::CStatic>::GetAppearType(*this)) {
            SetMsgHandled(FALSE);
        }
        else {
            PaintContext pc(*this, GetFont());
            CString text;
            if (GetWindowText(text)) {
                Owner->SetTextColor(pc.PaindDC.m_hDC);
                pc.PaindDC.DrawText(text, text.GetLength(), pc.Rect, Details<WTL::CStatic>::GetDrawTextFormat(*this));
            }
        }
    }

#pragma endregion

#pragma region Button specific

    template <>
    WCDAFX_API void Colorizer::Control<WTL::CButton>::OnPaint(CDCHandle)
    {
        Details<WTL::CButton>::AppearType type = Details<WTL::CButton>::GetAppearType(*this);
        if ((Details<WTL::CButton>::Ownerdraw == type) || (Details<WTL::CButton>::UserButton == type)) {
            SetMsgHandled(FALSE);
        }
        else {
            PaintContext pc(*this, GetFont());

            CString text;
            GetWindowText(text);
            Owner->SetTextColor(pc.PaindDC.m_hDC);

            switch (type) {
            case Details<WTL::CButton>::Groupbox: {
                Owner->DrawGroupBox(*this, pc.PaindDC.m_hDC, pc.Rect, text);
                break;
            }
            case Details<WTL::CButton>::PushButton:
            case Details<WTL::CButton>::DefPushButton:
            case Details<WTL::CButton>::Flat: {
                Owner->DrawButton(*this, pc.PaindDC.m_hDC, pc.Rect, text,
                    Details<WTL::CButton>::Flat == type,
                    Details<WTL::CButton>::DefPushButton == type);
                break;
            }
            case Details<WTL::CButton>::Checkbox:
            case Details<WTL::CButton>::AutoCheckbox:
            case Details<WTL::CButton>::ThreeState:
            case Details<WTL::CButton>::Auto3State: {
                Owner->DrawCheckBox(*this, pc.PaindDC.m_hDC, pc.Rect, text);
                break;
            }
            case Details<WTL::CButton>::RadioButton:
            case Details<WTL::CButton>::AutoRadioButton: {
                Owner->DrawRadioButton(*this, pc.PaindDC.m_hDC, pc.Rect, text);
                break;
            }
            case Details<WTL::CButton>::Pushbox:
            case Details<WTL::CButton>::UserButton:
            case Details<WTL::CButton>::Ownerdraw: {
                ::DebugBreak();
                break;
            }
            }
        }
    }

#pragma endregion

#pragma region ListBox specific

    template <>
    WCDAFX_API void Colorizer::Control<WTL::CListBox>::SpecificPreInit(HWND hwnd)
    {
        UINT ns = static_cast<UINT>(::GetWindowLong(hwnd, GWL_STYLE));
        ns |= LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT;
        ::SetWindowLong(hwnd, GWL_STYLE, static_cast<LONG>(ns));
    }

    template <>
    WCDAFX_API CString Colorizer::Control<WTL::CListBox>::GetItemText(int index) const
    {
        CString result;
        GetText(index, result);
        return result;
    }

    template <>
    WCDAFX_API BOOL Colorizer::Control<WTL::CListBox>::SpecificMessagesHandler(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD)
    {
        if (WM_DRAWITEM == uMsg) {
            this->SetMsgHandled(TRUE);
            OnDrawItem(static_cast<int>(wParam), reinterpret_cast<LPDRAWITEMSTRUCT>(lParam));
            lResult = TRUE;
            if(this->IsMsgHandled()) {
                return TRUE;
            }
        }
        return FALSE;
    }

#pragma endregion

#pragma region ComboBox specific

    template <>
    WCDAFX_API CString Colorizer::Control<WTL::CComboBox>::GetItemText(int index) const
    {
        CString result;
        GetLBText(index, result);
        return result;
    }

    template <> WCDAFX_API void Colorizer::Control<WTL::CComboBox>::OnNcPaint(CRgnHandle rgn) {}
    template <> WCDAFX_API BOOL Colorizer::Control<WTL::CComboBox>::OnEraseBkgnd(CDCHandle dc) { return TRUE; }

    template <>
    WCDAFX_API void Colorizer::Control<WTL::CComboBox>::OnPaint(CDCHandle)
    {
        PaintContext pc(*this, GetFont());
        Owner->DrawComboFace<WTL::CComboBox>(*this, pc.PaindDC.m_hDC, pc.Rect);
    }

    template <>
    WCDAFX_API HBRUSH Colorizer::Control<WTL::CComboBox>::OnCtlColorListBox(CDCHandle dc, HWND hwnd)
    {
        if (!Members.Listbox.get() && (m_hWnd != hwnd))
        {
            Owner->OnEraseBackground(*this, dc);
            Members.Listbox.reset(new Control<WTL::CListBox>(hwnd, Owner));
        }

        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    template <>
    WCDAFX_API HBRUSH Colorizer::Control<WTL::CComboBox>::OnCtlColorEdit(CDCHandle dc, HWND)
    {
        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    template <>
    WCDAFX_API BOOL Colorizer::Control<WTL::CComboBox>::SpecificMessagesHandler(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD)
    {
        if (WM_DRAWITEM == uMsg) {
            this->SetMsgHandled(TRUE);
            OnDrawItem(static_cast<int>(wParam), reinterpret_cast<LPDRAWITEMSTRUCT>(lParam));
            lResult = TRUE;
            if(this->IsMsgHandled()) {
                return TRUE;
            }
        }
        else if (uMsg == WM_CTLCOLORLISTBOX) {
            this->SetMsgHandled(TRUE);
            lResult = reinterpret_cast<LRESULT>(OnCtlColorListBox(reinterpret_cast<HDC>(wParam), reinterpret_cast<HWND>(lParam)));
            if(this->IsMsgHandled()) {
                return TRUE;
            }
        }
        else if (uMsg == WM_CTLCOLOREDIT) {
            this->SetMsgHandled(TRUE);
            lResult = reinterpret_cast<LRESULT>(OnCtlColorEdit(reinterpret_cast<HDC>(wParam), reinterpret_cast<HWND>(lParam)));
            if(this->IsMsgHandled()) {
                return TRUE;
            }
        }
        return FALSE;
    }

#pragma endregion

#pragma region ScrollBar specific

    template <>
    WCDAFX_API BOOL Colorizer::Control<WTL::CScrollBar>::OnEraseBkgnd(CDCHandle)
    {
        SetMsgHandled(FALSE);
        return FALSE;
    }

#pragma endregion

#pragma region TreeView specific

    template <>
    WCDAFX_API void Colorizer::Control<WTL::CTreeViewCtrl>::SpecificPreInit(HWND hwnd)
    {
        ::SetWindowLong(hwnd, GWL_EXSTYLE
            , ::GetWindowLong(hwnd, GWL_EXSTYLE) 
            | 0x0004 /*TVS_EX_DOUBLEBUFFER*/
            );
    }

    template <>
    WCDAFX_API LRESULT Colorizer::Control<WTL::CTreeViewCtrl>::OnGetDispinfo(LPNMHDR /*header*/)
    {
        //LPNMTVDISPINFO di = (LPNMTVDISPINFO)header;
        // ##TODO: implemenation of CTreeViewCtrl OnGetDispinfo
        return 0;
    }

    template <>
    WCDAFX_API DWORD Colorizer::Control<WTL::CTreeViewCtrl>::OnPrePaint(int, LPNMCUSTOMDRAW cd) 
    {
        CDCHandle dc(cd->hdc);
        dc.FillSolidRect(&cd->rc, Owner->MyBackColor);
        return CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
    }

    template <>
    WCDAFX_API DWORD Colorizer::Control<WTL::CTreeViewCtrl>::OnItemPrePaint(int, LPNMCUSTOMDRAW cd) 
    {
        NMTVCUSTOMDRAW* tv = reinterpret_cast<NMTVCUSTOMDRAW*>(cd);
        HTREEITEM     item = reinterpret_cast<HTREEITEM>(cd->dwItemSpec);
        if (GetItemState(item, TVIS_SELECTED | TVIS_DROPHILITED) & (TVIS_SELECTED | TVIS_DROPHILITED)) {
            tv->clrText = Owner->MyHotTextColor;
            tv->clrTextBk = Owner->MyHotBackColor;
        }
        else {
            tv->clrText = Owner->MyTextColor;
            tv->clrTextBk = Owner->MyBackColor;
        }
        return CDRF_DODEFAULT;
    }

    template <>
    WCDAFX_API BOOL Colorizer::Control<WTL::CTreeViewCtrl>::SpecificMessagesHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD)
    {
        //NOTIFY_CODE_HANDLER_EX(TVN_GETDISPINFO, OnGetDispinfo)
        CHAIN_MSG_MAP(CustomDraw)
        return FALSE;
    }

#pragma endregion
}
