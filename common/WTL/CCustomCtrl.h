#pragma once

#include <wcdafx.api.h>
#include <atlwin.h>
#include <atlapp.h>

template <typename Customer>
struct CCustomControl: ATL::CWindowImpl<Customer>
{
    using WndSuper = ATL::CWindowImpl<Customer>;

    DELETE_MOVE_OF(CCustomControl);

    ~CCustomControl() override;

protected:
    CCustomControl();
    HRESULT PreCreateWindowImpl(ATOM& rAtom, ATL::CWndClassInfo const& clsInfo);
};

template <typename Customer>
inline CCustomControl<Customer>::~CCustomControl() = default;

template <typename Customer>
inline CCustomControl<Customer>::CCustomControl() = default;

//inline HRESULT CCustomControl<Customer>::PreCreateWindow()
//{
//    return ERROR_CALL_NOT_IMPLEMENTED;
//}

template <typename Customer>
inline HRESULT CCustomControl<Customer>::PreCreateWindowImpl(ATOM& rAtom, ATL::CWndClassInfo const& clsInfo)
{
    Customer* pThis{static_cast<Customer*>(this)};
    WTL::CStaticDataInitCriticalSectionLock lock{};
    HRESULT code = lock.Lock();
    if (FAILED(code)) {
        ATLTRACE2(ATL::atlTraceRegistrar, 0, _T("ERROR : Unable to lock critical section in CCustomControl::PreCreateWindowImpl.\n"));
        ATLASSERT(FALSE);
        return code;
    }
    if (!rAtom) {
        const ATOM atom = ATL::AtlModuleRegisterClassEx(nullptr, &clsInfo.m_wc);
        if (!atom) {
            lock.Unlock();
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        rAtom = atom;
        ATLTRACE(ATL::atlTraceRegistrar, 0, _T("ATOM: %d '%s' for %p\n"),
            rAtom, clsInfo.m_wc.lpszClassName, pThis);
    }
    lock.Unlock();
    if (!pThis->m_thunk.Init(nullptr, nullptr)) {
        code = static_cast<HRESULT>(ERROR_OUTOFMEMORY);
        SetLastError(static_cast<DWORD>(code));
        return code;
    }
    WTL::ModuleHelper::AddCreateWndData(&pThis->m_thunk.cd, pThis);
    return S_OK;
}
