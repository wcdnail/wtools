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
    COLORREF    clCC{0x005a5a5a};
    COLORREF    clWT{0x00ffffff};
    COLORREF    clTG{0x00fc2500};
    COLORREF    clBK{0x00000000};

    GetClientRect(rc);
    dc.FillSolidRect(rc, m_clrBkgnd);

    TRIVERTEX vertex[] = 
    {
        { rc.Width() / 2, rc.Height() / 2,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clCC)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clCC)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clCC)) << 8),
            0x0000
        },
        { rc.left, rc.top,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clWT)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clWT)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clWT)) << 8),
            0x0000
        },
        { rc.right, rc.top,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clTG)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clTG)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clTG)) << 8),
            0x0000
        },
        { rc.right, rc.bottom,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clBK)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clBK)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clBK)) << 8),
            0x0000
        },
        { rc.left, rc.bottom,
            static_cast<COLOR16>(static_cast<USHORT>(GetRValue(clBK)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetGValue(clBK)) << 8),
            static_cast<COLOR16>(static_cast<USHORT>(GetBValue(clBK)) << 8),
            0x0000
        },
    };

    GRADIENT_TRIANGLE triGrad[] = {{0, 2, 1}, {2, 3, 0}, {4, 3, 0}};
    GradientFill(dc, vertex, _countof(vertex), triGrad, _countof(triGrad), GRADIENT_FILL_TRIANGLE);

}

inline int CColorPicker::OnCreate(LPCREATESTRUCT pCS)
{
    return 0;
}
