#include "stdafx.h"
#include "CColorPicker.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include "CSliderCtrl.h"
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlcrack.h>
#include <atldlgs.h>
#include <atlddx.h>

struct CColorPicker::Impl: WTL::CIndirectDialogImpl<Impl>,
                           WTL::CDialogResize<Impl>,
                           WTL::CWinDataExchange<Impl>
{
    using   ImplSuper = WTL::CIndirectDialogImpl<Impl>;
    using ImplResizer = WTL::CDialogResize<Impl>;

    ~Impl() override;
    Impl();

    HRESULT PreCreateWindow();

private:
    friend ImplSuper;
    friend ImplResizer;

    enum Sizes: short
    {
        DlgCX = 460,
        DlgCY = 205,
        HDlgCX = DlgCX/2-6,
        HDlg3CY = DlgCY/3-8,
        HHCX = HDlgCX/2-8,
        HHCY = HDlg3CY/2+8,
        HLCY = HDlg3CY/4,
    };

    enum Styles: DWORD
    {
        TB_VERT = TBS_LEFT | TBS_VERT | TBS_NOTICKS | TBS_DOWNISLEFT | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
        CC_CHILD = WS_CHILD | WS_VISIBLE,
    };

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 1726,
        CID_GRP_SPECTRUM,
        CID_SPEC_COMBO,
        CID_SPEC_COLOR_CAP,
        CID_SPEC_COLOR_SEL,
        CID_SPECTRUM_PIC,
        CID_SPECTRUM_SLD,
        CID_GRP_RGB,
        CID_GRP_HSL,
        CID_GRP_HSV,
        CID_GRP_PICKER,
    };

    CSpectrumImage m_imSpectrum;
    CSpectrumSlider  m_imSlider;
    int         m_nSpectrumKind;
    int                m_nSlide;

    BEGIN_CONTROLS_MAP()  //             Text/ID,            ID/ClassName,    Style,             X,             Y,          Width,        Height, Style...
        CONTROL_GROUPBOX(   _T("Spectrum Color"),        CID_GRP_SPECTRUM,                       4,             4,         HDlgCX,       DlgCY-8, 0, 0)
        CONTROL_COMBOBOX(                                  CID_SPEC_COMBO,                      16,            18,         HHCX-8,       DlgCY-8, CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_TABSTOP, 0)
        CONTROL_RTEXT(              _T("Color:"),      CID_SPEC_COLOR_CAP,    HDlgCX-HHCY-HHCX/2-8,            18,       HHCX/2-8,          HLCY, SS_CENTERIMAGE, 0)
        CONTROL_CTEXT(               _T("COLOR"),      CID_SPEC_COLOR_SEL,           HDlgCX-HHCY-8,            18,         HHCY+2,          HHCY, SS_SUNKEN | SS_CENTERIMAGE | SS_BITMAP, 0)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_PIC,          CSPECIMG_CLASS, CC_CHILD,            16,       24+HLCY, HDlgCX-HHCY-32, DlgCY-HLCY-36, 0)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_SLD, _T("msctls_trackbar32"),  TB_VERT, HDlgCX-HHCY-8,       24+HHCY,      HHCX/2-12, DlgCY-HHCY-36, 0)
        CONTROL_GROUPBOX(        _T("RGB Color"),             CID_GRP_RGB,              8+HDlgCX+4,             4,         HDlgCX,       HDlg3CY, 0, 0)
        CONTROL_GROUPBOX(        _T("HSL Color"),             CID_GRP_HSL,              8+HDlgCX+4,   4+HDlg3CY+4,     HDlgCX/2-4,       HDlg3CY, 0, 0)
        CONTROL_GROUPBOX(        _T("HSV Color"),             CID_GRP_HSV,     8+HDlgCX+HDlgCX/2+8,   4+HDlg3CY+4,     HDlgCX/2-4,       HDlg3CY, 0, 0)
        CONTROL_GROUPBOX(     _T("Color Picker"),          CID_GRP_PICKER,              8+HDlgCX+4, 4+HDlg3CY*2+8,         HDlgCX,     HDlg3CY+8, 0, 0)
    END_CONTROLS_MAP()

    BEGIN_DIALOG(0, 0, DlgCX, DlgCY)
        DIALOG_STYLE(WS_CHILD | WS_VISIBLE)
        DIALOG_FONT(8, _T("MS Shell Dlg 2"))
    END_DIALOG()

    BEGIN_DDX_MAP(CSpectrumColorPicker)
        DDX_COMBO_INDEX(CID_SPEC_COMBO, m_nSpectrumKind);
        DDX_INDEX(WTL::CSliderCtrl, CID_SPECTRUM_SLD, m_nSlide);
        if (!bSaveAndValidate) {
            OnDDXChanges(nCtlID);
        }
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(CID_GRP_SPECTRUM, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COMBO, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COLOR_CAP, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COLOR_SEL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_SPECTRUM_PIC, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPECTRUM_SLD, DLSZ_SIZE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_RGB, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_HSL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_HSV, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_PICKER, DLSZ_SIZE_Y | DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP_EX(CSpectrumColorPicker)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_COMMAND(OnCommand)
        CHAIN_MSG_MAP(ImplResizer)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    void OnDDXChanges(int nCtlID);
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnSpecComboChanged();
};

CColorPicker::Impl::~Impl() = default;

CColorPicker::Impl::Impl()
    :    m_imSpectrum{}
    ,      m_imSlider{}
    , m_nSpectrumKind{SPEC_HSV_Hue}
    ,        m_nSlide{0}
    ,   m_bMsgHandled{FALSE}
{
}

HRESULT CColorPicker::Impl::PreCreateWindow()
{
    HRESULT code{S_OK};
    code = m_imSlider.PreCreateWindow();
    if (ERROR_SUCCESS != code) {
        return code;
    }
    code = m_imSpectrum.PreCreateWindow();
    if (ERROR_SUCCESS != code) {
        return code;
    }
    return S_OK;
}

void CColorPicker::Impl::OnDDXChanges(int nCtlID)
{
    switch (nCtlID) {
    case CID_SPEC_COMBO: OnSpecComboChanged(); break;
    }
}

void CColorPicker::Impl::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    UNREFERENCED_PARAMETER(uNotifyCode);
    UNREFERENCED_PARAMETER(nID);
    UNREFERENCED_PARAMETER(wndCtl);
    SetMsgHandled(FALSE);
}

BOOL CColorPicker::Impl::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);

    ATLASSUME(m_imSpectrum.m_hWnd != nullptr);
    m_imSlider.SubclassWindow(GetDlgItem(CID_SPECTRUM_SLD));
    ATLASSUME(m_imSlider.m_hWnd != nullptr);

    WTL::CComboBox cbSpectrum(GetDlgItem(CID_SPEC_COMBO));
    cbSpectrum.AddString(L"RGB/红");
    cbSpectrum.AddString(L"RGB/绿");
    cbSpectrum.AddString(L"RGB/蓝");
    cbSpectrum.AddString(L"HSV/色调");
    cbSpectrum.AddString(L"HSV/饱和度");
    cbSpectrum.AddString(L"HSV/明度");

    m_imSlider.SetPrimaryColor(0xffffff);

    DoDataExchange(FALSE);
    DlgResize_Init(false, true, 0);

    OnSpecComboChanged();
    return TRUE;
}

void CColorPicker::Impl::OnSpecComboChanged()
{
    m_imSpectrum.SetSpectrumKind(static_cast<SpectrumKind>(m_nSpectrumKind));
}

CColorPicker::~CColorPicker() = default;

CColorPicker::CColorPicker()
    :         Super{}
    ,       m_pImpl{std::make_unique<Impl>()}
    , m_bMsgHandled{FALSE}
{
}

HRESULT CColorPicker::PreCreateWindow()
{
    auto const code = m_pImpl->PreCreateWindow();
    if (ERROR_SUCCESS != code) {
        return code;
    }
    return Super::PreCreateWindow();
}

ATOM& CColorPicker::GetWndClassAtomRef()
{
    static ATOM gs_CColorPicker_Atom{0};
    return gs_CColorPicker_Atom;
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
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        //MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        REFLECT_NOTIFICATIONS()
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
    if (!m_pImpl->Create(m_hWnd)) {
        return -1;
    }
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
}

void CColorPicker::OnSize(UINT nType, CSize size)
{
    CRect rc{0, 0, size.cx, size.cy};
    m_pImpl->MoveWindow(rc);
}
