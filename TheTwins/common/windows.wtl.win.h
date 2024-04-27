#ifndef _CF_windows_wtl_win_h__
#define _CF_windows_wtl_win_h__

#include "app.language.h"
#include <atlwin.h>
#include <atlstr.h>
#include <crash.report/crash.report.h>

namespace CF // Common framework
{
    template <class Base = ATL::CWindow>
    class CommonWindow: public Base
    {
    public:
        typedef typename Base Super;

        virtual ~CommonWindow();

        BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);

        static void OnException(char const* message = NULL, DWORD code = ::GetLastError());

    protected:
        CommonWindow(HMODULE resourceModuleHandle = WTL::ModuleHelper::GetResourceInstance());

        bool IsMsgHandled() const { return messageHandled_; }
        void SetMsgHandled(bool itis) { messageHandled_ = itis; }

        unsigned int GetLanguageId() const { return languageId_; }
        HMODULE GetResourceModuleHandle() const { return resourceModuleHandle_; }

        void SetItemText(int ctlId, UINT strId) const;
        void SetItemTextWithSameId(UINT ctlId) const;
        void SetItemText(HWND ctlHandle, UINT strId) const;

        template <class Data>
        int AddComboItem(HWND ctlCombo, UINT strId, Data data) const;

        template <class Data>
        static void GetComboValue(HWND ctlCombo, Data& value);

        template <class Data>
        static void SetComboCurSel(HWND ctlCombo, Data const& value);

        ATL::CString LoadString(UINT id) const;

    private:
        bool messageHandled_;
        HMODULE resourceModuleHandle_;

        virtual BOOL OnWindowMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD) = 0;

    private:
        CommonWindow(CommonWindow const&);
        CommonWindow& operator = (CommonWindow const&);
    };

    template <class Base>
    inline CommonWindow<Base>::~CommonWindow()
    {}

    template <class Base>
    inline CommonWindow<Base>::CommonWindow(HMODULE resourceModuleHandle /* = WTL::ModuleHelper::GetResourceInstance()*/)
        : messageHandled_(FALSE)
        , resourceModuleHandle_(resourceModuleHandle)
    {}

    template <class Base>
    inline void CommonWindow<Base>::OnException(char const* message, DWORD code)
    {
        ::CRASH_REPORT(message, code);
        ::PostQuitMessage(code);
    }

    template <class Base>
    inline BOOL CommonWindow<Base>::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
    {
        try
        {
            bool lastRv = messageHandled_;
            BOOL rv = OnWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
            messageHandled_ = (bool)lastRv;
            return (BOOL)rv;
        }
        catch (std::exception const& ex)
        {
            OnException(ex.what());
        }
        catch (...)
        {
            OnException(NULL);
        }

        return FALSE;
    }

    template <class Base>
    inline BOOL CommonWindow<Base>::OnWindowMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD)
    {
        return FALSE;
    }

    template <class Base>
    inline void CommonWindow<Base>::SetItemText(HWND ctlHandle, UINT strId) const
    {
        ::SetWindowText(ctlHandle, LoadString(strId));
    }

    template <class Base>
    inline void CommonWindow<Base>::SetItemText(int ctlId, UINT strId) const
    {
        SetItemText(::GetDlgItem(m_hWnd, ctlId), strId);
    }

    template <class Base>
    inline void CommonWindow<Base>::SetItemTextWithSameId(UINT id) const
    {
        SetItemText(id, id);
    }

    template <class Base>
    template <class Data>
    inline int CommonWindow<Base>::AddComboItem(HWND ctlCombo, UINT strId, Data data) const
    {
        int n = (int)::SendMessage(ctlCombo, CB_ADDSTRING, 0, (LPARAM)(PCTSTR)LoadString(strId));
        if (LB_ERR != n)
            ::SendMessage(ctlCombo, CB_SETITEMDATA, n, (LPARAM)data);
        return n;
    }

    template <class Base>
    template <class Data>
    inline void CommonWindow<Base>::GetComboValue(HWND ctlCombo, Data& value)
    {
        int selected = (int)::SendMessage(ctlCombo, CB_GETCURSEL, 0, 0L);
        if (CB_ERR != selected)
        {
            value = (Data)(DWORD_PTR)::SendMessage(ctlCombo, CB_GETITEMDATA, selected, 0L);
        }
    }

    template <class Base>
    template <class Data>
    inline void CommonWindow<Base>::SetComboCurSel(HWND ctlCombo, Data const& value)
    {
        int count = (int)::SendMessage(ctlCombo, CB_GETCOUNT, 0, 0);
        for (int i=0; i<count; i++)
        {
            Data temp = (Data)(DWORD_PTR)::SendMessage(ctlCombo, CB_GETITEMDATA, i, 0L);
            if (value == temp)
            {
                ::SendMessage(ctlCombo, CB_SETCURSEL, i, 0);
                break;
            }
        }
    }

    template <class Base>
    inline ATL::CString CommonWindow<Base>::LoadString(UINT id) const
    {
        ATL::CString result;
        result.LoadString(resourceModuleHandle_, id, (WORD)AppLanguage().GetId());
        return result;
    }
}

#endif // _CF_windows_wtl_win_h__
