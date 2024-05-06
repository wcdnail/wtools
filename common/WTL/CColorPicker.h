#pragma once

#include <atlwin.h>
#include <atlcrack.h>

struct CColorPicker;

using  CColorPickerBase = ATL::CWindowImpl<CColorPicker>;

struct CColorPicker: CColorPickerBase
{
    DECLARE_WND_CLASS(_T("CColorPicker"))

    static ATOM gs_Atom;
    COLORREF m_clrBkgnd;

    ~CColorPicker() override;
    CColorPicker();

    HRESULT PreCreateClass();

    CColorPicker& operator = (HWND hWnd); // make compatible with WTL/DDX

    BEGIN_MSG_MAP_EX(CColorPicker)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT pCS);
    BOOL OnEraseBkgnd(CDCHandle dc) const;
};

__declspec(selectany) ATOM CColorPicker::gs_Atom{0};

inline CColorPicker::~CColorPicker()
{
}

inline CColorPicker::CColorPicker()
    : CColorPickerBase{}
    ,       m_clrBkgnd{GetSysColor(COLOR_BTNFACE)}
    ,    m_bMsgHandled{FALSE}
{
}

inline HRESULT CColorPicker::PreCreateClass()
{
    HRESULT code = S_OK;
    // ##TODO: ThreadSafe gs_Atom!
    if (!gs_Atom) {
        const ATOM atom = ATL::AtlModuleRegisterClassExW(nullptr, &GetWndClassInfo().m_wc);
        if (!atom) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        // ##TODO: ThreadSafe gs_Atom!
        gs_Atom = atom;
    }
    if (!m_thunk.Init(nullptr, nullptr)) {
        code = static_cast<HRESULT>(ERROR_OUTOFMEMORY);
        SetLastError(static_cast<DWORD>(code));
        return code;
    }
    ModuleHelper::AddCreateWndData(&m_thunk.cd, this);
    return S_OK;
}

inline CColorPicker& CColorPicker::operator=(HWND hWnd)
{
    Attach(hWnd);
    return *this;
}

inline BOOL CColorPicker::OnEraseBkgnd(CDCHandle dc) const
{
    CRect rc;
    GetClientRect(rc);
    dc.FillSolidRect(rc, m_clrBkgnd);
    return TRUE;
}

inline int CColorPicker::OnCreate(LPCREATESTRUCT pCS)
{
    return 0;
}
