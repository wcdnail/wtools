#include "stdafx.h"
#include "CColorPicker.h"
#include <atldlgs.h>
#include <atlddx.h>

ATOM CSpectrumColorPicker::gs_Atom{0};

struct CSpectrumColorPicker::Impl: WTL::CIndirectDialogImpl<Impl>,
                                   WTL::CDialogResize<Impl>
{
    using   ImplSuper = WTL::CIndirectDialogImpl<Impl>;
    using ImplResizer = WTL::CDialogResize<Impl>;

    enum Sizes: short
    {
        DEFAULT_CX = 230,
        DEFAULT_CY = 205,
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
        IDC_GRP_BEVEL,
    };

    BEGIN_CONTROLS_MAP()
        CONTROL_GROUPBOX(_T("Spectrum color"), IDC_GRP_BEVEL, 4, 4, DEFAULT_CX-8, DEFAULT_CY-8, WS_GROUP, 0)
        //CONTROL_CONTROL(_T(""), IDC_CUSTOM_CTL1, _T("CSpectrumColorPicker"), WS_GROUP, DIALOG_BRD_CX+8, DIALOG_BRD_CY+8, DIALOG_BRD_CX+8+CSpectrumColorPicker::DEFAULT_CX, DIALOG_BRD_CY+8+CSpectrumColorPicker::DEFAULT_CY, 0)
    END_CONTROLS_MAP()

    BEGIN_DDX_MAP(CSpectrumColorPicker)
        //DDX_CONTROL_HANDLE(IDC_CUSTOM_CTL1, m_ccColorPicker)
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(IDC_GRP_BEVEL, DLSZ_SIZE_Y | DLSZ_SIZE_X)
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

CSpectrumColorPicker::~CSpectrumColorPicker() = default;

CSpectrumColorPicker::CSpectrumColorPicker()
    : Super{}
    , m_pImpl{std::make_unique<Impl>()}
    , m_bMsgHandled{FALSE}
{
}

HRESULT CSpectrumColorPicker::PreCreateWindow()
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

CSpectrumColorPicker& CSpectrumColorPicker::operator=(HWND hWnd) // make compatible with WTL/DDX
{
    Attach(hWnd);
    return *this;
}

int CSpectrumColorPicker::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);
    m_pImpl->Create(m_hWnd);
    return 0;
}

void CSpectrumColorPicker::OnSize(UINT nType, CSize size)
{
    CRect rc{0, 0, size.cx, size.cy};
    m_pImpl->MoveWindow(rc);
}
