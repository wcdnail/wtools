#pragma once

#include "wcdafx.api.h"
#include <atlwin.h>

#ifdef _DEBUG
#  define DEBUG_FIELD_DECL(Type, Name)           Type Name
#  define DEBUG_FIELD_CTOR_INIT0(Name)           , Name()
#  define DEBUG_FIELD_CTOR_INIT1(Name, Value)    , Name(Value)
#else
#  define DEBUG_FIELD_DECL(Type, Name)
#  define DEBUG_FIELD_CTOR_INIT0(Name)
#  define DEBUG_FIELD_CTOR_INIT1(Name, Value)
#endif

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
