#pragma once

#include "wcdafx.api.h"
#include "wtl.control.defs.h"
#include <atlwin.h>
#include <atlcrack.h>

namespace CF
{
    struct ControlBase;

    using  ControlPtr = std::unique_ptr<ControlBase>;

    struct ControlBase: CWindow
    {
        enum CtlFlags : uint64_t
        {
            MESSAGE_HANDLED = 0x0000000000000001
        };

        WCDAFX_API virtual ~ControlBase();
        WCDAFX_API ControlBase();
        WCDAFX_API ControlBase(HWND hWnd);
        
        WCDAFX_API BOOL IsMsgHandled() const;
        WCDAFX_API void SetMsgHandled(BOOL bHandled);

        WCDAFX_API void OnContruct(PCTSTR thisClass);
        WCDAFX_API void OnDestroy(PCTSTR thisClass);
        WCDAFX_API virtual BOOL OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

    protected:
        uint64_t                      m_Flags;
        DEBUG_FIELD_DECL(CString, m_prevClass);
    };
}
