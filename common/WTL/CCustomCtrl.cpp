#include "stdafx.h"
#include "CCustomCtrl.h"
#include <atlapp.h>

CCustomControl::~CCustomControl() = default;
CCustomControl::CCustomControl() = default;

HRESULT CCustomControl::PreCreateWindow()
{
    return ERROR_CALL_NOT_IMPLEMENTED;
}

HRESULT CCustomControl::PreCreateWindowImpl(ATOM& rAtom, ATL::CWndClassInfo const& clsInfo)
{
    WTL::CStaticDataInitCriticalSectionLock lock;
    HRESULT code = lock.Lock();
    if (FAILED(code)) {
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
    }
    lock.Unlock();
    if (!this->m_thunk.Init(nullptr, nullptr)) {
        code = static_cast<HRESULT>(ERROR_OUTOFMEMORY);
        SetLastError(static_cast<DWORD>(code));
        return code;
    }
    WTL::ModuleHelper::AddCreateWndData(&this->m_thunk.cd, this);

    ATLTRACE(ATL::atlTraceRegistrar, 0, _T("ATOM: %d '%s' for %p\n"),
        rAtom,
        clsInfo.m_wc.lpszClassName,
        this
    );
    return S_OK;
}
