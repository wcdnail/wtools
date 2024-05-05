#include "stdafx.h"
#include "luicAppearance.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageAppearance::~CPageAppearance()
{
}

CPageAppearance::CPageAppearance(std::wstring&& caption)
    : CPageImpl{ IDD_PAGE_APPEARANCE, std::move(caption) }
    , m_stPreview{}
{
}

void CPageAppearance::InitResizeMap()
{
    static const WTL::_AtlDlgResizeMap ctrlResizeMap[] = {
        { IDC_APP_THEME_CAP,                DLSZ_MOVE_Y },
        { IDC_APP_THEME_SEL,                DLSZ_SIZE_X | DLSZ_MOVE_Y },
        { IDC_APP_SIZE_CAP,                 DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_SIZE_SEL,                 DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_THEME_BN_IMPORT,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_THEME_BN_SAVE,            DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_THEME_BN_RENAME,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_THEME_BN_REMOVE,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_CAP,                 DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SEL,                 DLSZ_SIZE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SIZE1_CAP,           DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SIZE1_EDIT,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SIZE1_SPIN,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SIZE2_CAP,           DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SIZE2_EDIT,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_SIZE2_SPIN,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_COLOR1_CAP,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_COLOR1_SEL,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_COLOR2_CAP,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_ITEM_COLOR2_SEL,          DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_CAP,                 DLSZ_MOVE_Y },
        { IDC_APP_FONT_SEL,                 DLSZ_SIZE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_SIZE_CAP,            DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_SIZE_SEL,            DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_WDTH_CAP,            DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_WDTH_EDIT,           DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_WDTH_SPIN,           DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_COLOR_CAP,           DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_COLOR_SEL,           DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_CAP,           DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_BOLD,          DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_ITALIC,        DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_UNDERLINE,     DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_ANGLE_CAP,     DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_ANGLE_EDIT,    DLSZ_MOVE_Y },
        { IDC_APP_FONT_STYLE_ANGLE_SPIN,    DLSZ_MOVE_Y },
        { IDC_APP_FONT_SMOOTH_CAP,          DLSZ_MOVE_Y },
        { IDC_APP_FONT_SMOOTH_SEL,          DLSZ_MOVE_Y },
        { IDC_APP_CB_TTILEBAR_GRAD,         DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_CB_FLATMENUS,             DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_PREVIEW,                  DLSZ_SIZE_X | DLSZ_SIZE_Y },
    };
    DlgResizeAdd(ctrlResizeMap, std::size(ctrlResizeMap));
}

BOOL CPageAppearance::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    auto const* pApp = CLUIApp::App();
    DoForEach(CtlDisable);

    m_stTheme.Attach(GetDlgItem(IDC_APP_THEME_CAP));
    m_cbTheme.Attach(GetDlgItem(IDC_APP_THEME_SEL));
    m_stThemeSize.Attach(GetDlgItem(IDC_APP_SIZE_CAP));
    m_cbThemeSize.Attach(GetDlgItem(IDC_APP_SIZE_SEL));
    m_bnThemeImport.Attach(GetDlgItem(IDC_APP_THEME_BN_IMPORT));
    m_bnThemeSave.Attach(GetDlgItem(IDC_APP_THEME_BN_SAVE));
    m_bnThemeRename.Attach(GetDlgItem(IDC_APP_THEME_BN_RENAME));
    m_bnThemeDelete.Attach(GetDlgItem(IDC_APP_THEME_BN_REMOVE));

    m_stItem.Attach(GetDlgItem(IDC_APP_ITEM_CAP));
    m_cbItem.Attach(GetDlgItem(IDC_APP_ITEM_SEL));
    m_stItemSize1.Attach(GetDlgItem(IDC_APP_ITEM_SIZE1_CAP));
    m_edItemSize1.Attach(GetDlgItem(IDC_APP_ITEM_SIZE1_EDIT));
    m_udItemSize1.Attach(GetDlgItem(IDC_APP_ITEM_SIZE1_SPIN));
    m_stItemSize2.Attach(GetDlgItem(IDC_APP_ITEM_SIZE2_CAP));
    m_edItemSize2.Attach(GetDlgItem(IDC_APP_ITEM_SIZE2_EDIT));
    m_udItemSize2.Attach(GetDlgItem(IDC_APP_ITEM_SIZE2_SPIN));
    m_stItemClr1.Attach(GetDlgItem(IDC_APP_ITEM_COLOR1_CAP));
    m_bnItemClr1.Attach(GetDlgItem(IDC_APP_ITEM_COLOR1_SEL));
    m_stItemClr2.Attach(GetDlgItem(IDC_APP_ITEM_COLOR2_CAP));
    m_bnItemClr2.Attach(GetDlgItem(IDC_APP_ITEM_COLOR2_SEL));

    m_stFont.Attach(GetDlgItem(IDC_APP_FONT_CAP));
    m_cbFont.Attach(GetDlgItem(IDC_APP_FONT_SEL));
    m_stFontSize.Attach(GetDlgItem(IDC_APP_FONT_SIZE_CAP));
    m_cbFontSize.Attach(GetDlgItem(IDC_APP_FONT_SIZE_SEL));
    m_stFontWidth.Attach(GetDlgItem(IDC_APP_FONT_WDTH_CAP));
    m_edFontWidth.Attach(GetDlgItem(IDC_APP_FONT_WDTH_EDIT));
    m_udFontWidth.Attach(GetDlgItem(IDC_APP_FONT_WDTH_SPIN));
    m_stFontClr.Attach(GetDlgItem(IDC_APP_FONT_COLOR_CAP));
    m_bnFontClr.Attach(GetDlgItem(IDC_APP_FONT_COLOR_SEL));
    m_stFontStyle.Attach(GetDlgItem(IDC_APP_FONT_STYLE_CAP));
    m_bnFontBold.Attach(GetDlgItem(IDC_APP_FONT_STYLE_BOLD));
    m_bnFontItalic.Attach(GetDlgItem(IDC_APP_FONT_STYLE_ITALIC));
    m_bnFontUndrln.Attach(GetDlgItem(IDC_APP_FONT_STYLE_UNDERLINE));
    m_stFontAngle.Attach(GetDlgItem(IDC_APP_FONT_STYLE_ANGLE_CAP));
    m_edFontAngle.Attach(GetDlgItem(IDC_APP_FONT_STYLE_ANGLE_EDIT));
    m_udFontAngle.Attach(GetDlgItem(IDC_APP_FONT_STYLE_ANGLE_SPIN));
    m_stFontSmooth.Attach(GetDlgItem(IDC_APP_FONT_SMOOTH_CAP));
    m_cbFontSmooth.Attach(GetDlgItem(IDC_APP_FONT_SMOOTH_SEL));

    m_bcGradientCapts.Attach(GetDlgItem(IDC_APP_CB_TTILEBAR_GRAD));
    m_bcFlatMenus.Attach(GetDlgItem(IDC_APP_CB_FLATMENUS));

    m_stPreview.SubclassIt(GetDlgItem(IDC_APP_PREVIEW));

    m_bnThemeSave.SetIcon(pApp->GetIcon(IconFloppy));
    m_bnThemeRename.SetIcon(pApp->GetIcon(IconEditField));
    m_bnThemeDelete.SetIcon(pApp->GetIcon(IconHatchCross));
    m_bnThemeImport.SetIcon(pApp->GetIcon(IconFolderOpen));
    m_cbThemeSize.AddString(L"Normal");
    m_cbThemeSize.SetCurSel(0);

    CTheme::PerformStaticInit(*this, pApp);
    InitResizeMap();
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

void CPageAppearance::OnDestroy()
{
    CPageImpl::OnDestroy();
}

void CPageAppearance::ThemeEnable(BOOL bEnable)
{
    m_stTheme.EnableWindow(bEnable);
    m_cbTheme.EnableWindow(bEnable);
    m_stThemeSize.EnableWindow(bEnable);
    m_cbThemeSize.EnableWindow(bEnable);
    m_bnThemeImport.EnableWindow(bEnable);
    m_bnThemeSave.EnableWindow(bEnable);
    m_bnThemeRename.EnableWindow(bEnable);
    m_bnThemeDelete.EnableWindow(bEnable); /// DELETE button!
}

void CPageAppearance::ItemEnable(BOOL bEnable)
{
    m_stItem.EnableWindow(bEnable);
    m_cbItem.EnableWindow(bEnable);
}

void CPageAppearance::ItemSize1Enable(BOOL bEnable)
{
    m_stItemSize1.EnableWindow(bEnable);
    m_edItemSize1.EnableWindow(bEnable);
    m_udItemSize1.EnableWindow(bEnable);
}

void CPageAppearance::ItemSize2Enable(BOOL bEnable)
{
    m_stItemSize2.EnableWindow(bEnable);
    m_edItemSize2.EnableWindow(bEnable);
    m_udItemSize2.EnableWindow(bEnable);
}

void CPageAppearance::ItemClr1Enable(BOOL bEnable)
{
    m_stItemClr1.EnableWindow(bEnable);
    m_bnItemClr1.EnableWindow(bEnable);
}

void CPageAppearance::ItemClr2Enable(BOOL bEnable)
{
    m_stItemClr2.EnableWindow(bEnable);
    m_bnItemClr2.EnableWindow(bEnable);
}

void CPageAppearance::FontEnable(BOOL bEnable)
{
    m_stFont.EnableWindow(bEnable);
    m_cbFont.EnableWindow(bEnable);
    m_stFontSize.EnableWindow(bEnable);
    m_cbFontSize.EnableWindow(bEnable);
    m_stFontWidth.EnableWindow(bEnable);
    m_edFontWidth.EnableWindow(bEnable);
    m_udFontWidth.EnableWindow(bEnable);
    m_stFontClr.EnableWindow(bEnable);
    m_bnFontClr.EnableWindow(bEnable);
    m_stFontStyle.EnableWindow(bEnable);
    m_bnFontBold.EnableWindow(bEnable);
    m_bnFontItalic.EnableWindow(bEnable);
    m_bnFontUndrln.EnableWindow(bEnable);
    m_stFontAngle.EnableWindow(bEnable);
    m_edFontAngle.EnableWindow(bEnable);
    m_udFontAngle.EnableWindow(bEnable);
    m_stFontSmooth.EnableWindow(bEnable);
    m_cbFontSmooth.EnableWindow(bEnable);
}

void CPageAppearance::FontClrEnable(BOOL bEnable)
{
    m_stFontClr.EnableWindow(bEnable);
    m_bnFontClr.EnableWindow(bEnable);
}

void CPageAppearance::BtnFillColor(WTL::CButton& bnControl, HBRUSH hBrush, CBitmap& bmp)
{
    LONG          cx{0};
    LONG          cy{0};
    CRect      rcBtn{};
    CDC     dcCompat{};
    CBitmap bmCompat{};
    CWindowDC     dc{bnControl.m_hWnd};
    bnControl.GetClientRect(rcBtn);
    cx = rcBtn.Width();
    cy = rcBtn.Height();
    rcBtn.DeflateRect(cx/6, cy/4);
    dcCompat.CreateCompatibleDC(dc);
    bmCompat.CreateCompatibleBitmap(dcCompat, rcBtn.Width(), rcBtn.Height());
    bmp.Attach(bmCompat.Detach());
    HBITMAP bmPrev = dcCompat.SelectBitmap(bmp);
    dcCompat.FillRect(rcBtn, hBrush);
    dcCompat.SelectBitmap(bmPrev);
    bnControl.SetBitmap(bmp);
}

bool CPageAppearance::BtnSetColor(WTL::CButton& bnControl, int iColor, CBitmap& bmp) const
{
    HBRUSH hBrush = nullptr;
    if (!m_pTheme || !(hBrush = m_pTheme->GetBrush(iColor))) {
        return false;
    }
    BtnFillColor(bnControl, hBrush, bmp);
    return true;
}

void CPageAppearance::ItemColorSet(int nItem)
{
    ItemClr1Enable(FALSE);
    ItemClr2Enable(FALSE);
    FontClrEnable(FALSE);
    auto const* pAssignment = GetElementAssignment(nItem);
    if (!pAssignment) {
        return ;
    }
    if (BtnSetColor(m_bnItemClr1, pAssignment->color1, m_bmSolid[0])) {
        ItemClr1Enable(TRUE);
    }
    if (BtnSetColor(m_bnItemClr2, pAssignment->color2, m_bmSolid[1])) {
        ItemClr2Enable(TRUE);
    }
    if (BtnSetColor(m_bnFontClr, pAssignment->fontColor, m_bmSolid[2])) {
        FontClrEnable(TRUE);
    }
}

void CPageAppearance::SizeSet(int metric, int textControl, int udControl)
{
    if (metric < 0) {
        SetDlgItemTextW(textControl, L"");
        return;
    }
    //SendDlgItemMessage(m_hWnd, udControl, UDM_SETRANGE, 0L, MAKELONG(g_sizeRanges[metric].max, g_sizeRanges[metric].min));
    //int value = GetSchemeMetric(&g_schemes->scheme.ncMetrics, metric);
    //SetDlgItemInt(g_hDlg, textControl, value, FALSE);
}

void CPageAppearance::FontSetFamily(LOGFONT const* pLogFont)
{
    if (!pLogFont) {
        // ##FIXME: trace error?
        return ;
    }
    const int nFont = m_cbFont.FindStringExact(-1, pLogFont->lfFaceName);
    if (nFont < 0) {
        // ##TODO: trace error
        return ;
    }
    m_cbFont.SetCurSel(nFont);
}

void CPageAppearance::FontSetSizes(LOGFONT const* pLogFont)
{
    LONG size = FontLogToPt<LONG>(pLogFont->lfHeight);
    wchar_t szSize[4] = { 0 };
    if (wsprintf(szSize, L"%d", size) < 1) {
        szSize[0] = L'8';
    }
    int comboIndex;
    if (size < MIN_FONT_SIZE) {
        comboIndex = 0;
    }
    else if (size > MAX_FONT_SIZE) {
        comboIndex = MAX_FONT_SIZE - MIN_FONT_SIZE;
    }
    else {
        comboIndex = m_cbFontSize.FindStringExact(CB_ERR, szSize);
        if (comboIndex < 0) {
            comboIndex = 2; // ##TODO: ENUM this!
        }
    }
    m_cbFontSize.SetCurSel(comboIndex);
    m_cbFontSize.SetWindowTextW(szSize);
}

void CPageAppearance::FontOnItemChaged(int nItem)
{
    auto const* pAssignment = GetElementAssignment(nItem);
    const int         iFont = pAssignment ? pAssignment->font : TI_Invalid;
    if (iFont < 0) {
        FontEnable(FALSE);
        return;
    }
    auto const* pLogFont = m_pTheme->GetLogFont(nItem);
    if (!pLogFont) {
        FontEnable(FALSE);
        return;
    }
    FontEnable(TRUE);
    FontSetFamily(pLogFont);
    FontSetSizes(pLogFont);
    //SetDlgItemInt(g_hDlg, IDC_CLASSIC_FONTWIDTH_E, FontLogToPt(plfFont->lfWidth), FALSE);
    //SetDlgItemInt(g_hDlg, IDC_CLASSIC_FONTANGLE_E, plfFont->lfEscapement / 10, FALSE);
    //SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FONTBOLD, BM_SETCHECK, (WPARAM)(plfFont->lfWeight >= FW_BOLD), 0L);
    //SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FONTITALIC, BM_SETCHECK, (WPARAM)plfFont->lfItalic, 0L);
    //SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FONTUNDERLINE, BM_SETCHECK, (WPARAM)plfFont->lfUnderline, 0L);
    //SelectFontSmoothing(lfFont);
}

void CPageAppearance::OnSelectTheme(int nThemeIndex)
{
    auto const* pApp = CLUIApp::App();
    m_pTheme = &pApp->GetTheme(nThemeIndex);
    m_stPreview.OnSelectTheme(m_pTheme);
    m_stPreview.EnableWindow(TRUE);
    m_cbTheme.SetCurSel(nThemeIndex);
    ThemeEnable(TRUE);
    OnSelectItem(EN_Desktop);
}

void CPageAppearance::OnSelectItem(int nItem)
{
    m_stPreview.OnSelectItem(nItem);
    m_cbItem.SetCurSel(nItem);
    ItemEnable(TRUE);
    FontOnItemChaged(nItem);
    ItemColorSet(nItem);
}
