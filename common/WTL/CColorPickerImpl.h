#pragma once

#include "CColorPicker.h"
#include "CSpectrumImage.h"
#include "CSpectrumSlider.h"
#include "CColorStatic.h"
#include "CMagnifierCtrl.h"
#include <wcdafx.api.h>
#include <atlddx.h>
#include <atlcrack.h>
#include <Magnification.h>

namespace CPInt
{
    constexpr int HCELL_RADIUS{CHECKERS_CX};

    struct HistoryItem
    {
        COLORREF crColor;
        int       nAlpha;

        DELETE_COPY_OF(HistoryItem);

        ~HistoryItem();
        HistoryItem(COLORREF crClr, int nA);

        HistoryItem(HistoryItem&& rhs) noexcept;
        HistoryItem& operator = (HistoryItem&& rhs) noexcept;
        void Swap(HistoryItem& rhs) noexcept;

        HDC GetDC(HDC dc, CRect const& rc);
        bool operator == (const HistoryItem&) const;

    private:
        HBITMAP       m_hPrev;
        WTL::CDC         m_DC;
        WTL::CBitmap m_Bitmap;
    };
}

struct CColorPicker::Impl: WTL::CIndirectDialogImpl<Impl>,
                           WTL::CDialogResize<Impl>,
                           WTL::CWinDataExchange<Impl>
{
    DELETE_COPY_MOVE_OF(Impl);

    using   ImplSuper = WTL::CIndirectDialogImpl<Impl>;
    using ImplResizer = WTL::CDialogResize<Impl>;

    ~Impl() override;
    Impl();

    HRESULT PreCreateWindow();

private:
    friend CColorPicker;
    friend ImplResizer;
    friend ImplSuper;

    using HistoryCont = std::deque<CPInt::HistoryItem>;

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

    static int     gs_nRasterCX;

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
          CONTROL_PUSHBUTTON(             _T(""),      CID_BTN_PICK_COLOR,           SPEC_CX-HHCY-HHCX,             12,        HHCX/4+4,         HLCY+4,   BS_BITMAP, 0)
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
           CONTROL_CTEXT(                 _T(""),         CID_STA_HISTORY,                   CLMNT_X+4,    14+RGB_CY*2,       RGB_CX-15,      RGB_CY-22,   SS_OWNERDRAW | SS_NOTIFY, 0)
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
    void OnEditUpdate(int nID);
    void HistoryDropTail();
    void HistoryStore();
    void HistorySelect(HistoryCont::const_reference item);
    size_t HistoryMax() const;
    void HistoryPick();
    void HistoryLoad(CF::ColorTabItem const* crTab, size_t nCount);
    void OnCommand(UINT uNotifyCode, int nID, HWND);
    void TogglePalette(BOOL bPalVisible) const;
    void ColorpickBegin();
    void SetColorRef(COLORREF crColor, bool bStoreToHistory);
    void GetColorFromWindowDC(CPoint const& pt, bool bStoreToHistory);
    void GetColorFromDesktopDC(CPoint const& pt, bool bStoreToHistory);
    void ColorpickEnd(UINT, CPoint const&, bool bSelect);
    void OnTimer(UINT_PTR nIDEvent);
    static void DrawColorRect(WTL::CDCHandle dc, CRect const& rc, CPInt::HistoryItem& item, HBRUSH brBack);
    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
};
