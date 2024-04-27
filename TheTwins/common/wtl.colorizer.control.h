#pragma once

#include "wtl.colorizer.h"
#include "wtl.colorizer.helpers.h"
#include "wtl.colorizer.control.details.h"
#include "wtl.colorizer.control.specific.h"
#include "dh.tracing.h"
#include "windows.gdi.rects.h"
#include <dev.assistance/dev.assist.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <boost/noncopyable.hpp>

namespace Cf
{
    #pragma region ControlBase

    struct Colorizer::ControlBase: boost::noncopyable
    {
        typedef ATL::CWindowImpl<ControlBase> ImplBase;

        ControlBase(HWND hwnd, PCTSTR childClass);
        virtual ~ControlBase();
        virtual ImplBase* Impl();

    protected:
        void OnDestroy(HWND hwnd, PCTSTR childClass);

    private:
#ifdef _DEBUG
        CString PrevClass;
#endif
    };

    #pragma endregion 

    template <typename T>
    struct Colorizer::Control: ControlBase
                             , ATL::CWindowImpl<Control<T>, T>
                             , WTL::CCustomDraw<Control<T> >
    {
        typedef ATL::CWindowImpl<Control<T>, T> Super;
        typedef WTL::CCustomDraw<Control<T> > CustomDraw;

        Control(HWND hwnd, Colorizer* owner);
        virtual ~Control();

        virtual ImplBase* Impl();

        CString GetItemText(int index) const;
        int GetImageIndex(int index) const;
        HIMAGELIST GetImageList() const;

        using Super::operator HWND;

    private:
        friend class Super;
        friend class OwnerDraw;
        friend class CustomDraw;

        Colorizer* Owner;
        SpecificMembers<T> Members;

        void SpecificPreInit(HWND hwnd);
        BOOL SpecificMessagesHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

        void OnDestroy();
        void OnPaint(CDCHandle);
        void OnNcPaint(CRgnHandle);
        BOOL OnEraseBkgnd(CDCHandle dc);
        void OnDrawItem(int id, LPDRAWITEMSTRUCT di);
        HBRUSH OnCtlColorStatic(CDCHandle dc, HWND hwnd);
        HBRUSH OnCtlColorListBox(CDCHandle dc, HWND hwnd);
        HBRUSH OnCtlColorEdit(CDCHandle dc, HWND hwnd);
        HBRUSH OnCtlColorScrollBar(CDCHandle dc, HWND hwnd);

        DWORD OnPrePaint(int, LPNMCUSTOMDRAW cd);
        DWORD OnItemPrePaint(int, LPNMCUSTOMDRAW cd);
        LRESULT OnGetDispinfo(LPNMHDR header);

        #pragma region Message map

        #define CALL_SPECIFIC_HANDLER() \
            if (SpecificMessagesHandler(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)) \
                return TRUE;

        BEGIN_MSG_MAP_EX(Control)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_NCPAINT(OnNcPaint)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            CALL_SPECIFIC_HANDLER()
            DEFAULT_REFLECTION_HANDLER()
        END_MSG_MAP()

        #pragma endregion

    private:
        Control(Control const&);
        Control& operator = (Control const&);
    };

    #pragma region Inherited

    template <typename T>
    inline Colorizer::Control<T>::Control(HWND hwnd, Colorizer* owner) 
        : ControlBase(hwnd, Super::GetWndClassName())
        , Owner(owner)
    {
        Super::m_hWnd = NULL;
        SpecificPreInit(hwnd);
        Super::SubclassWindow(hwnd);
    }

    template <typename T>
    inline Colorizer::Control<T>::~Control() 
    {}

    template <typename T>
    inline Colorizer::ControlBase::ImplBase* Colorizer::Control<T>::Impl() 
    {
        return (ImplBase*)this; 
    }

    #pragma endregion 

    #pragma region Generic implementation

    template <typename T>
    inline void Colorizer::Control<T>::SpecificPreInit(HWND)
    {}

    template <typename T>
    inline BOOL Colorizer::Control<T>::SpecificMessagesHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        return FALSE;
    }

    template <typename T>
    inline void Colorizer::Control<T>::OnDestroy()
    {
        ControlBase::OnDestroy(Super::m_hWnd, Super::GetWndClassName());
        SetMsgHandled(FALSE);
    }

    template <typename T>
    inline void Colorizer::Control<T>::OnPaint(CDCHandle)
    {
        SetMsgHandled(FALSE);
    }

    template <typename T>
    inline void Colorizer::Control<T>::OnNcPaint(CRgnHandle rgn)
    {
        BorderFlags bt = Details<T>::GetBorderType(*this);
        if (BorderNone != bt)
        {
            NcPainContext nc(*this, rgn);
            Owner->DrawControlBorder(nc.Dc.m_hDC, nc.Rect, bt);
        }
    }

    template <typename T>
    inline BOOL Colorizer::Control<T>::OnEraseBkgnd(CDCHandle dc)
    {
        Owner->OnEraseBackground<T>(*this, dc);
        return TRUE;
    }

    template <typename T>
    inline DWORD Colorizer::Control<T>::OnPrePaint(int, LPNMCUSTOMDRAW cd)
    {
        SetMsgHandled(FALSE);
        return CDRF_DODEFAULT;
    }

    template <typename T>
    inline DWORD Colorizer::Control<T>::OnItemPrePaint(int, LPNMCUSTOMDRAW cd)
    {
        SetMsgHandled(FALSE);
        return CDRF_DODEFAULT;
    }

    template <typename T>
    inline LRESULT Colorizer::Control<T>::OnGetDispinfo(LPNMHDR header)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    template <typename T>
    inline CString Colorizer::Control<T>::GetItemText(int index) const
    {
        return _T("");
    }

    template <typename T>
    inline int Colorizer::Control<T>::GetImageIndex(int index) const
    {
        return -1;
    }

    template <typename T>
    inline HIMAGELIST Colorizer::Control<T>::GetImageList() const
    {
        return NULL;
    }

    template <typename T>
    inline void Colorizer::Control<T>::OnDrawItem(int id, LPDRAWITEMSTRUCT di)
    {
        Owner->DrawItem<T>(*this, id, di);
    }

    template <typename T>
    inline HBRUSH Colorizer::Control<T>::OnCtlColorStatic(CDCHandle dc, HWND hwnd)
    {
        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    template <typename T>
    inline HBRUSH Colorizer::Control<T>::OnCtlColorListBox(CDCHandle dc, HWND hwnd)
    {
        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    template <typename T>
    inline HBRUSH Colorizer::Control<T>::OnCtlColorEdit(CDCHandle dc, HWND hwnd)
    {
        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    template <typename T>
    inline HBRUSH Colorizer::Control<T>::OnCtlColorScrollBar(CDCHandle dc, HWND hwnd)
    {
        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    #pragma endregion

    #pragma region Static specific

    template <>
    inline void Colorizer::Control<WTL::CStatic>::OnPaint(CDCHandle)
    {
        if (Details<WTL::CStatic>::Normal != Details<WTL::CStatic>::GetAppearType(*this))
            SetMsgHandled(FALSE);

        else
        {
            PaintContext pc(*this, GetFont());

            CString text;
            if (GetWindowText(text))
            {
                Owner->SetTextColor(pc.PaindDC.m_hDC);
                pc.PaindDC.DrawText(text, text.GetLength(), pc.Rect, Details<WTL::CStatic>::GetDrawTextFormat(*this));
            }
        }
    }

    #pragma endregion

    #pragma region Button specific

    template <>
    inline void Colorizer::Control<WTL::CButton>::OnPaint(CDCHandle)
    {
        Details<WTL::CButton>::AppearType type = Details<WTL::CButton>::GetAppearType(*this);

        if ((Details<WTL::CButton>::Ownerdraw == type) || (Details<WTL::CButton>::UserButton == type))
            SetMsgHandled(FALSE);

        else
        {
            PaintContext pc(*this, GetFont());

            CString text;
            GetWindowText(text);

            Owner->SetTextColor(pc.PaindDC.m_hDC);

            switch (type)
            {
            case Details<WTL::CButton>::Groupbox:
                Owner->DrawGroupBox(*this, pc.PaindDC.m_hDC, pc.Rect, text);
                break;

            case Details<WTL::CButton>::PushButton:
            case Details<WTL::CButton>::DefPushButton:
            case Details<WTL::CButton>::Flat: 
                Owner->DrawButton(*this, pc.PaindDC.m_hDC, pc.Rect, text
                    , Details<WTL::CButton>::Flat == type
                    , Details<WTL::CButton>::DefPushButton == type);
                break;

            case Details<WTL::CButton>::Checkbox:
            case Details<WTL::CButton>::AutoCheckbox:
            case Details<WTL::CButton>::ThreeState:
            case Details<WTL::CButton>::Auto3State:
                Owner->DrawCheckBox(*this, pc.PaindDC.m_hDC, pc.Rect, text);
                break;

            case Details<WTL::CButton>::RadioButton:
            case Details<WTL::CButton>::AutoRadioButton:
                Owner->DrawRadioButton(*this, pc.PaindDC.m_hDC, pc.Rect, text);
                break;

            case Details<WTL::CButton>::Pushbox: 
                ::DebugBreak();
                break;
            }
        }
    }

    #pragma endregion

    #pragma region ListBox specific

    template <>
    void Colorizer::Control<WTL::CListBox>::SpecificPreInit(HWND hwnd)
    {
        unsigned ns = ::GetWindowLong(hwnd, GWL_STYLE);
        ns |= LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT;
        ::SetWindowLong(hwnd, GWL_STYLE, ns);
    }

    template <>
    inline CString Colorizer::Control<WTL::CListBox>::GetItemText(int index) const
    {
        CString result;
        GetText(index, result);
        return result;
    }

    template <>
    BOOL Colorizer::Control<WTL::CListBox>::SpecificMessagesHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        MSG_WM_DRAWITEM(OnDrawItem)
        return FALSE;
    }

    #pragma endregion

    #pragma region ComboBox specific

    template <>
    inline CString Colorizer::Control<WTL::CComboBox>::GetItemText(int index) const
    {
        CString result;
        GetLBText(index, result);
        return result;
    }

    template <> inline void Colorizer::Control<WTL::CComboBox>::OnNcPaint(CRgnHandle rgn) {}
    template <> inline BOOL Colorizer::Control<WTL::CComboBox>::OnEraseBkgnd(CDCHandle dc) { return TRUE; }

    template <>
    inline void Colorizer::Control<WTL::CComboBox>::OnPaint(CDCHandle)
    {
        PaintContext pc(*this, GetFont());
        Owner->DrawComboFace<WTL::CComboBox>(*this, pc.PaindDC.m_hDC, pc.Rect);
    }

    template <>
    inline HBRUSH Colorizer::Control<WTL::CComboBox>::OnCtlColorListBox(CDCHandle dc, HWND hwnd)
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
    inline HBRUSH Colorizer::Control<WTL::CComboBox>::OnCtlColorEdit(CDCHandle dc, HWND hwnd)
    {
        Owner->SetTextColor(dc);
        return Owner->MyBackBrush[0];
    }

    template <>
    BOOL Colorizer::Control<WTL::CComboBox>::SpecificMessagesHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
        MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
        return FALSE;
    }

    #pragma endregion

    #pragma region ScrollBar specific

    template <>
    BOOL Colorizer::Control<WTL::CScrollBar>::OnEraseBkgnd(CDCHandle)
    {
        SetMsgHandled(FALSE);
        return FALSE;
    }

    #pragma endregion

    #pragma region TreeView specific

    template <>
    void Colorizer::Control<WTL::CTreeViewCtrl>::SpecificPreInit(HWND hwnd)
    {
        ::SetWindowLong(hwnd, GWL_EXSTYLE
            , ::GetWindowLong(hwnd, GWL_EXSTYLE) 
            | 0x0004 /*TVS_EX_DOUBLEBUFFER*/
            );
    }

    template <>
    LRESULT Colorizer::Control<WTL::CTreeViewCtrl>::OnGetDispinfo(LPNMHDR header)
    {
        LPNMTVDISPINFO di = (LPNMTVDISPINFO)header;
        return 0;
    }

    template <>
    DWORD Colorizer::Control<WTL::CTreeViewCtrl>::OnPrePaint(int, LPNMCUSTOMDRAW cd) 
    {
        CDCHandle dc(cd->hdc);
        dc.FillSolidRect(&cd->rc, Owner->MyBackColor);
        return CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
    }

    template <>
    DWORD Colorizer::Control<WTL::CTreeViewCtrl>::OnItemPrePaint(int, LPNMCUSTOMDRAW cd) 
    {
        NMTVCUSTOMDRAW* tv = (NMTVCUSTOMDRAW*)cd;

        HTREEITEM item = (HTREEITEM)cd->dwItemSpec;
        if (GetItemState(item, TVIS_SELECTED | TVIS_DROPHILITED) & (TVIS_SELECTED | TVIS_DROPHILITED))
        {
            tv->clrText = Owner->MyHotTextColor;
            tv->clrTextBk = Owner->MyHotBackColor;
        }
        else
        {
            tv->clrText = Owner->MyTextColor;
            tv->clrTextBk = Owner->MyBackColor;
        }

        return CDRF_DODEFAULT;
    }

    template <>
    BOOL Colorizer::Control<WTL::CTreeViewCtrl>::SpecificMessagesHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
      //NOTIFY_CODE_HANDLER_EX(TVN_GETDISPINFO, OnGetDispinfo)
        CHAIN_MSG_MAP(CustomDraw);
        return FALSE;
    }

    #pragma endregion
}
