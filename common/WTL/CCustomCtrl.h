#pragma once

#include <wcdafx.api.h>
#include <dh.tracing.h>
#include <atlwin.h>
#include <atlapp.h>

template <typename Customer, typename TBase = ATL::CWindow>
struct CCustomControl: ATL::CWindowImpl<Customer, TBase>
{
    using WndSuper = ATL::CWindowImpl<Customer, TBase>;

    DELETE_COPY_MOVE_OF(CCustomControl);

    ~CCustomControl() override;

    virtual HRESULT PreCreateWindow() = 0;

protected:
    CCustomControl();
    HRESULT PreCreateWindowImpl(ATOM& rAtom, ATL::CWndClassInfo const& clsInfo);
};

template <typename Customer, typename TBase>
inline CCustomControl<Customer, TBase>::~CCustomControl() = default;

template <typename Customer, typename TBase>
inline CCustomControl<Customer, TBase>::CCustomControl() = default;

template <typename Customer, typename TBase>
inline HRESULT CCustomControl<Customer, typename TBase>::PreCreateWindowImpl(ATOM& rAtom, ATL::CWndClassInfo const& clsInfo)
{
    Customer* pThis{static_cast<Customer*>(this)};
    WTL::CStaticDataInitCriticalSectionLock lock{};
    HRESULT code = lock.Lock();
    if (FAILED(code)) {
        DH::TPrintf(TL_Error, L"Unable to lock critical section in %s\n", __FUNCTIONW__);
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
        DH::TPrintf(TL_Notify, L"ATOM: %d '%s' for %p\n", rAtom, clsInfo.m_wc.lpszClassName, pThis);
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
