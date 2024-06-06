#include "stdafx.h"
#include "wtl.control.h"
#include "dh.tracing.h"
#include "dev.assistance/dev.assist.h"

namespace CF
{
    ControlBase::~ControlBase()
    {
    }

    ControlBase::ControlBase()
        :              m_Flags(0)
        DEBUG_FIELD_CTOR_INIT0(m_prevClass)
    {
    }

    ControlBase::ControlBase(HWND hWnd)
        : ControlBase()
    {
        UNREFERENCED_ARG(hWnd);
    }

    BOOL ControlBase::IsMsgHandled() const
    {
        return (0 != (m_Flags & MESSAGE_HANDLED));
    }

    void ControlBase::SetMsgHandled(BOOL bHandled)
    {
        if (bHandled) {
            m_Flags |= MESSAGE_HANDLED;
        }
        else {
            m_Flags &= ~MESSAGE_HANDLED;
        }
    }

#ifdef _DEBUG
    static void traceCtrl(HWND hWnd, CString& prevClass, PCWSTR thisClass, PCWSTR format)
    {
        CStringW      caption;
        int                id = GetDlgCtrlID(hWnd);
        wchar_t     text[256] = {0};
        wchar_t dispText[256] = {0};
        wchar_t   _class[256] = {0};
        int clen = GetClassNameW(hWnd, _class, _countof(_class)-1);
        prevClass = CString(_class, clen);
        int textLen = GetWindowTextW(hWnd, text, _countof(text)-1);
        if (textLen > 0) {
            DH::MakePrintable(dispText, text, static_cast<size_t>(textLen));
            caption.Format(L"%d '%s'", id, dispText);
        }
        else {
            caption.Format(L"%d", id);
        }
        DBGTPrint(0, format, hWnd, caption.GetString(), prevClass.GetString(), thisClass);
    }
#else
#  define traceCtrl(...)
#endif

    void ControlBase::OnContruct(PCTSTR thisClass)
    {
        traceCtrl(m_hWnd, m_prevClass, thisClass, L"++ %p [%s] '%s' ==> '%s'\n");
    }

    void ControlBase::OnDestroy(PCTSTR thisClass)
    {
        traceCtrl(m_hWnd, m_prevClass, thisClass, L"-- %p [%s] '%s' <== '%s'\n");
    }

    BOOL ControlBase::OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        UNREFERENCED_ARG(hWnd);
        UNREFERENCED_ARG(uMsg);
        UNREFERENCED_ARG(wParam);
        UNREFERENCED_ARG(lParam);
        UNREFERENCED_ARG(lResult);
        UNREFERENCED_ARG(dwMsgMapID);
#ifdef _DEBUG
        if (1) {
            MSG msg;
            msg.hwnd = hWnd;
            msg.message = uMsg;
            msg.wParam = wParam;
            msg.lParam = lParam;
            auto msgStr = DH::MessageToStrignW(&msg);
            DBGTPrint(0, L"ControlBase: -WM- [[ %s ]]\n", msgStr.c_str());
        }
#endif
        SetMsgHandled(FALSE);
        return FALSE;
    }
}
