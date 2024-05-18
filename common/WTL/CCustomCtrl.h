#pragma once

#include <atlwin.h>

template <typename T, typename TBase = ATL::CWindowImpl<T>>
struct CCustomControl: TBase
{
    ~CCustomControl() override = default;

    virtual HRESULT PreCreateWindow();

protected:
    static ATOM gs_Atom;

    CCustomControl() = default;
};

template <typename T, typename TBase>
__declspec(selectany) ATOM CCustomControl<T, TBase>::gs_Atom{0};

template <typename T, typename TBase>
inline HRESULT CCustomControl<T, TBase>::PreCreateWindow()
{
    HRESULT code = S_OK;
    // ##TODO: gs_Atom is not ThreadSafe!
    // look at CStaticDataInitCriticalSectionLock lock;
    if (!gs_Atom) {
        const ATOM atom = ATL::AtlModuleRegisterClassExW(nullptr, &T::GetWndClassInfo().m_wc);
        if (!atom) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        // ##TODO: gs_Atom is not ThreadSafe!
        gs_Atom = atom;
    }
    if (!this->m_thunk.Init(nullptr, nullptr)) {
        code = static_cast<HRESULT>(ERROR_OUTOFMEMORY);
        SetLastError(static_cast<DWORD>(code));
        return code;
    }
    WTL::ModuleHelper::AddCreateWndData(&this->m_thunk.cd, this);
    return S_OK;
}
