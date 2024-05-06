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

    HRESULT PreCreateWindow();

    CColorPicker& operator = (HWND hWnd); // make compatible with WTL/DDX

    BEGIN_MSG_MAP_EX(CColorPicker)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_PAINT(OnPaint)
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT pCS);
    BOOL OnEraseBkgnd(CDCHandle dc) const;
    void OnPaint(CDCHandle);
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

inline HRESULT CColorPicker::PreCreateWindow()
{
    HRESULT code = S_OK;
    // ##TODO: gs_Atom is not ThreadSafe!
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
    //CRect rc;
    //GetClientRect(rc);
    //dc.FillSolidRect(rc, m_clrBkgnd);
    return TRUE;
}

inline void CColorPicker::OnPaint(CDCHandle)
{
    CPaintDC paintDC{m_hWnd};
    CDCHandle     dc{paintDC};
    CRect         rc{};
    COLORREF    clLT{0x00ffffff};
    COLORREF    clRT{0x00fc2500};
    COLORREF    clRB{0x00000000};
    COLORREF    clLB{0x00000000};

    GetClientRect(rc);
    dc.FillSolidRect(rc, m_clrBkgnd);

    TRIVERTEX vertex[4] = 
    {
        { rc.left, rc.top,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clLT)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clLT)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clLT)) << 8),
            0x0000
        },
        { rc.right, rc.top,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clRT)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clRT)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clRT)) << 8),
            0x0000
        },
        { rc.right, rc.bottom,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clRB)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clRB)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clRB)) << 8),
            0x0000
        },
        { rc.left, rc.bottom,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clLB)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clLB)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clLB)) << 8),
            0x0000
        },
    };

    GRADIENT_TRIANGLE triGrad[2] = {{1, 2, 3},{0, 1, 3}};
    GradientFill(dc, vertex, _countof(vertex), triGrad, _countof(triGrad), GRADIENT_FILL_TRIANGLE);

}

inline int CColorPicker::OnCreate(LPCREATESTRUCT pCS)
{
    return 0;
}
