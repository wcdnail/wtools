﻿#include "stdafx.h"
#include "CColorPicker.h"
#include "CColorPickerDefs.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
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
    friend ImplResizer;
    friend ImplSuper;

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 1905,
        CID_GRP_SPECTRUM,
        CID_SPEC_COMBO,
        CID_SPEC_COLOR_CAP, CID_SPEC_COLOR_SEL,
        CID_SPECTRUM_PIC,
        CID_SPECTRUM_SLD,
        CID_GRP_RGB,
        CID_RGB_RED_CAP, CID_RGB_RED_VAL, CID_RGB_RED_UDS,
        CID_RGB_GRN_CAP, CID_RGB_GRN_VAL, CID_RGB_GRN_UDS,
        CID_RGB_BLU_CAP, CID_RGB_BLU_VAL, CID_RGB_BLU_UDS,
        CID_RGB_HEX_VAL,
        CID_RGB_HTM_VAL,
        CID_RGB_ALP_CAP, CID_RGB_ALP_VAL, CID_RGB_ALP_UDS,
        CID_GRP_HSL,
        CID_HSL_HUE_CAP, CID_HSL_HUE_VAL, CID_HSL_HUE_UDS,
        CID_HSL_SAT_CAP, CID_HSL_SAT_VAL, CID_HSL_SAT_UDS,
        CID_HSL_LTN_CAP, CID_HSL_LTN_VAL, CID_HSL_LTN_UDS,
        CID_GRP_HSV,
        CID_HSV_HUE_CAP, CID_HSV_HUE_VAL, CID_HSV_HUE_UDS,
        CID_HSV_SAT_CAP, CID_HSV_SAT_VAL, CID_HSV_SAT_UDS,
        CID_HSV_VAL_CAP, CID_HSV_VAL_VAL, CID_HSV_VAL_UDS,
        CID_GRP_PICKER,
        CONTROL_COUNT = CID_GRP_PICKER - BEFORE_FIRST_CONTROL_ID,
    };

    enum Sizes: short
    {
        TRGB_CX = DLG_CX/4,
        RGB_CX  = TRGB_CX + TRGB_CX/2,
        RGB_CY  = DLG_CY/3,
        SPEC_CX = DLG_CX - RGB_CX,
        HHCX    = RGB_CX/2,
        HHCY    = RGB_CY/2,
        HLCY    = RGB_CY/4 - 4,
        CLMNT_X = SPEC_CX + 4,                  // 1 column static X
        CLMN1_X = SPEC_CX + HHCX/3,             // 1 column edit X
        CLMN2_X = SPEC_CX-2+HHCX/3+RGB_CX/2,    // 2 column edit X
        HLCYS   = HLCY + 3,                     // edit Y spacing
    };

    enum Styles: DWORD
    {
        CC_CHILD = WS_CHILD | WS_VISIBLE,
        UD_CHILD = UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_GROUP,
    };

    bool            m_bSaveData;
    CSpectrumImage m_imSpectrum;
    int         m_nSpectrumKind; // For DDX
    ATL::CString    m_sColorHex;
    ATL::CString   m_sColorHtml;
    CSpectrumSlider  m_imSlider;
    WTL::CStatic      m_stColor;
    WTL::CFont       m_fntFixed;

    BEGIN_CONTROLS_MAP()  //             Text/ID,            ID/ClassName,    Style,                 X,              Y,           Width,         Height,   Styles
        CONTROL_GROUPBOX(         _T("Spectrum"),        CID_GRP_SPECTRUM,                           2,              2,       SPEC_CX-4,       DLG_CY-4,   0, 0)
            CONTROL_COMBOBOX(                              CID_SPEC_COMBO,                           8,             14,        HHCX*2-8,       DLG_CY-4,   CBS_AUTOHSCROLL | CBS_DROPDOWNLIST, 0)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_PIC,          CSPECIMG_CLASS,   CC_CHILD,               8,        18+HLCY, SPEC_CX-HHCY-20, DLG_CY-HLCY-26,   WS_EX_STATICEDGE)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_SLD,          CSPECSLD_CLASS,   CC_CHILD, SPEC_CX-HHCY-10,        18+HHCY,          HHCY+2, DLG_CY-HHCY-26,   WS_EX_STATICEDGE)
            CONTROL_RTEXT(          _T("Color:"),      CID_SPEC_COLOR_CAP,      SPEC_CX-HHCY-HHCX/2-10,             14,        HHCX/2-8,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_CTEXT(           _T("COLOR"),      CID_SPEC_COLOR_SEL,             SPEC_CX-HHCY-10,             14,          HHCY+2,         HHCY-2,   SS_CENTERIMAGE | SS_OWNERDRAW, WS_EX_STATICEDGE)
        CONTROL_GROUPBOX(              _T("RGB"),             CID_GRP_RGB,                     CLMNT_X,              2,        RGB_CX-8,       RGB_CY-4,   0, 0)
            CONTROL_RTEXT(               _T("R"),         CID_RGB_RED_CAP,                   CLMNT_X+2,     HLCYS*0+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_RTEXT(               _T("G"),         CID_RGB_GRN_CAP,                   CLMNT_X+2,     HLCYS*1+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_RTEXT(               _T("B"),         CID_RGB_BLU_CAP,                   CLMNT_X+2,     HLCYS*2+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_RED_VAL,                     CLMN1_X,     HLCYS*0+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_RGB_RED_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN1_X+24,     HLCYS*0+13,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_RGB_GRN_VAL,                     CLMN1_X,     HLCYS*1+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_RGB_GRN_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN1_X+24,     HLCYS*1+13,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_RGB_BLU_VAL,                     CLMN1_X,     HLCYS*2+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_RGB_BLU_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN1_X+24,     HLCYS*2+13,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_RGB_HEX_VAL,          SPEC_CX-2+RGB_CX/2,     HLCYS*0+13,         HHCX-10,           HLCY,   ES_CENTER, 0)
            CONTROL_EDITTEXT(                             CID_RGB_HTM_VAL,          SPEC_CX-2+RGB_CX/2,     HLCYS*1+13,         HHCX-10,           HLCY,   ES_CENTER, 0)
            CONTROL_RTEXT(               _T("A"),         CID_RGB_ALP_CAP,          SPEC_CX+3+RGB_CX/2,     HLCYS*2+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_ALP_VAL,                     CLMN2_X,     HLCYS*2+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_RGB_ALP_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN2_X+24,     HLCYS*4+16,       HHCX/2-22,           HLCY,   0)
        CONTROL_GROUPBOX(              _T("HSL"),             CID_GRP_HSL,                     CLMNT_X,       RGB_CY+2,      RGB_CX/2-6,       RGB_CY-4,   0, 0)
            CONTROL_RTEXT(               _T("H"),         CID_HSL_HUE_CAP,                   CLMNT_X+2,     HLCYS*4+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_RTEXT(               _T("S"),         CID_HSL_SAT_CAP,                   CLMNT_X+2,     HLCYS*5+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_RTEXT(               _T("L"),         CID_HSL_LTN_CAP,                   CLMNT_X+2,     HLCYS*6+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_HSL_HUE_VAL,                     CLMN1_X,     HLCYS*4+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_HSL_HUE_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN1_X+24,     HLCYS*4+16,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_HSL_SAT_VAL,                     CLMN1_X,     HLCYS*5+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_HSL_SAT_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN1_X+24,     HLCYS*5+16,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_HSL_LTN_VAL,                     CLMN1_X,     HLCYS*6+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_HSL_LTN_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN1_X+24,     HLCYS*6+16,       HHCX/2-22,           HLCY,   0)
        CONTROL_GROUPBOX(              _T("HSV"),             CID_GRP_HSV,          SPEC_CX+2+RGB_CX/2,       RGB_CY+2,      RGB_CX/2-6,       RGB_CY-4,   0, 0)
            CONTROL_RTEXT(               _T("H"),         CID_HSV_HUE_CAP,          SPEC_CX+3+RGB_CX/2,     HLCYS*4+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_RTEXT(               _T("S"),         CID_HSV_SAT_CAP,          SPEC_CX+3+RGB_CX/2,     HLCYS*5+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_RTEXT(               _T("V"),         CID_HSV_VAL_CAP,          SPEC_CX+3+RGB_CX/2,     HLCYS*6+16,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_HSV_HUE_VAL,                     CLMN2_X,     HLCYS*4+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_HSV_HUE_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN2_X+24,     HLCYS*4+16,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_HSV_SAT_VAL,                     CLMN2_X,     HLCYS*5+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_HSV_SAT_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN2_X+24,     HLCYS*5+16,       HHCX/2-22,           HLCY,   0)
            CONTROL_EDITTEXT(                             CID_HSV_VAL_VAL,                     CLMN2_X,     HLCYS*6+16,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_HSV_VAL_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN2_X+24,     HLCYS*6+16,       HHCX/2-22,           HLCY,   0)
        CONTROL_GROUPBOX(           _T("Picker"),          CID_GRP_PICKER,                     CLMNT_X,     2+RGB_CY*2,        RGB_CX-8,       RGB_CY-4,   0, 0)
    END_CONTROLS_MAP()

    BEGIN_DIALOG(0, 0, DLG_CX, DLG_CY)
        DIALOG_STYLE(WS_CHILD | WS_VISIBLE)
        DIALOG_FONT(8, _T("MS Shell Dlg 2"))
    END_DIALOG()

    BEGIN_DDX_MAP(CSpectrumColorPicker)
        DDX_CONTROL_HANDLE(CID_SPEC_COLOR_SEL, m_stColor)
        DDX_UINT(CID_RGB_RED_VAL, m_imSpectrum.GetColor().m_R);
        DDX_UINT(CID_RGB_GRN_VAL, m_imSpectrum.GetColor().m_G);
        DDX_UINT(CID_RGB_BLU_VAL, m_imSpectrum.GetColor().m_B);
        DDX_UINT(CID_RGB_ALP_VAL, m_imSpectrum.GetColor().m_A);
        DDX_UINT(CID_HSV_HUE_VAL, m_imSpectrum.GetColor().m_H);
        DDX_UINT(CID_HSV_SAT_VAL, m_imSpectrum.GetColor().m_S);
        DDX_UINT(CID_HSV_VAL_VAL, m_imSpectrum.GetColor().m_V);
        DDX_UINT(CID_HSL_HUE_VAL, m_imSpectrum.GetColor().m_Hl);
        DDX_UINT(CID_HSL_SAT_VAL, m_imSpectrum.GetColor().m_Sl);
        DDX_UINT(CID_HSL_LTN_VAL, m_imSpectrum.GetColor().m_L);
        DDX_TEXT(CID_RGB_HEX_VAL, m_sColorHex);
        DDX_TEXT(CID_RGB_HTM_VAL, m_sColorHtml);
        DDX_COMBO_INDEX(CID_SPEC_COMBO, m_nSpectrumKind);
        if (static_cast<UINT>(-1) != nCtlID) {
            OnDDXChanges(nCtlID, bSaveAndValidate);
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
            // Red edit
            DLGRESIZE_CONTROL(CID_RGB_RED_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_RED_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_RED_UDS, DLSZ_MOVE_X)
            // Green edit
            DLGRESIZE_CONTROL(CID_RGB_GRN_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_GRN_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_GRN_UDS, DLSZ_MOVE_X)
            // Blue edit
            DLGRESIZE_CONTROL(CID_RGB_BLU_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_BLU_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_BLU_UDS, DLSZ_MOVE_X)
            // Strings
            DLGRESIZE_CONTROL(CID_RGB_HEX_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_HTM_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_ALP_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_ALP_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_RGB_ALP_UDS, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_HSL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_HUE_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_HUE_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_HUE_UDS, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_SAT_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_SAT_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_SAT_UDS, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_LTN_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_LTN_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSL_LTN_UDS, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_HSV, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_HUE_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_HUE_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_HUE_UDS, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_SAT_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_SAT_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_SAT_UDS, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_VAL_CAP, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_VAL_VAL, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(CID_HSV_VAL_UDS, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_GRP_PICKER, DLSZ_SIZE_Y | DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP(CColorPicker::Impl)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_DRAWITEM(OnDrawItem)
        CHAIN_MSG_MAP(ImplResizer)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    void SpectruKindChanged();
    LRESULT SliderChanged(bool bNotify);
    void UpdateDDX();
    LRESULT ColorChanged(bool bNotify);
    void OnDDXChanges(UINT nID, BOOL bSaveAndValidate);
    LRESULT OnNotify(int nID, LPNMHDR pnmh);
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI);
};

CColorPicker::Impl::~Impl() = default;

CColorPicker::Impl::Impl()
    :     m_bSaveData{true}
    ,    m_imSpectrum{0xffffff, SPEC_HSV_Hue}
    , m_nSpectrumKind{m_imSpectrum.GetSpectrumKind()}
    ,      m_imSlider{m_imSpectrum.GetSpectrumKindRef(), m_imSpectrum.GetColor()}
    ,       m_stColor{}
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
    case CID_RGB_RED_VAL:
    case CID_RGB_GRN_VAL:
    case CID_RGB_BLU_VAL:
        ColorChanged(false);
        m_imSpectrum.GetColor().RGBtoHSV();
        break;
    case CID_HSV_HUE_VAL:
    case CID_HSV_SAT_VAL:
    case CID_HSV_VAL_VAL:
        ColorChanged(false);
        m_imSpectrum.GetColor().HSVtoRGB();
        break;
    case CID_HSL_HUE_VAL:
    case CID_HSL_SAT_VAL:
    case CID_HSL_LTN_VAL:
        ColorChanged(false);
        m_imSpectrum.GetColor().FromHSL();
        break;
    case CID_SPEC_COMBO:
        SpectruKindChanged();
        break;
    default: 
        break;
    }
}

#if 0
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
#endif


void CColorPicker::Impl::UpdateDDX()
{
    auto const& Color{m_imSpectrum.GetColor()};
    m_sColorHex.Format(TEXT("0x%02x%02x%02x"), Color.GetBlue(), Color.GetGreen(), Color.GetRed());
    m_sColorHtml.Format(TEXT("#%02X%02X%02X"), Color.GetRed(), Color.GetGreen(), Color.GetBlue());
    DoDataExchange(DDX_LOAD);
}

LRESULT CColorPicker::Impl::ColorChanged(bool bNotify)
{
    if (SPEC_HSV_Hue != m_imSpectrum.GetSpectrumKind()) {
        m_imSlider.UpdateRaster();
    }
    if (bNotify) {
        CScopedBoolGuard bGuard{m_bSaveData};
        UpdateDDX();
    }
    else {
        m_imSpectrum.InvalidateRect(nullptr, FALSE);
    }
    m_imSlider.InvalidateRect(nullptr, FALSE);
    m_stColor.InvalidateRect(nullptr, FALSE);
    return 0;
}

LRESULT CColorPicker::Impl::SliderChanged(bool bNotify)
{
    if (bNotify) {
        CScopedBoolGuard bGuard{m_bSaveData};
        UpdateDDX();
    }
    m_imSpectrum.InvalidateRect(nullptr, FALSE);
    m_stColor.InvalidateRect(nullptr, FALSE);
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
    case TBN_SAVE:            // skip
        return 0;
    case NM_SLIDER_CLR_SEL:
    case NM_SPECTRUM_CLR_SEL:
        switch (nID) {
        case CID_SPECTRUM_PIC: return ColorChanged(true);
        case CID_SPECTRUM_SLD: return SliderChanged(true);
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
            m_imSpectrum.InvalidateRect(nullptr, FALSE);
            m_imSpectrum.NotifySend();
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

struct UDSpinnerConf
{
    UINT nID;
    int nMin;
    int nMax;
};

struct CUFontConf
{
    UINT    nID;
    HFONT hFont;
};

BOOL CColorPicker::Impl::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);
    ATLASSUME(m_imSpectrum.m_hWnd != nullptr);
    ATLASSUME(m_imSlider.m_hWnd != nullptr);

    m_fntFixed = CreateFontW(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Courier");

    WTL::CComboBox cbSpectrum(GetDlgItem(CID_SPEC_COMBO));
    cbSpectrum.AddString(L"RGB/红");
    cbSpectrum.AddString(L"RGB/绿");
    cbSpectrum.AddString(L"RGB/蓝");
    cbSpectrum.AddString(L"HSV/色调");
    cbSpectrum.AddString(L"HSV/饱和度");
    cbSpectrum.AddString(L"HSV/明度");

    constexpr UDSpinnerConf udSpinnerConf[] = {
        { CID_RGB_RED_UDS, 0, RGB_MAX_INT },
        { CID_RGB_GRN_UDS, 0, RGB_MAX_INT },
        { CID_RGB_BLU_UDS, 0, RGB_MAX_INT },
        { CID_HSV_HUE_UDS, 0, HSV_HUE_MAX_INT },
        { CID_HSV_SAT_UDS, 0, HSV_100PERC_INT },
        { CID_HSV_VAL_UDS, 0, HSV_100PERC_INT },
        { CID_HSL_HUE_UDS, 0, HSV_HUE_MAX_INT },
        { CID_HSL_SAT_UDS, 0, HSV_100PERC_INT },
        { CID_HSL_LTN_UDS, 0, HSV_100PERC_INT },
    };
    for (auto const& spConf: udSpinnerConf) {
        WTL::CUpDownCtrl ctlSpinner{GetDlgItem(spConf.nID)};
        ctlSpinner.SetRange(spConf.nMin, spConf.nMax);
    }

    static const CUFontConf cuFontConf[] = {
        { CID_RGB_HEX_VAL, m_fntFixed },
        { CID_RGB_HTM_VAL, m_fntFixed },
    };
    for (auto const& fntConf: cuFontConf) {
        ATL::CWindow ctl{GetDlgItem(fntConf.nID)};
        ctl.SetFont(fntConf.hFont, FALSE);
    }

    m_imSlider.Initialize();
    m_imSpectrum.Initialize();
    UpdateDDX();
    SpectruKindChanged();
    DlgResize_Init(false, true, 0);
    m_bSaveData = false;
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
    m_imSlider.UpdateRaster();
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
