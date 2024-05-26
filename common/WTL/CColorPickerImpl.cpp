#include "stdafx.h"
#include "CColorPickerImpl.h"
#include "CColorPickerDefs.h"
#include "CMagnifierInit.h"
#include "CAppModuleRef.h"
#include <dev.assistance/dev.assist.h>
#include <string.utils.error.code.h>
#include <scoped.bool.guard.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlddx.h>
#include <atlmisc.h>
#include <algorithm>
#include <deque>
#include <cctype>

constexpr float MAG_FACTOR{5.f};

namespace CPInt
{
    HistoryItem::~HistoryItem()
    {
        if (m_hPrev) {
            m_DC.SelectBitmap(m_hPrev);
        }
    }

    HistoryItem::HistoryItem(COLORREF crClr, int nA)
        : crColor{crClr}
        ,  nAlpha{nA}
        , m_hPrev{nullptr}
    {
    }

    HistoryItem::HistoryItem(HistoryItem&& rhs)
        : crColor{rhs.crColor}
        ,  nAlpha{rhs.nAlpha}
        , m_hPrev{nullptr}
    {
    }

    HistoryItem& HistoryItem::operator=(HistoryItem&& rhs)
    {
        rhs.Swap(*this);
        return *this;
    }

    void HistoryItem::Swap(HistoryItem& rhs) noexcept
    {
        std::swap(crColor, rhs.crColor);
        std::swap(nAlpha, rhs.nAlpha);
        std::swap(m_hPrev, rhs.m_hPrev);
        std::swap(m_DC.m_hDC, rhs.m_DC.m_hDC);
        std::swap(m_Bitmap.m_hBitmap, rhs.m_Bitmap.m_hBitmap);
    }

    HDC HistoryItem::GetDC(HDC dc, CRect const& rc)
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

    bool HistoryItem::operator==(const HistoryItem& lhs) const
    {
        return nAlpha == lhs.nAlpha &&
               crColor == lhs.crColor;
    }
}

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

void CColorPicker::Impl::SpectruKindChanged()
{
    m_imSpectrum.SetSpectrumKind(static_cast<SpectrumKind>(m_nSpectrumKind));
    m_imSlider.UpdateRaster();
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

void CColorPicker::Impl::OnEditUpdate(int nID)
{
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
}

LRESULT CColorPicker::Impl::OnNotify(int nID, LPNMHDR pnmh)
{
    if (NM_CUSTOMDRAW == pnmh->code) {
        SetMsgHandled(FALSE);
        return 0;
    }
    if (m_bSaveData) {
        return 0;
    }
    switch (pnmh->code) {
    case TBN_SAVE:
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

void CColorPicker::Impl::OnCommand(UINT uNotifyCode, int nID, HWND)
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
        case CID_SPECTRUM_PIC:
        case CID_SPEC_COLOR_SEL: ColorToHistory(); return ;
        }
        break;
    case BN_CLICKED: // STN_CLICKED
        switch (nID) {
        case CID_BTN_PICK_COLOR: ColorpickBegin(); return ;
        case CID_STA_HISTORY:    PickFromHistory(); return ;
        }
        break;
    case EN_UPDATE:     OnEditUpdate(nID); return ;
    case CBN_SELENDOK:  DoDataExchange(DDX_SAVE, nID); return ;
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

void CColorPicker::Impl::TogglePalette(BOOL bPalVisible) const
{
    WTL::CButton   grpPalette{GetDlgItem(CID_GRP_HISTORY)};
    WTL::CButton grpMagnifier{GetDlgItem(CID_GRP_MAGNIFIER)};
    int const        nPalShow{bPalVisible ? SW_SHOW : SW_HIDE};
    int const        nMagShow{bPalVisible ? SW_HIDE : SW_SHOW};
    grpPalette.ShowWindow(nPalShow);
    grpMagnifier.ShowWindow(nMagShow);
}

void CColorPicker::Impl::ColorpickBegin()
{
    WTL::CButton bnPick{GetDlgItem(CID_BTN_PICK_COLOR)};
    bnPick.EnableWindow(FALSE);
    m_Magnifier.Show(TRUE);
    SetTimer(TIMER_COLORPICK, TIMER_COLORPICK_MSEC);
}

void CColorPicker::Impl::SetColorRef(COLORREF crColor, bool bStoreToHistory)
{
    m_imSpectrum.GetColor().SetColorRef(crColor);
    ColorChanged(true);
    if (bStoreToHistory) {
        ColorToHistory();
    }
}

void CColorPicker::Impl::GetColorFromWindowDC(CPoint const& pt, bool bStoreToHistory)
{
    ATL::CWindow const  wnd{WindowFromPoint(pt)};
    WTL::CWindowDC const dc{wnd};
    CPoint            ptWin{pt};
    wnd.ScreenToClient(&ptWin);
    COLORREF const  crPixel{dc.GetPixel(ptWin)};
    SetColorRef(crPixel, bStoreToHistory);
    DBGTPrint(LTH_COLORPICKER L" [%p] {%d, %d} ==> 0x%08x\n", wnd.m_hWnd, pt.x, pt.y, crPixel);
}

void CColorPicker::Impl::GetColorFromDesktopDC(CPoint const& pt, bool bStoreToHistory)
{
    WTL::CDCHandle const dc{::GetDC(nullptr)};
    COLORREF const  crPixel{dc.GetPixel(pt)};
    ::ReleaseDC(nullptr, dc);
    SetColorRef(crPixel, bStoreToHistory);
    DBGTPrint(LTH_COLORPICKER L" {%d, %d} ==> 0x%08x\n", pt.x, pt.y, crPixel);
}

void CColorPicker::Impl::ColorpickEnd(UINT, CPoint const& pt, bool bSelect)
{
    WTL::CButton bnPick{GetDlgItem(CID_BTN_PICK_COLOR)};
    bnPick.EnableWindow(TRUE);
    m_Magnifier.Show(FALSE);
    KillTimer(TIMER_COLORPICK);
    if (bSelect) {
        GetColorFromDesktopDC(pt, true);
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

void CColorPicker::Impl::DrawColorRect(WTL::CDCHandle dc, CRect const& rc, CPInt::HistoryItem& item, HBRUSH brBack)
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
    CPInt::HistoryItem it{crColor, nAlpha};
    if (!m_deqHistory.empty() && it == m_deqHistory.front()) {
    }
    m_deqHistory.emplace_front(std::move(it));
    ATL::CWindow{GetDlgItem(CID_STA_HISTORY)}.InvalidateRect(nullptr, FALSE);
}

void CColorPicker::Impl::SetColorFrom(HistoryCont::const_reference item)
{
    m_imSpectrum.GetColor().m_A = item.nAlpha;
    SetColorRef(item.crColor, false);
}

void CColorPicker::Impl::PickFromHistory()
{
    ATL::CWindow const staHistory{GetDlgItem(CID_STA_HISTORY)};
    CRect                      rc{};
    CPoint                     pt{};
    GetCursorPos(&pt);
    staHistory.GetClientRect(rc);
    staHistory.ScreenToClient(&pt);
    auto const   xPos{static_cast<size_t>(pt.x / CPInt::HCELL_RADIUS)};
    auto const   yPos{static_cast<size_t>(pt.y / CPInt::HCELL_RADIUS)};
    auto const xWidth{static_cast<size_t>(rc.Width() / CPInt::HCELL_RADIUS)};
    auto const nIndex{xPos + yPos * xWidth};
    if (nIndex < m_deqHistory.size()) {
        SetColorFrom(m_deqHistory[nIndex]);
    }
}

void CColorPicker::Impl::OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI)
{
    if (CID_STA_HISTORY == nID) {
        WTL::CDCHandle  dc{pDI->hDC};
        CRect const     rc{pDI->rcItem};
        int const    iSave{dc.SaveDC()};
        HBRUSH const brBrd{WTL::AtlGetStockBrush(GRAY_BRUSH)};
        CRect       rcItem{0, 0, CPInt::HCELL_RADIUS, CPInt::HCELL_RADIUS};
        int           nTop{0};
        for (auto& it: m_deqHistory) {
            DrawColorRect(dc, rcItem, it, m_imSpectrum.GetBackBrush());
            dc.FrameRect(rcItem, brBrd);
            rcItem.left += CPInt::HCELL_RADIUS;
            rcItem.right = rcItem.left + CPInt::HCELL_RADIUS;
            if (rcItem.left > rc.right) {
                nTop += rcItem.Height();
                rcItem.SetRect(0, nTop, CPInt::HCELL_RADIUS, nTop + CPInt::HCELL_RADIUS);
                if (rcItem.top > rc.bottom) {
                    break;
                }
            }
        }
        dc.RestoreDC(iSave);
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
