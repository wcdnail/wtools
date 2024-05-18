#pragma once

#include <atlwin.h>
#include <atlapp.h>

template <typename T, typename TBase = ATL::CWindow>
struct CCustomControl: ATL::CWindowImpl<T, TBase>
{
    ~CCustomControl() override = default;

    virtual HRESULT PreCreateWindow();

protected:
    CCustomControl() = default;
};

template <typename T, typename TBase>
inline HRESULT CCustomControl<T, TBase>::PreCreateWindow()
{
    WTL::CStaticDataInitCriticalSectionLock lock;
    HRESULT code = lock.Lock();
    if (FAILED(code)) {
        return code;
    }
    ATOM& rAtom = T::GetWndClassAtomRef();
    if (!rAtom) {
        const ATOM atom = ATL::AtlModuleRegisterClassExW(nullptr, &T::GetWndClassInfo().m_wc);
        if (!atom) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        // ##TODO: gs_Atom is not ThreadSafe!
        rAtom = atom;
    }
    lock.Unlock();
    if (!this->m_thunk.Init(nullptr, nullptr)) {
        code = static_cast<HRESULT>(ERROR_OUTOFMEMORY);
        SetLastError(static_cast<DWORD>(code));
        return code;
    }
    WTL::ModuleHelper::AddCreateWndData(&this->m_thunk.cd, this);

    ATLTRACE(ATL::atlTraceRegistrar, 0, _T("ATOM: %d '%s' for %p\n"),
        T::GetWndClassAtomRef(),
        T::GetWndClassInfo().m_wc.lpszClassName,
        this
    );
    return S_OK;
}
