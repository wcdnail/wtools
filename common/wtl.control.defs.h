#pragma once

#include "wcdafx.api.h"
#include <atlcrack.h>

namespace CF
{
}

#ifdef _DEBUG
#  define DEBUG_FIELD_DECL(Type, Name)           Type Name
#  define DEBUG_FIELD_CTOR_INIT0(Name)           , Name()
#  define DEBUG_FIELD_CTOR_INIT1(Name, Value)    , Name(Value)
#else
#  define DEBUG_FIELD_DECL(Type, Name)
#  define DEBUG_FIELD_CTOR_INIT0(Name)
#  define DEBUG_FIELD_CTOR_INIT1(Name, Value)
#endif

#define DECL_MSG_MAP_PROC(FnName)    \
    BOOL FnName(HWND        hWnd,     \
                UINT        uMsg,      \
                WPARAM    wParam,       \
                LPARAM    lParam,        \
                LRESULT& lResult,         \
                DWORD dwMsgMapID = 0)

#define IMPL_MSG_MAP_DISP(theClass, ImplFn, DrvdFn)    \
    BOOL theClass::ImplFn(HWND        hWnd,             \
                          UINT        uMsg,              \
                          WPARAM    wParam,               \
                          LPARAM    lParam,                \
                          LRESULT& lResult,                 \
                          DWORD dwMsgMapID)                  \
    {                                                         \
        BOOL bRev = IsMsgHandled();                            \
        BOOL bRet = DrvdFn(hWnd, uMsg, wParam, lParam,          \
                                    lResult, dwMsgMapID);        \
        SetMsgHandled(bRev);                                      \
        return bRet;                                               \
    }

#define IMPL_MSG_MAP_DISP_INH(theClass, ImplFn, DrvdFn, CallInh)       \
    BOOL theClass::ImplFn(HWND        hWnd,                             \
                          UINT        uMsg,                              \
                          WPARAM    wParam,                               \
                          LPARAM    lParam,                                \
                          LRESULT& lResult,                                 \
                          DWORD dwMsgMapID)                                  \
    {                                                                         \
        BOOL bRev = IsMsgHandled();                                            \
        BOOL bRet = CallInh(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);   \
        if (!bRet) {                                                             \
            bRet = DrvdFn(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);       \
        }                                                                          \
        SetMsgHandled(bRev);                                                        \
        return bRet;                                                                 \
    }

#define IMPL_MSG_MAP_BEG(theClass, FnName)       \
    BOOL theClass::FnName(HWND        hWnd,       \
                          UINT        uMsg,        \
                          WPARAM    wParam,         \
                          LPARAM    lParam,          \
                          LRESULT& lResult,           \
                          DWORD dwMsgMapID)            \
    {                                                   \
        BOOL bHandled = TRUE;                            \
        (hWnd);                                           \
        (uMsg);                                            \
        (wParam);                                           \
        (lParam);                                            \
        (lResult);                                            \
        (bHandled);                                            \
        switch(dwMsgMapID) {                                    \
        case 0:


#define DECL_MSG_MAP_EX(theClass)           \
    DECL_MSG_MAP_PROC(ProcessWindowMessage); \
    WCDAFX_API DECL_MSG_MAP_PROC(OnWindowMessage)

#define DECL_MSG_MAP_EX_INHERITED(theClass)          \
    DECL_MSG_MAP_PROC(ProcessWindowMessage) override; \
    WCDAFX_API DECL_MSG_MAP_PROC(OnWindowMessage)

#define DECL_MSG_MAP_EX_CTL_INHERITED(theClass) \
    DECL_MSG_MAP_EX_INHERITED(theClass) override

#define IMPL_MSG_MAP_EX(theClass) \
    IMPL_MSG_MAP_DISP(theClass, ProcessWindowMessage, OnWindowMessage) \
    IMPL_MSG_MAP_BEG(theClass, OnWindowMessage)

#define CHAIN_MSG_MAP_CUST(theChainMethod) {                    \
    if (theChainMethod(hWnd, uMsg, wParam, lParam, lResult)) {   \
        return TRUE;                                              \
    }}

#define DECL_OVERRIDE_MSG_MAP_EX(theClass) \
    DECL_MSG_MAP_PROC(OnWindowMessage) override

#define IMPL_OVERRIDE_MSG_MAP_EX(theClass) \
    IMPL_MSG_MAP_BEG(theClass, OnWindowMessage)

#define HANDLE_SUPER_MSG_MAP_EX(theClass) {                               \
    if (theClass::OnWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {  \
        return TRUE;                                                        \
    }}
