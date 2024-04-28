#include "stdafx.h"
#include "wtl.control.h"
#include "dh.tracing.h"
#include <stdexcept>
#include <format>

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

    void ControlBase::OnContruct(PCTSTR thisClass)
    {
#ifdef _DEBUG
        wchar_t _class[256] = {0};
        int clen = GetClassNameW(m_hWnd, _class, _countof(_class)-1);
        m_prevClass = CString(_class, clen);

        CStringW text;
        GetWindowTextW(text);

        CStringW dispText;
        Dh::Impl::CopyCharsForPrinting(dispText.GetBufferSetLength(text.GetLength() + 1),
                                       static_cast<size_t>(text.GetLength() + 1),
                                       text.GetString(),
                                       static_cast<size_t>(text.GetLength())
                                       );
        dispText.ReleaseBuffer(text.GetLength());

        Dh::ThreadPrintf(_T("Colorize: \t\t++ %p [%s] `%s` ==> `%s`\n"), m_hWnd,
            dispText.GetString(), m_prevClass.GetString(), thisClass);
#endif
    }

    void ControlBase::OnDestroy(PCTSTR thisClass)
    {
#ifdef _DEBUG
        CStringW text;
        GetWindowTextW(text);

        CStringW dispText;
        Dh::Impl::CopyCharsForPrinting(dispText.GetBufferSetLength(text.GetLength() + 1),
                                       static_cast<size_t>(text.GetLength() + 1),
                                       text.GetString(),
                                       static_cast<size_t>(text.GetLength())
                                       );
        dispText.ReleaseBuffer(text.GetLength());

        Dh::ThreadPrintf(L"Colorize: \t\t-- %p [%s] `%s` <== `%s`\n", m_hWnd,
            dispText.GetString(), m_prevClass.GetString(), thisClass);
#endif
    }

    BOOL ControlBase::OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        UNREFERENCED_ARG(hWnd);
        UNREFERENCED_ARG(uMsg);
        UNREFERENCED_ARG(wParam);
        UNREFERENCED_ARG(lParam);
        UNREFERENCED_ARG(lResult);
        UNREFERENCED_ARG(dwMsgMapID);
        return FALSE;
    }
}
