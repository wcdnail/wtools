#pragma once

#include "wcdafx.api.h"
#include "wtl.colorizer.h"
#include "dialogz.types.h"
#include "strint.h"
#include "rect.alloc.h"
#include "cf-resources/resource.h"
#include <atltypes.h>
#include <atlwin.h>

namespace CF
{
    class BasicDialog: public ATL::CDialogImpl<BasicDialog, CF::Colorized::Colorizer>
    {
    public:
        using Super = ATL::CDialogImpl<BasicDialog, CF::Colorized::Colorizer>;

        BasicDialog(BasicDialog const&) = delete;
        BasicDialog& operator = (BasicDialog const&) = delete;

        WCDAFX_API ~BasicDialog() override;
        WCDAFX_API BasicDialog(UINT idd = IDD_BASIC, unsigned flags = 0);
        WCDAFX_API BasicDialog(WStrView message, WStrView title, unsigned flags);

        /**
         *  hResInst == nullptr, load from _AtlBaseModule.GetResourceInstance()
         *  Check GetLastError if failure
         */
        WCDAFX_API bool Create(HINSTANCE hResInst, HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = 0);
        WCDAFX_API bool Create(Rect const& rc, HWND hWndParent = ::GetActiveWindow(), HINSTANCE hResInst = nullptr, LPARAM param = 0);

        /**
         *  hResInst == nullptr, load from _AtlBaseModule.GetResourceInstance()
         *  Check GetLastError if failure
         */
        WCDAFX_API bool ShowModal(HINSTANCE hResInst, HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = 0);
        WCDAFX_API bool ShowModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = 0);


        WCDAFX_API DialogResult Result() const;

        WCDAFX_API static unsigned GetCompatFlags(unsigned flags);

    protected:
        friend Super;

        UINT              IDD;
        DialogResult m_result;
        unsigned      m_attrs;
        WString       m_title;
        WString        m_text;
        CIcon          m_icon;
        CFont          m_font;
        CFont      m_textFont;

        enum ControlIds
        {
            IdFirst = 1001,
            IdOk = IdFirst,
            IdYes,
            IdNo,
            IdCancel,
            IdAbort,
            IdRetry,
            IdIgnore,
            IdClose,
            IdHelp,
            IdContinue,
            IdLast
        };

        CRect SetupIcon(RectzAllocator<LONG>& rcAlloc);

        WCDAFX_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
        WCDAFX_API BOOL OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) override;

    private:
        void CreateButtons(CF::RectzAllocator<LONG>& btnAlloc) const;
        int OnCreate(LPCREATESTRUCT cs);
        BOOL OnInitDialog(HWND focusedWindow, LPARAM param);
        void OnDestroy();
        void OnKeyDown(UINT key, UINT rep, UINT flags);
        void OnCommand(UINT code, int id, HWND control);
    };
}
