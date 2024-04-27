#pragma once

#include "wtl.colorizer.h"
#include "wtl.colorizer.helpers.h"
#include "wtl.colorizer.control.details.h"
#include "wtl.colorizer.control.specific.h"
#include "dh.tracing.h"
#include "windows.gdi.rects.h"
#include <dev.assistance/dev.assist.h>

namespace Cf
{
    #pragma region ControlBase

    struct Colorizer::ControlBase
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
    struct Colorizer::Control: ControlBase,
                               ATL::CWindowImpl<Control<T>, T>,
                               WTL::CCustomDraw<Control<T>>
    {
        using      Super = ATL::CWindowImpl<Control<T>, T>;
        using CustomDraw = WTL::CCustomDraw<Control<T>>;

        Control(Control const&) = delete;
        Control& operator = (Control const&) = delete;

        Control(HWND hwnd, Colorizer* owner);
        ~Control() override;

        ImplBase* Impl() override;

        CString GetItemText(int index) const;
        int GetImageIndex(int index) const;
        HIMAGELIST GetImageList() const;

        using Super::operator HWND;

    private:
        friend class Super;
        friend class OwnerDraw;
        friend class CustomDraw;

        Colorizer*           Owner;
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

        #define CALL_SPECIFIC_HANDLER()                                                     \
            if (SpecificMessagesHandler(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)) {  \
                return TRUE;                                                                  \
            }

        static constexpr size_t PaddingAdjust = 
            //sizeof(Owner) + 
            //sizeof(SpecificMembers<T>) +
            3;

        uint8_t reserved[PaddingAdjust];

        BEGIN_MSG_MAP_EX(Control)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_NCPAINT(OnNcPaint)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            CALL_SPECIFIC_HANDLER()
            DEFAULT_REFLECTION_HANDLER()
        END_MSG_MAP()

        #pragma endregion
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
        UNREFERENCED_ARG(hWnd);
        UNREFERENCED_ARG(uMsg);
        UNREFERENCED_ARG(wParam);
        UNREFERENCED_ARG(lParam);
        UNREFERENCED_ARG(lResult);
        UNREFERENCED_ARG(dwMsgMapID);
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
}
