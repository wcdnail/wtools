#include "stdafx.h"
#include "CColorPicker.h"
#include <atldlgs.h>
#include <atlddx.h>

ATOM CColorPicker::gs_Atom{0};

struct CColorPicker::Impl: WTL::CIndirectDialogImpl<Impl>,
                                   WTL::CDialogResize<Impl>
{
    using   ImplSuper = WTL::CIndirectDialogImpl<Impl>;
    using ImplResizer = WTL::CDialogResize<Impl>;

    enum Sizes: short
    {
        DEFAULT_CX = 460,
        DEFAULT_CY = 205,
        SPEC_CX = DEFAULT_CX/2-6,
        SPEC_CY = DEFAULT_CY/3-8,
    };

    ~Impl() override = default;
    Impl() = default;

private:
    friend ImplSuper;
    friend ImplResizer;

    BEGIN_DIALOG(0, 0, DEFAULT_CX, DEFAULT_CY)
        DIALOG_STYLE(WS_CHILD | WS_VISIBLE)
        DIALOG_FONT(8, _T("MS Shell Dlg 2"))
    END_DIALOG()

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 1726,
        IDC_GRP_SPECTRUM,
        IDC_GRP_RGB,
        IDC_GRP_HSL,
        IDC_GRP_HSV,
        IDC_GRP_PICKER,
    };

    BEGIN_CONTROLS_MAP()
        CONTROL_GROUPBOX(_T("Spectrum Color"), IDC_GRP_SPECTRUM,                     4,             4,     SPEC_CX, DEFAULT_CY-8, 0, 0)
        CONTROL_GROUPBOX(_T("RGB Color"),           IDC_GRP_RGB,           8+SPEC_CX+4,             4,     SPEC_CX,      SPEC_CY, 0, 0)
        CONTROL_GROUPBOX(_T("HSL Color"),           IDC_GRP_HSL,           8+SPEC_CX+4,   4+SPEC_CY+4, SPEC_CX/2-4,      SPEC_CY, 0, 0)
        CONTROL_GROUPBOX(_T("HSV Color"),           IDC_GRP_HSV, 8+SPEC_CX+SPEC_CX/2+8,   4+SPEC_CY+4, SPEC_CX/2-4,      SPEC_CY, 0, 0)
        CONTROL_GROUPBOX(_T("Color Picker"),     IDC_GRP_PICKER,           8+SPEC_CX+4, 4+SPEC_CY*2+8,     SPEC_CX,    SPEC_CY+8, 0, 0)
    END_CONTROLS_MAP()

    BEGIN_DDX_MAP(CSpectrumColorPicker)
        //DDX_CONTROL_HANDLE(IDC_CUSTOM_CTL1, m_ccColorPicker)
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(IDC_GRP_SPECTRUM, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_GRP_RGB, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_GRP_HSL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_GRP_HSV, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_GRP_PICKER, DLSZ_SIZE_Y | DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP_EX(CSpectrumColorPicker)
        MSG_WM_INITDIALOG(OnInitDialog)
        CHAIN_MSG_MAP(ImplResizer)
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
    {
        UNREFERENCED_PARAMETER(wndFocus);
        UNREFERENCED_PARAMETER(lInitParam);

        CRect rc;
        GetClientRect(rc);

        DlgResize_Init(false, true, 0);
        return TRUE;
    }
};

CColorPicker::~CColorPicker() = default;

CColorPicker::CColorPicker()
    : Super{}
    , m_pImpl{std::make_unique<Impl>()}
    , m_bMsgHandled{FALSE}
{
}

HRESULT CColorPicker::PreCreateWindow()
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

CColorPicker& CColorPicker::operator=(HWND hWnd) // make compatible with WTL/DDX
{
    Attach(hWnd);
    return *this;
}

#if 0
struct CDCEx: WTL::CWindowDC
{
    CDCEx(HWND hWnd, WTL::CRgnHandle rgn, DWORD dwFlags)
        : WTL::CWindowDC(nullptr)
    {
        ATLASSERT((hWnd == NULL) || ::IsWindow(hWnd));
        m_hWnd = hWnd;
        m_hDC = ::GetDCEx(hWnd, rgn, dwFlags);
        if (!m_hDC) {
            auto const code = static_cast<HRESULT>(GetLastError());
            ATLTRACE(L"GetDCEx failed! <%d>\n", code);
        }
    }

    ~CDCEx() = default;
};
#endif

BOOL CColorPicker::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL const bSave{m_bMsgHandled};
    BOOL const  bRet{_ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)};
    m_bMsgHandled = bSave;
    return bRet;
}

BOOL CColorPicker::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled = TRUE;
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        //MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

void CColorPicker::OnNcPaint(WTL::CRgnHandle rgn)
{
    CRect rc{};
    if constexpr (false) {
        WTL::CDCHandle dc{GetDCEx(rgn, DCX_INTERSECTRGN | DCX_WINDOW)};
        GetClipBox(dc, rc);
        dc.DrawEdge(rc, EDGE_ETCHED, BF_FLAT | BF_RECT);
        ReleaseDC(dc);
    }
    else {
        WTL::CClientDC dc{m_hWnd};
        GetWindowRect(rc);
        dc.DrawEdge(rc, EDGE_ETCHED, BF_FLAT | BF_RECT);
    }
}

int CColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);
    m_pImpl->Create(m_hWnd);
    return 0;
}

void CColorPicker::OnSize(UINT nType, CSize size)
{
    CRect rc{0, 0, size.cx, size.cy};
    m_pImpl->MoveWindow(rc);
}
