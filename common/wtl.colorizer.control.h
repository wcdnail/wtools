#pragma once

#include "wtl.control.h"
#include "wtl.colorizer.control.specific.h"

namespace CF::Colorized
{
    class Colorizer;

    //
    // CWindowImplRoot<TBase>: -> TBase, -> CMessageMap
    //   |
    //   -- CWindowImplRoot<TBase>
    //          |
    //          -- CWindowImplBaseT<TBase, TWinTraits>
    //                 |
    //                 -- CWindowImpl<T, TBase, TWinTraits>
    //                                |
    //                                -- This class
    //

    template <typename T>
    struct Control: public ATL::CWindowImpl<Control<T>, T>
    {
        using Super = ATL::CWindowImpl<Control<T>, T>;

        Control(Control const&) = delete;
        Control& operator = (Control const&) = delete;

        ~Control() override;
        Control(HWND hwnd, Colorizer& master);

        CString GetItemText(int index) const;
        int GetImageIndex(int index) const;
        HIMAGELIST GetImageList() const;

    private:
        friend class Super;
        friend class OwnerDraw;
        friend class CustomDraw;

        Colorizer&  m_Master;
        SpecImpl<T>   m_Spec;

        void Init(HWND hwnd);
        void InitSpec(HWND hwnd);

        void OnDestroyThiz();
        void OnNcPaint(CRgnHandle);
        void OnPaint(CDCHandle);
        BOOL OnEraseBkgnd(CDCHandle dc);
        void OnDrawItem(int id, LPDRAWITEMSTRUCT di);
        HBRUSH OnCtlColorStatic(CDCHandle dc, HWND hwnd);
        HBRUSH OnCtlColorListBox(CDCHandle dc, HWND hwnd);
        HBRUSH OnCtlColorEdit(CDCHandle dc, HWND hwnd);
        HBRUSH OnCtlColorScrollBar(CDCHandle dc, HWND hwnd);
        DWORD OnPrePaint(int id, LPNMCUSTOMDRAW cd);
        DWORD OnItemPrePaint(int id, LPNMCUSTOMDRAW cd);
        LRESULT OnGetDispinfo(LPNMHDR header);

        BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
        BOOL OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
        LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    };

#pragma endregion
}
