﻿#include "stdafx.h"
#include "CColorPicker.h"
#include "CColorPickerDefs.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include "CColorStatic.h"
#include "CMagnifierCtrl.h"
#include "CMagnifierInit.h"
#include "CAppModuleRef.h"
#include <dev.assistance/dev.assist.h>
#include <string.utils.error.code.h>
#include <scoped.bool.guard.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlddx.h>
#include <atlmisc.h>
#include <Magnification.h>
#include <algorithm>
#include <memory>
#include <deque>
#include <cctype>

constexpr float MAG_FACTOR{5.f};

struct CColorPicker::Impl: WTL::CIndirectDialogImpl<Impl>,
                           WTL::CDialogResize<Impl>,
                           WTL::CWinDataExchange<Impl>
{
    struct HistoryItem
    {
        COLORREF crColor;
        int       nAlpha;

        HistoryItem(COLORREF clr, int a);
        ~HistoryItem();

        HDC GetDC(HDC dc, CRect const& rc);

    private:
        HBITMAP       m_hPrev;
        WTL::CDC         m_DC;
        WTL::CBitmap m_Bitmap;
    };

    DELETE_COPY_MOVE_OF(Impl);

    using   ImplSuper = WTL::CIndirectDialogImpl<Impl>;
    using ImplResizer = WTL::CDialogResize<Impl>;

    ~Impl() override;
    Impl();

    HRESULT PreCreateWindow();

private:
    friend ImplResizer;
    friend ImplSuper;

    using HistoryCont = std::deque<HistoryItem>;

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 1905,
        CID_GRP_SPECTRUM,
        CID_SPEC_COMBO,
        CID_BTN_PICK_COLOR,
        CID_SPEC_COLOR_CAP, CID_SPEC_COLOR_SEL,
        CID_SPECTRUM_PIC,
        CID_SPECTRUM_SLD,
        CID_GRP_RGB,
        CID_RGB_RED_CAP,
        CID_RGB_GRN_CAP,
        CID_RGB_BLU_CAP,
        CID_RGB_ALP_CAP,
        CID_GRP_HSL,
        CID_HSL_HUE_CAP,
        CID_HSL_SAT_CAP,
        CID_HSL_LTN_CAP,
        CID_GRP_HSV,
        CID_HSV_HUE_CAP,
        CID_HSV_SAT_CAP,
        CID_HSV_VAL_CAP,
        CID_GRP_HISTORY, CID_STA_HISTORY,
        CID_GRP_MAGNIFIER, CID_PLC_MAGNIFIER,
        // Edit controls
        CID_RGB_RED_VAL, CID_RGB_RED_UDS,
        CID_RGB_GRN_VAL, CID_RGB_GRN_UDS,
        CID_RGB_BLU_VAL, CID_RGB_BLU_UDS,
        CID_RGB_ALP_VAL, CID_RGB_ALP_UDS,
        CID_HSL_HUE_VAL, CID_HSL_HUE_UDS,
        CID_HSL_SAT_VAL, CID_HSL_SAT_UDS,
        CID_HSL_LTN_VAL, CID_HSL_LTN_UDS,
        CID_HSV_HUE_VAL, CID_HSV_HUE_UDS,
        CID_HSV_SAT_VAL, CID_HSV_SAT_UDS,
        CID_HSV_VAL_VAL, CID_HSV_VAL_UDS,
        CID_RGB_HEX_VAL, CID_HSV_HEX_UDS,
        CID_RGB_HTM_VAL, CID_HSV_HTM_UDS,
        CID_EDITS_LAST,
        CID_EDITS_FIRST = CID_RGB_RED_VAL,
        CID_EDITS_COUNT = CID_EDITS_LAST - CID_EDITS_FIRST,
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
        CC_CHILD  = WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        UD_CHILD  = UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | WS_GROUP,
        MAG_CHILD = WS_CHILD | WS_VISIBLE | MS_SHOWMAGNIFIEDCURSOR,
    };

    enum Timers: int
    {
        TIMER_COLORPICK      = 1024,
        TIMER_COLORPICK_MSEC = 5,
    };

    bool            m_bSaveData;
    CSpectrumImage m_imSpectrum;
    CSpectrumSlider  m_imSlider;
    CColorStatic      m_stColor;
    CMagnifierCtrl  m_Magnifier;
    HistoryCont    m_deqHistory;
    int         m_nSpectrumKind; // For DDX
    ATL::CString    m_sColorHex;
    ATL::CString   m_sColorHtml;
    WTL::CFont         m_fntHex;
    WTL::CCursor     m_curArrow;
    WTL::CCursor    m_curPicker;
    WTL::CCursor     m_curCross;

    BEGIN_CONTROLS_MAP()  //             Text/ID,            ID/ClassName,    Style,                 X,              Y,           Width,         Height,   Styles
        CONTROL_GROUPBOX(         _T("Spectrum"),        CID_GRP_SPECTRUM,                           2,              2,       SPEC_CX-4,       DLG_CY-4,   0, 0)
            CONTROL_COMBOBOX(                              CID_SPEC_COMBO,                           8,             14,         HHCX+20,       DLG_CY-4,   CBS_AUTOHSCROLL | CBS_DROPDOWNLIST, 0)
          CONTROL_PUSHBUTTON(             _T(""),      CID_BTN_PICK_COLOR,           SPEC_CX-HHCY-HHCX,             14,        HHCX/4+4,         HLCY+2,   BS_BITMAP, 0)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_PIC,          CSPECIMG_CLASS,   CC_CHILD,               8,        18+HLCY, SPEC_CX-HHCY-20, DLG_CY-HLCY-26,   WS_EX_STATICEDGE)
        CONTROL_CONTROL(_T(""), CID_SPECTRUM_SLD,          CSPECSLD_CLASS,   CC_CHILD, SPEC_CX-HHCY-10,        18+HHCY,          HHCY+2, DLG_CY-HHCY-26,   WS_EX_STATICEDGE)
            CONTROL_RTEXT(          _T("Color:"),      CID_SPEC_COLOR_CAP,      SPEC_CX-HHCY-HHCX/2-10,             14,        HHCX/2-8,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_CTEXT(           _T("COLOR"),      CID_SPEC_COLOR_SEL,             SPEC_CX-HHCY-10,             14,          HHCY+2,         HHCY-2,   SS_NOTIFY | WS_TABSTOP, WS_EX_STATICEDGE)
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
            // Hex edit and Alpha is last
            CONTROL_EDITTEXT(                             CID_RGB_HEX_VAL,          SPEC_CX-2+RGB_CX/2,     HLCYS*0+13,         HHCX-10,           HLCY,   ES_CENTER, 0)
            CONTROL_EDITTEXT(                             CID_RGB_HTM_VAL,          SPEC_CX-2+RGB_CX/2,     HLCYS*1+13,         HHCX-10,           HLCY,   ES_CENTER, 0)
            CONTROL_RTEXT(               _T("A"),         CID_RGB_ALP_CAP,          SPEC_CX+3+RGB_CX/2,     HLCYS*2+13,       HHCX/3-10,           HLCY,   SS_CENTERIMAGE, 0)
            CONTROL_EDITTEXT(                             CID_RGB_ALP_VAL,                     CLMN2_X,     HLCYS*2+13,        HHCX/2+2,           HLCY,   ES_CENTER | ES_NUMBER | WS_GROUP, 0)
        CONTROL_CONTROL(_T(""),  CID_RGB_ALP_UDS,            UPDOWN_CLASS, UD_CHILD,        CLMN2_X+24,     HLCYS*4+16,       HHCX/2-22,           HLCY,   0)
        CONTROL_GROUPBOX(          _T("History"),         CID_GRP_HISTORY,                     CLMNT_X,     2+RGB_CY*2,        RGB_CX-8,       RGB_CY-4,   0, 0)
           CONTROL_CTEXT(                 _T(""),         CID_STA_HISTORY,                   CLMNT_X+4,    14+RGB_CY*2,       RGB_CX-16,      RGB_CY-22,   SS_OWNERDRAW, 0)
      //CONTROL_GROUPBOX(        _T("Magnifier"),       CID_GRP_MAGNIFIER,                     CLMNT_X,     2+RGB_CY*2,        RGB_CX-8,       RGB_CY-4,   0, 0)
          //CONTROL_CTEXT(       _T("MAGNIFIER"),       CID_PLC_MAGNIFIER,                   CLMNT_X+4,    14+RGB_CY*2,       RGB_CX-16,      RGB_CY-22,   SS_SUNKEN | SS_CENTERIMAGE, 0)
      //CONTROL_CONTROL(_T(""), CID_PLC_MAGNIFIER,           WC_MAGNIFIER, MAG_CHILD,        CLMNT_X+4,    14+RGB_CY*2,       RGB_CX-16,      RGB_CY-22,   0)
    END_CONTROLS_MAP()

    BEGIN_DIALOG(0, 0, DLG_CX, DLG_CY)
        DIALOG_STYLE(DS_CONTROL | WS_CHILD | WS_VISIBLE)
        DIALOG_FONT(8, _T("Cascadia Mono Light"))
    END_DIALOG()

    BEGIN_DDX_MAP(CSpectrumColorPicker)
        DDX_CONTROL(CID_SPEC_COLOR_SEL, m_stColor)
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
            OnDDXLoading(nCtlID, bSaveAndValidate);
        }
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(CID_GRP_SPECTRUM, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_SPEC_COMBO, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(CID_BTN_PICK_COLOR, DLSZ_MOVE_X)
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
        DLGRESIZE_CONTROL(CID_GRP_HISTORY, DLSZ_SIZE_Y | DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(CID_STA_HISTORY, DLSZ_SIZE_Y | DLSZ_MOVE_X)
      //DLGRESIZE_CONTROL(CID_GRP_MAGNIFIER, DLSZ_SIZE_Y | DLSZ_MOVE_X)
      //DLGRESIZE_CONTROL(CID_PLC_MAGNIFIER, DLSZ_SIZE_Y | DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP(CColorPicker::Impl)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_DRAWITEM(OnDrawItem)
        CHAIN_MSG_MAP(ImplResizer)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    void SpectruKindChanged();
    LRESULT SliderChanged(bool bNotify);
    void UpdateHexStr();
    void UpdateHtmlStr();
    void UpdateDDX();
    void UpdateColorStatic();
    void DDXReloadEditsExcept(int nId);
    LRESULT ColorChanged(bool bUpdateDDX);
    void OnDDXLoading(UINT nID, BOOL bSaveAndValidate);
    LRESULT OnNotify(int nID, LPNMHDR pnmh);
    void ValidateHexInput(WTL::CEdit& edCtrl);
    void ColorToHistory();
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    void TogglePalette(BOOL bPalVisible) const;
    void ColorpickBegin();
    void SetColorRef(COLORREF crColor);
    void GetColorFromWindowDC(CPoint const& pt);
    void GetColorFromDesktopDC(CPoint const& pt);
    void ColorpickEnd(UINT, CPoint const&, bool bSelect);
    void OnTimer(UINT_PTR nIDEvent);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
    static void DrawColorRect(WTL::CDCHandle dc, CRect const& rc, HistoryItem& item, HBRUSH brBack);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI);
};

CColorPicker::Impl::~Impl() = default;

CColorPicker::Impl::Impl()
    :     m_bSaveData{true}
    ,    m_imSpectrum{0xffffff, SPEC_HSV_Hue}
    ,      m_imSlider{m_imSpectrum.GetSpectrumKindRef(), m_imSpectrum.GetColor()}
    ,       m_stColor{m_imSpectrum.GetMinColorRef(1, 1, 1)}
    , m_nSpectrumKind{m_imSpectrum.GetSpectrumKind()}
{
    UNREFERENCED_PARAMETER(CMagnifierInit::Instance());
}

HRESULT CColorPicker::Impl::PreCreateWindow()
{
    HRESULT code{S_OK};
    code = m_Magnifier.PreCreateWindow();
    if (ERROR_SUCCESS != code) {
        return code;
    }
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

void CColorPicker::Impl::OnDDXLoading(UINT nID, BOOL bSaveAndValidate)
{
    if (m_bSaveData || !bSaveAndValidate) {
        return;
    }
    switch (nID) {
    case CID_SPEC_COMBO:
        SpectruKindChanged();
        break;
    case CID_RGB_RED_VAL:
    case CID_RGB_GRN_VAL:
    case CID_RGB_BLU_VAL:
        m_imSpectrum.GetColor().RGBtoHSV();
        break;
    case CID_HSV_HUE_VAL:
    case CID_HSV_SAT_VAL:
    case CID_HSV_VAL_VAL:
        m_imSpectrum.GetColor().HSVtoRGB();
        break;
    case CID_HSL_HUE_VAL:
    case CID_HSL_SAT_VAL:
    case CID_HSL_LTN_VAL:
        m_imSpectrum.GetColor().FromHSL();
        break;
    case CID_RGB_HEX_VAL:
        if (!m_imSpectrum.GetColor().FromString(m_sColorHex, false)) {
            break;
        }
        goto updated;
    case CID_RGB_HTM_VAL:
        if (!m_imSpectrum.GetColor().FromString(m_sColorHtml, true)) {
            break;
        }
    updated:
        m_imSpectrum.GetColor().RGBtoHSV();
        break;
    default: 
        break;
    }
    if (m_imSpectrum.GetColor().IsUpdated()) {
        ColorChanged(false);
    }
}

void CColorPicker::Impl::UpdateHexStr()
{
    auto const& Color{m_imSpectrum.GetColor()};
    m_sColorHex.Format(TEXT("0x%02x%02x%02x"), Color.GetBlue(), Color.GetGreen(), Color.GetRed());
}

void CColorPicker::Impl::UpdateHtmlStr()
{
    auto const& Color{m_imSpectrum.GetColor()};
    m_sColorHtml.Format(TEXT("#%02X%02X%02X"), Color.GetRed(), Color.GetGreen(), Color.GetBlue());
}

void CColorPicker::Impl::UpdateDDX()
{
    UpdateHexStr();
    UpdateHtmlStr();
    DoDataExchange(DDX_LOAD);
}

void CColorPicker::Impl::UpdateColorStatic()
{
    m_stColor.Reset(m_imSpectrum.GetMinColorRef(1, 1, 1),
                    m_imSpectrum.GetColor().m_A,
                    m_imSpectrum.GetBackBrush());
}

LRESULT CColorPicker::Impl::ColorChanged(bool bUpdateDDX)
{
    if (SPEC_HSV_Hue != m_imSpectrum.GetSpectrumKind()) {
        m_imSlider.UpdateRaster();
    }
    if (bUpdateDDX) {
        CScopedBoolGuard bGuard{m_bSaveData};
        UpdateDDX();
    }
    m_imSpectrum.InvalidateRect(nullptr, FALSE);
    m_imSlider.InvalidateRect(nullptr, FALSE);
    UpdateColorStatic();
    return 0;
}

LRESULT CColorPicker::Impl::SliderChanged(bool bNotify)
{
    if (bNotify) {
        CScopedBoolGuard bGuard{m_bSaveData};
        UpdateDDX();
    }
    m_imSpectrum.InvalidateRect(nullptr, FALSE);
    UpdateColorStatic();
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
    case TBN_SAVE:             // skip
    case BCN_HOTITEMCHANGE:
        return 0;
    case NM_SLIDER_CLR_SEL:
    case NM_SPECTRUM_CLR_SEL:
        switch (nID) {
        case CID_SPECTRUM_PIC: return ColorChanged(true);
        case CID_SPECTRUM_SLD: return SliderChanged(true);
        }
        break;
    }
    DBGTPrint(LTH_WM_NOTIFY L" id:%-4d nc:%-5d %s\n", nID, pnmh->code, DH::WM_NC_C2SW(pnmh->code));
    SetMsgHandled(FALSE);
    return 0;
}

void CColorPicker::Impl::DDXReloadEditsExcept(int nID)
{
    CScopedBoolGuard bGuard{m_bSaveData};
    for (UINT nEdit = CID_EDITS_FIRST; nEdit < CID_EDITS_LAST; nEdit += 2) {
        if (nEdit != nID) {
            DoDataExchange(DDX_LOAD, nEdit);
        }
    }
}

static void CEdit_CursorToEnd(WTL::CEdit& edCtrl)
{
    edCtrl.SetSel(0,-1);
    edCtrl.SetSel(-1);
}

void CColorPicker::Impl::ValidateHexInput(WTL::CEdit& edCtrl)
{
    ATL::CString sColor{};
    edCtrl.GetWindowTextW(sColor);

    EHexHeadType hType{HEX_STR_PLAIN};
    int           nLen{sColor.GetLength()};
    PCTSTR const  pBeg{SkipHexHead(sColor.GetString(), nLen, hType)};
    if (!pBeg) {
        return ;
    }
    PCTSTR const pEnd{pBeg + nLen};
    if (nLen > HEX_STR_LEN_MAX) {
        sColor.ReleaseBufferSetLength(static_cast<int>(pEnd - pBeg) + 1);
        CScopedBoolGuard bGuard{m_bSaveData};
        edCtrl.SetWindowTextW(sColor);
        CEdit_CursorToEnd(edCtrl);
    }

    auto const& pFind{std::find_if_not(pBeg, pEnd, isxdigit)};
    if (pFind != pEnd) {
        sColor.Remove(*pFind);
        CScopedBoolGuard bGuard{m_bSaveData};
        edCtrl.SetWindowTextW(sColor);
        CEdit_CursorToEnd(edCtrl);
    }
}

void CColorPicker::Impl::OnCommand(UINT uNotifyCode, int nID, CWindow /*wndCtl*/)
{
    if (m_bSaveData) {
        return ;
    }
    switch (uNotifyCode) {
    case BN_UNHILITE:
    case BN_DISABLE:
    case BN_KILLFOCUS:
        break;
    case STN_DBLCLK:
        switch (nID) {
        case CID_SPEC_COLOR_SEL:
            ColorToHistory();
            return ;
        }
        break;
    case BN_CLICKED:
        switch (nID) {
        case CID_BTN_PICK_COLOR:
            ColorpickBegin();
            return ;
        }
        break;
    case EN_UPDATE: {
        WTL::CEdit      edCtrl{GetDlgItem(nID)};
        WTL::CUpDownCtrl udCtl{GetDlgItem(nID + 1)};
        if (udCtl.m_hWnd) {
            udCtl.SetPos(udCtl.GetPos());
            CEdit_CursorToEnd(edCtrl);
        }
        else {
            ValidateHexInput(edCtrl);
        }
        if (DoDataExchange(DDX_SAVE, nID)) {
            m_imSpectrum.InvalidateRect(nullptr, FALSE);
            m_imSlider.InvalidateRect(nullptr, FALSE);
            UpdateColorStatic();
            if (CID_RGB_HEX_VAL != nID) {
                UpdateHexStr();
            }
            if (CID_RGB_HTM_VAL != nID) {
                UpdateHtmlStr();
            }
            DDXReloadEditsExcept(nID);
        }
        return ;
    }
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
        DBGTPrint(LTH_WM_COMMAND L" id:%-4d nc:%-5d %s\n", nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
        break;
    }
    SetMsgHandled(FALSE);
}

#if 0
void CColorPicker::Impl::TogglePalette(BOOL bPalVisible) const
{
    WTL::CButton   grpPalette{GetDlgItem(CID_GRP_HISTORY)};
    WTL::CButton grpMagnifier{GetDlgItem(CID_GRP_MAGNIFIER)};
    int const        nPalShow{bPalVisible ? SW_SHOW : SW_HIDE};
    int const        nMagShow{bPalVisible ? SW_HIDE : SW_SHOW};
    grpPalette.ShowWindow(nPalShow);
    grpMagnifier.ShowWindow(nMagShow);
}
#endif


void CColorPicker::Impl::ColorpickBegin()
{
    WTL::CButton bnPick{GetDlgItem(CID_BTN_PICK_COLOR)};
    bnPick.EnableWindow(FALSE);
    m_Magnifier.Show(TRUE);
    SetTimer(TIMER_COLORPICK, TIMER_COLORPICK_MSEC);
}

void CColorPicker::Impl::SetColorRef(COLORREF crColor)
{
    m_imSpectrum.GetColor().SetColorRef(crColor);
    ColorChanged(true);
    ColorToHistory();
}

void CColorPicker::Impl::GetColorFromWindowDC(CPoint const& pt)
{
    ATL::CWindow const  wnd{WindowFromPoint(pt)};
    WTL::CWindowDC const dc{wnd};
    CPoint            ptWin{pt};
    wnd.ScreenToClient(&ptWin);
    COLORREF const  crPixel{dc.GetPixel(ptWin)};
    SetColorRef(crPixel);
    DBGTPrint(LTH_COLORPICKER L" [%p] {%d, %d} ==> 0x%08x\n", wnd.m_hWnd, pt.x, pt.y, crPixel);
}

void CColorPicker::Impl::GetColorFromDesktopDC(CPoint const& pt)
{
    WTL::CDCHandle const dc{::GetDC(nullptr)};
    COLORREF const  crPixel{dc.GetPixel(pt)};
    ::ReleaseDC(nullptr, dc);
    SetColorRef(crPixel);
    DBGTPrint(LTH_COLORPICKER L" {%d, %d} ==> 0x%08x\n", pt.x, pt.y, crPixel);
}

void CColorPicker::Impl::ColorpickEnd(UINT, CPoint const& pt, bool bSelect)
{
    WTL::CButton bnPick{GetDlgItem(CID_BTN_PICK_COLOR)};
    bnPick.EnableWindow(TRUE);
    m_Magnifier.Show(FALSE);
    KillTimer(TIMER_COLORPICK);
    if (bSelect) {
        GetColorFromDesktopDC(pt);
    }
}

void CColorPicker::Impl::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
    case TIMER_COLORPICK:
        m_Magnifier.UpdatePosition();
        break;
    }
}

BOOL CColorPicker::Impl::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);
    m_curArrow = LoadCursorW(nullptr, IDC_ARROW);
    m_curCross = LoadCursorW(nullptr, IDC_CROSS);
    m_curPicker = LoadCursorW(nullptr, IDC_CROSS);
  //WTL::CButton bnPick{GetDlgItem(CID_BTN_PICK_COLOR)};
  //bnPick.SetBitmap((HBITMAP)m_curPicker.m_hCursor);
    SetCursor(m_curArrow);
    ATLASSUME(m_imSpectrum.m_hWnd != nullptr);
    ATLASSUME(m_imSlider.m_hWnd != nullptr);
    WTL::CComboBox cbSpectrum(GetDlgItem(CID_SPEC_COMBO));
    cbSpectrum.AddString(L"RGB/红");
    cbSpectrum.AddString(L"RGB/绿");
    cbSpectrum.AddString(L"RGB/蓝");
    cbSpectrum.AddString(L"HSV/色调");
    cbSpectrum.AddString(L"HSV/饱和度");
    cbSpectrum.AddString(L"HSV/明度");
    struct UDSpinnerConf
    {
        UINT nID;
        int nMin;
        int nMax;
    };
    constexpr UDSpinnerConf udSpinnerConf[] = {
        { CID_RGB_RED_UDS, 0, RGB_MAX_INT },
        { CID_RGB_GRN_UDS, 0, RGB_MAX_INT },
        { CID_RGB_BLU_UDS, 0, RGB_MAX_INT },
        { CID_RGB_ALP_UDS, 0, RGB_MAX_INT },
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
    m_fntHex = CreateFontW(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Anonymous Pro");
    if (m_fntHex) {
        constexpr UINT cuFixedFont[] = {
            CID_RGB_HEX_VAL,
            CID_RGB_HTM_VAL,
        };
        for (auto const& id: cuFixedFont) {
            ATL::CWindow ctl{GetDlgItem(id)};
            ctl.SetFont(m_fntHex.m_hFont, FALSE);
        }
    }
    m_imSpectrum.Initialize(SPECTRUM_CX, SPECTRUM_CY, nullptr, m_curArrow);
    m_imSlider.Initialize(SPECTRUM_SLIDER_CX, m_imSpectrum.GetBackBrush(), m_curCross);
    UpdateDDX();
    SpectruKindChanged();
    m_Magnifier.Initialize(nullptr, MAG_FACTOR, m_curPicker,
        [this](UINT nFlags, CPoint const& pt, bool bSelect) {
            ColorpickEnd(nFlags, pt, bSelect);
        }
    );
  //TogglePalette(TRUE);
    DlgResize_Init(false, true, 0);
    m_bSaveData = false;
    return TRUE;
}

void CColorPicker::Impl::OnDestroy()
{
    if (m_Magnifier.m_hWnd) {
        m_Magnifier.DestroyWindow();
    }
    SetMsgHandled(FALSE);
}

CColorPicker::Impl::HistoryItem::HistoryItem(COLORREF clr, int a)
    : crColor{clr}
    ,  nAlpha{a}
    , m_hPrev{nullptr}
{
}

CColorPicker::Impl::HistoryItem::~HistoryItem()
{
    if (m_hPrev) {
        m_DC.SelectBitmap(m_hPrev);
    }
}

HDC CColorPicker::Impl::HistoryItem::GetDC(HDC dc, CRect const& rc)
{
    if (!m_DC) {
        if (!m_Bitmap) {
            m_Bitmap = CreateCompatibleBitmap(dc, rc.Width(), rc.Height());
        }
        m_DC.CreateCompatibleDC(dc);
        m_hPrev = m_DC.SelectBitmap(m_Bitmap);
        m_DC.FillSolidRect(rc, crColor);
    }
    return m_DC.m_hDC;
}

void CColorPicker::Impl::DrawColorRect(WTL::CDCHandle dc, CRect const& rc, HistoryItem& item, HBRUSH brBack)
{
    if (item.nAlpha < RGB_MAX_INT) {
        CRect const        rcColor{0, 0, 1, 1};
        WTL::CDCHandle const dcClr{item.GetDC(dc, rcColor)};
        BLENDFUNCTION const  blend{AC_SRC_OVER, 0, static_cast<BYTE>(item.nAlpha), 0};
        dc.SelectBrush(brBack);
        dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
        dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(),
                      dcClr, 0, 0, rcColor.Width(), rcColor.Height(), blend);
    }
    else {
        dc.FillSolidRect(rc, item.crColor);
    }
}

void CColorPicker::Impl::ColorToHistory()
{
    COLORREF const crColor{m_imSpectrum.GetMinColorRef(1, 1, 1)};
    int const       nAlpha{m_imSpectrum.GetColor().m_A};
    HistoryItem       item{crColor, nAlpha};
    if (!m_deqHistory.empty()) {
        HistoryItem const& front{m_deqHistory.front()};
        if (item.crColor == front.crColor && item.nAlpha == front.nAlpha) {
            return ;
        }
    }
    m_deqHistory.emplace_front(std::move(item));
    ATL::CWindow{GetDlgItem(CID_STA_HISTORY)}.InvalidateRect(nullptr, FALSE);
}

void CColorPicker::Impl::OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI)
{
    if (CID_STA_HISTORY == nID) {
        WTL::CDCHandle dc{pDI->hDC};
        CRect const    rc{pDI->rcItem};
        int const   iSave{dc.SaveDC()};
        CRect      rcItem{0, 0, CHECKERS_CX, CHECKERS_CX};
        int          nTop{0};
        for (auto& it: m_deqHistory) {
            DrawColorRect(dc, rcItem, it, m_imSpectrum.GetBackBrush());
            dc.DrawEdge(rcItem, EDGE_BUMP, BF_FLAT | BF_RECT);
            rcItem.left += CHECKERS_CX;
            rcItem.right = rcItem.left + CHECKERS_CX;
            if (rcItem.left > rc.right) {
                nTop += rcItem.Height();
                rcItem.SetRect(0, nTop, CHECKERS_CX, nTop + CHECKERS_CX);
            }
        }
        dc.RestoreDC(iSave);
    }
}

void CColorPicker::Impl::SpectruKindChanged()
{
    m_imSpectrum.SetSpectrumKind(static_cast<SpectrumKind>(m_nSpectrumKind));
    m_imSlider.UpdateRaster();
}

CColorPicker::~CColorPicker() = default;

CColorPicker::CColorPicker()
    : CCustomControl{}
    ,        m_pImpl{std::make_unique<Impl>()}
{
}

HRESULT CColorPicker::PreCreateWindow()
{
    auto const code = m_pImpl->PreCreateWindow();
    if (ERROR_SUCCESS != code) {
        return code;
    }
    static ATOM gs_CColorPicker_Atom{0};
    return CCustomControl::PreCreateWindowImpl(gs_CColorPicker_Atom, GetWndClassInfo());
}

BOOL CColorPicker::PreTranslateMessage(MSG* pMsg)
{
    if (m_pImpl->IsDialogMessageW(pMsg)) {
        return TRUE;
    }
    return FALSE;
}

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
#ifdef _DEBUG_XTRA
        if constexpr (true) {
            auto const msgStr = DH::MessageToStrignW((PMSG)GetCurrentMessage());
            DBGTPrint(LTH_CONTROL L" -WM- [[ %s ]]\n", msgStr.c_str());
        }
#endif
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
    ModifyStyleEx(0, WS_EX_CONTROLPARENT);
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
