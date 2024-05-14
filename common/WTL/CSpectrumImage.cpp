#include "stdafx.h"
#include "CSpectrumImage.h"

ATOM CSpectrumImage::gs_Atom{0};

CSpectrumImage::~CSpectrumImage() = default;
CSpectrumImage::CSpectrumImage() = default;

HRESULT CSpectrumImage::PreCreateWindow()
{
    HRESULT code = S_OK;
    // ##TODO: gs_Atom is not ThreadSafe!
    // look at CStaticDataInitCriticalSectionLock lock;
    if (!gs_Atom) {
        const ATOM atom = ATL::AtlModuleRegisterClassExW(nullptr, &GetWndClassInfo().m_wc);
        if (!atom) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        // ##TODO: gs_Atom is not ThreadSafe!
        gs_Atom = atom;
    }
    if (!m_thunk.Init(nullptr, nullptr)) {
        code = static_cast<HRESULT>(ERROR_OUTOFMEMORY);
        SetLastError(static_cast<DWORD>(code));
        return code;
    }
    WTL::ModuleHelper::AddCreateWndData(&m_thunk.cd, this);
    return S_OK;
}

BOOL CSpectrumImage::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL const bSave{m_bMsgHandled};
    BOOL const  bRet{_ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)};
    m_bMsgHandled = bSave;
    return bRet;
}

BOOL CSpectrumImage::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}
