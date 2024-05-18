#pragma once

#include <atlwin.h>
#include <atlapp.h>

template <typename T, typename TBase = ATL::CWindow, typename TWinTraits = ATL::CControlWinTraits>
struct CCustomControl: ATL::CWindowImpl<T, TBase, TWinTraits>
{
    ~CCustomControl() override = default;

    virtual HRESULT PreCreateWindow();

protected:
    CCustomControl() = default;
};

template <typename T, typename TBase, typename TWinTraits>
inline HRESULT CCustomControl<T, TBase, TWinTraits>::PreCreateWindow()
{
    WTL::CStaticDataInitCriticalSectionLock lock;
    HRESULT code = lock.Lock();
    if (FAILED(code)) {
        return code;
    }
    ATOM& rAtom = T::GetWndClassAtomRef();
    if (!rAtom) {
        const ATOM atom = ATL::AtlModuleRegisterClassEx(nullptr, &T::GetWndClassInfo().m_wc);
        if (!atom) {
            lock.Unlock();
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
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
