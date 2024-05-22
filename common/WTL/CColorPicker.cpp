﻿#include "stdafx.h"
#include "CColorPicker.h"
#include "CColorPickerDefs.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include "CSliderCtrl.h"
#include "CAppModuleRef.h"
#include <dev.assistance/dev.assist.h>
#include <scoped.bool.guard.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlframe.h>
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
        RGB_CX  = DLG_CX/3,
        RGB_CY  = DLG_CY/3,
        SPEC_CX = DLG_CX-RGB_CX,
        HHCX    = RGB_CX/2,
        HHCY    = RGB_CY/2,
        HLCY    = RGB_CY/4-4,
    };

    enum Styles: DWORD
    {
        TB_VERT = TBS_NOTHUMB | TBS_RIGHT | TBS_VERT | TBS_NOTICKS | TBS_DOWNISLEFT | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
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
        CID_RGB_RED_CAP,
        CID_RGB_RED_VAL,
        CID_RGB_GRN_CAP,
        CID_RGB_GRN_VAL,
        CID_RGB_BLU_CAP,
        CID_RGB_BLU_VAL,
        CID_GRP_HSV,
        CID_RGB_HUE_CAP,
        CID_RGB_HUE_VAL,
        CID_RGB_SAT_CAP,
        CID_RGB_SAT_VAL,
        CID_RGB_VAL_CAP,
        CID_RGB_VAL_VAL,
        CID_GRP_HSL,
        CID_GRP_PICKER,
    };

    CSpectrumImage m_imSpectrum;
    int         m_nSpectrumKind; // For DDX
    CSpectrumSlider  m_imSlider;
    WTL::CStatic      m_stColor;
    bool            m_bSaveData;

    BEGIN_CONTROLS_MAP()  //             Text/ID,            ID/ClassName,    Style,               X,              Y,           Width,         Height,   Styles
        CONTROL_GROUPBOX(   _T("Spectrum Color"),        CID_GRP_SPECTRUM,                         2,              2,       SPEC_CX-4,       DLG_CY-4,   0, 0)
            CONTROL_COMBOBOX(                              CID_SPEC_COMBO,                         8,             14,        HHCX*2-8,       DLG_CY-4,   CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_TABSTOP, 0)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_PIC,          CSPECIMG_CLASS, CC_CHILD,               8,        18+HLCY, SPEC_CX-HHCY-20, DLG_CY-HLCY-26,   WS_EX_STATICEDGE)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_SLD, _T("msctls_trackbar32"),  TB_VERT, SPEC_CX-HHCY-10,        18+HHCY,          HHCY+2, DLG_CY-HHCY-26,   0)
            CONTROL_RTEXT(          _T("Color:"),      CID_SPEC_COLOR_CAP,    SPEC_CX-HHCY-HHCX/2-10,             14,        HHCX/2-8,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_CTEXT(           _T("COLOR"),      CID_SPEC_COLOR_SEL,           SPEC_CX-HHCY-10,             14,          HHCY+2,         HHCY-2,   SS_SUNKEN | SS_CENTERIMAGE | SS_OWNERDRAW, WS_EX_STATICEDGE)
        CONTROL_GROUPBOX(        _T("RGB Color"),             CID_GRP_RGB,                 SPEC_CX+4,              2,        RGB_CX-8,       RGB_CY-4,   0, 0)
            CONTROL_RTEXT(               _T("R"),         CID_RGB_RED_CAP,               0+SPEC_CX+6,  (HLCY+3)*0+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_RED_VAL,          SPEC_CX+0+HHCX/3,  (HLCY+3)*0+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | ES_WANTRETURN, 0)
            CONTROL_RTEXT(               _T("G"),         CID_RGB_GRN_CAP,               0+SPEC_CX+6,  (HLCY+3)*1+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_GRN_VAL,          SPEC_CX+0+HHCX/3,  (HLCY+3)*1+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | ES_WANTRETURN, 0)
            CONTROL_RTEXT(               _T("B"),         CID_RGB_BLU_CAP,               0+SPEC_CX+6,  (HLCY+3)*2+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_BLU_VAL,          SPEC_CX+0+HHCX/3,  (HLCY+3)*2+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | ES_WANTRETURN, 0)
        CONTROL_GROUPBOX(        _T("HSV Color"),             CID_GRP_HSL,                 SPEC_CX+4,     0+RGB_CY+2,      RGB_CX/2-6,       RGB_CY-4,   0, 0)
            CONTROL_RTEXT(               _T("H"),         CID_RGB_HUE_CAP,               0+SPEC_CX+6,  (HLCY+3)*4+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_HUE_VAL,          SPEC_CX+0+HHCX/3,  (HLCY+3)*4+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | ES_WANTRETURN, 0)
            CONTROL_RTEXT(               _T("S"),         CID_RGB_SAT_CAP,               0+SPEC_CX+6,  (HLCY+3)*5+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_SAT_VAL,          SPEC_CX+0+HHCX/3,  (HLCY+3)*5+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | ES_WANTRETURN, 0)
            CONTROL_RTEXT(               _T("V"),         CID_RGB_VAL_CAP,               0+SPEC_CX+6,  (HLCY+3)*6+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_VAL_VAL,          SPEC_CX+0+HHCX/3,  (HLCY+3)*6+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | ES_WANTRETURN, 0)
        CONTROL_GROUPBOX(        _T("HSL Color"),             CID_GRP_HSV,        SPEC_CX+2+RGB_CX/2,     0+RGB_CY+2,      RGB_CX/2-6,       RGB_CY-4,   0, 0)
        CONTROL_GROUPBOX(     _T("Color Picker"),          CID_GRP_PICKER,               8+SPEC_CX+4,   4+RGB_CY*2+8,         SPEC_CX,       RGB_CY+8,   0, 0)
    END_CONTROLS_MAP()

    BEGIN_DIALOG(0, 0, DLG_CX, DLG_CY)
        DIALOG_STYLE(WS_CHILD | WS_VISIBLE)
        DIALOG_FONT(8, _T("MS Shell Dlg 2"))
    END_DIALOG()

    BEGIN_DDX_MAP(CSpectrumColorPicker)
        DDX_UINT_RANGE(CID_RGB_RED_VAL, m_imSpectrum.GetColorUnion().GetRed(), 0, RGB_MAX_INT);
        DDX_UINT_RANGE(CID_RGB_GRN_VAL, m_imSpectrum.GetColorUnion().GetGreen(), 0, RGB_MAX_INT);
        DDX_UINT_RANGE(CID_RGB_BLU_VAL, m_imSpectrum.GetColorUnion().GetBlue(), 0, RGB_MAX_INT);
        DDX_FLOAT_RANGE(CID_RGB_HUE_VAL, m_imSpectrum.GetColorUnion().m_dH, 0, HSV_HUE_MAX);
        DDX_FLOAT_RANGE(CID_RGB_SAT_VAL, m_imSpectrum.GetColorUnion().m_dS, 0, HSV_SAT_MAX);
        DDX_FLOAT_RANGE(CID_RGB_VAL_VAL, m_imSpectrum.GetColorUnion().m_dV, 0, HSV_VAL_MAX);
        DDX_COMBO_INDEX(CID_SPEC_COMBO, m_nSpectrumKind);
        OnDDXChanges(nCtlID, bSaveAndValidate);
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(CID_GRP_SPECTRUM, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COMBO, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COLOR_CAP, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COLOR_SEL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_SPECTRUM_PIC, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPECTRUM_SLD, DLSZ_SIZE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_RGB, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_RED_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_RED_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_GRN_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_GRN_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_BLU_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_BLU_VAL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_HSV, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_HUE_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_HUE_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_SAT_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_SAT_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_VAL_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_VAL_VAL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_HSL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_PICKER, DLSZ_SIZE_Y | DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP(CColorPicker::Impl)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_HSCROLL(OnWMScroll)
        MSG_WM_VSCROLL(OnWMScroll)
        CHAIN_MSG_MAP(ImplResizer)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    void SpectruKindChanged();
    LRESULT SliderChanged(LPNMHDR pnmh);
    LRESULT ColorChanged(bool bNotify);
    void OnDDXChanges(UINT nID, BOOL bSaveAndValidate);
    void OnDataValidateError(UINT nCtrlID, BOOL bSave, _XData& data);
    LRESULT OnNotify(int nID, LPNMHDR pnmh);
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI);
    void OnWMScroll(UINT nSBCode, UINT nPos, WTL::CScrollBar ctlScrollBar);
};

CColorPicker::Impl::~Impl() = default;

CColorPicker::Impl::Impl()
    :    m_imSpectrum{0xffffff, SPEC_HSV_Hue}
    , m_nSpectrumKind{m_imSpectrum.GetSpectrumKind()}
    ,      m_imSlider{}
    ,       m_stColor{}
    ,   m_bSaveData{false}
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

void CColorPicker::Impl::OnDDXChanges(UINT nID, BOOL bSaveAndValidate)
{
    if (m_bSaveData || !bSaveAndValidate) {
        return;
    }
    switch (nID) {
    case CID_SPEC_COMBO:    SpectruKindChanged(); break;
    case CID_RGB_RED_VAL:
    case CID_RGB_GRN_VAL:
    case CID_RGB_BLU_VAL:  ColorChanged(false); break;
    }
}

void CColorPicker::Impl::OnDataValidateError(UINT nCtrlID, BOOL bSave, _XData& data)
{
    switch (nCtrlID) {
    case CID_RGB_RED_VAL:
    case CID_RGB_GRN_VAL:
    case CID_RGB_BLU_VAL:{
        if (bSave) {
            CScopedBoolGuard bGuard{m_bSaveData};
            SetDlgItemInt(nCtrlID, data.intData.nMax);
        }
        break;
    }
    }
}

LRESULT CColorPicker::Impl::SliderChanged(LPNMHDR pnmh)
{
    auto const* ptPosCh = reinterpret_cast<NMTRBTHUMBPOSCHANGING const*>(pnmh);
    if (!ptPosCh) {
        return 0;
    }
    m_imSpectrum.OnSliderChanged(ptPosCh->dwPos);
    m_stColor.InvalidateRect(nullptr, FALSE);
    return 0;
}

LRESULT CColorPicker::Impl::ColorChanged(bool bNotify)
{
    if (SPEC_HSV_Hue != m_imSpectrum.GetSpectrumKind()) {
        m_imSlider.UpdateRaster(m_imSpectrum.GetSpectrumKind(), m_imSpectrum.GetColorUnion());
    }
    m_stColor.InvalidateRect(nullptr, FALSE);
    if (bNotify) {
        CScopedBoolGuard bGuard{m_bSaveData};
        DoDataExchange(DDX_LOAD, CID_RGB_RED_VAL);
        DoDataExchange(DDX_LOAD, CID_RGB_GRN_VAL);
        DoDataExchange(DDX_LOAD, CID_RGB_BLU_VAL);
        DoDataExchange(DDX_LOAD, CID_RGB_HUE_VAL);
        DoDataExchange(DDX_LOAD, CID_RGB_SAT_VAL);
        DoDataExchange(DDX_LOAD, CID_RGB_VAL_VAL);
    }
    else {
        m_imSpectrum.InvalidateRect(nullptr, FALSE);
    }
    return 0;
}

LRESULT CColorPicker::Impl::OnNotify(int nID, LPNMHDR pnmh)
{
    if (NM_CUSTOMDRAW == pnmh->code) {
        if constexpr (false) {
            LPNMCUSTOMDRAW nmcd{reinterpret_cast<LPNMCUSTOMDRAW>(pnmh)};
            DBGTPrint(LTH_WM_NOTIFY L" id:%-4d nc:%-4d %s >> %05x\n", nID,
                pnmh->code, DH::WM_NC_C2SW(pnmh->code), nmcd->dwDrawStage);
        }
        SetMsgHandled(FALSE);
        return 0;
    }
    if (m_bSaveData) {
        return 0;
    }
    switch (pnmh->code) {
    case TRBN_THUMBPOSCHANGING:
        switch (nID) {
        case CID_SPECTRUM_SLD: return SliderChanged(pnmh);
        }
        break;
    case NM_SPECTRUM_CLR_SEL:
        switch (nID) {
        case CID_SPECTRUM_PIC: return ColorChanged(true);
        }
        break;
    }
    DBGTPrint(LTH_WM_NOTIFY L" id:%-4d nc:%-4d %s\n", nID, pnmh->code, DH::WM_NC_C2SW(pnmh->code));
    SetMsgHandled(FALSE);
    return 0;
}

void CColorPicker::Impl::OnCommand(UINT uNotifyCode, int nID, CWindow /*wndCtl*/)
{
    if (m_bSaveData) {
        return ;
    }
    switch (uNotifyCode) {
    case BN_UNHILITE:
    case BN_PAINT:
    case BN_DISABLE:
    case BN_KILLFOCUS:
        break;
    case EN_UPDATE:
        if (DoDataExchange(DDX_SAVE, nID)) {
            m_imSpectrum.GetColorUnion().SetUpdated(true);
            m_imSpectrum.InvalidateRect(nullptr, FALSE);
        }
        return ;
    case CBN_SELENDOK:
        DoDataExchange(DDX_SAVE, nID);
        return ;
    case CBN_SELENDCANCEL:
    case CBN_CLOSEUP:
    case EN_CHANGE:
    case EN_SETFOCUS:
    case EN_KILLFOCUS:
    case EN_MAXTEXT:
        return ;
    default:
        DBGTPrint(LTH_WM_COMMAND L" id:%-4d nc:%-4d %s\n", nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
        break;
    }
    SetMsgHandled(FALSE);
}

BOOL CColorPicker::Impl::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);

    m_stColor.Attach(GetDlgItem(CID_SPEC_COLOR_SEL));

    ATLASSUME(m_imSpectrum.m_hWnd != nullptr);
    m_imSlider.SubclassWindow(GetDlgItem(CID_SPECTRUM_SLD));
    ATLASSUME(m_imSlider.m_hWnd != nullptr);

    m_imSlider.Initialize();
    m_imSpectrum.Initialize();

    WTL::CComboBox cbSpectrum(GetDlgItem(CID_SPEC_COMBO));
    cbSpectrum.AddString(L"RGB/红");
    cbSpectrum.AddString(L"RGB/绿");
    cbSpectrum.AddString(L"RGB/蓝");
    cbSpectrum.AddString(L"HSV/色调");
    cbSpectrum.AddString(L"HSV/饱和度");
    cbSpectrum.AddString(L"HSV/明度");

    {
        CScopedBoolGuard bGuard{m_bSaveData};
        DoDataExchange(DDX_LOAD);
        SpectruKindChanged();
    }

    DlgResize_Init(false, true, 0);
    return TRUE;
}

void CColorPicker::Impl::OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI)
{
    switch (nID) {
    case CID_SPEC_COLOR_SEL:{
        WTL::CDCHandle dc{pDI->hDC};
        CRect const    rc{pDI->rcItem};
        dc.FillSolidRect(rc, m_imSpectrum.GetColorRef());
        return;
    }
    }
    SetMsgHandled(FALSE);
    DBGTPrint(LTH_WM_DRAWITEM L" id:%-4d ct:%-4d\n", nID, pDI->CtlType);
}

void CColorPicker::Impl::SpectruKindChanged()
{
    m_imSpectrum.SetSpectrumKind(static_cast<SpectrumKind>(m_nSpectrumKind));
    m_imSlider.UpdateRaster(m_imSpectrum.GetSpectrumKind(), m_imSpectrum.GetColorUnion());
    m_imSpectrum.OnSliderChanged(m_imSlider.GetPos());
}

void CColorPicker::Impl::OnWMScroll(UINT nSBCode, UINT nPos, WTL::CScrollBar ctlScrollBar)
{
    if (ctlScrollBar.m_hWnd == m_imSlider.m_hWnd) {
        m_imSpectrum.OnSliderChanged(nPos);
        m_stColor.InvalidateRect(nullptr, FALSE);
    }
}

CColorPicker::~CColorPicker() = default;

CColorPicker::CColorPicker()
    :   Super{}
    , m_pImpl{std::make_unique<Impl>()}
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

BOOL CColorPicker::PreTranslateMessage(MSG* pMsg)
{
    if (m_pImpl->IsDialogMessageW(pMsg)) {
        return TRUE;
    }
    return FALSE;
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
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(bHandled);
    switch(dwMsgMapID) { 
    case 0:
      //MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
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
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
    }
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
}

void CColorPicker::OnDestroy()
{
    SetMsgHandled(FALSE);
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
    }
}

void CColorPicker::OnSize(UINT nType, CSize size) const
{
    if constexpr (true) {
        CRect rc{0, 0, size.cx, size.cy};
        m_pImpl->MoveWindow(rc);
    }
}
