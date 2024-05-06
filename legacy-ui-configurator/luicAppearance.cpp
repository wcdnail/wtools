#include "stdafx.h"
#include "luicAppearance.h"
#include "luicDrawings.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "dev.assistance/dev.assist.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"
#include <atldlgs.h>

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

static inline void CtlAdjustHeight(ATL::CWindow& control, int nHeight)
{
    CRect rc;
    control.GetWindowRect(rc);
    control.SetWindowPos(nullptr, 0, 0, rc.right - rc.left, nHeight,
        SWP_NOMOVE |
        SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOCOPYBITS |
        SWP_NOOWNERZORDER
    );
}

static inline void CtlAdjustHeightAndShift(ATL::CWindow& control, int x, int y, int nHeight)
{
    CRect rc;
    control.GetWindowRect(rc);
    OffsetRect(rc, x, y);
    control.SetWindowPos(nullptr, rc.left, rc.top - 1, rc.right - rc.left, nHeight + 2,
        SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOCOPYBITS |
        SWP_NOOWNERZORDER
    );
}

void CPageAppearance::CtlAdjustPositions()
{
    CRect rcItemCb;
    m_cbItem.GetWindowRect(rcItemCb);
    const int nHeight = rcItemCb.Height();

    CRect rcDialog;
    GetWindowRect(rcDialog);
    int xOffset = -rcDialog.left;
    int yOffset = -rcDialog.top;

    CtlAdjustHeightAndShift(m_bnThemeImport, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnThemeSave, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnThemeRename, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnThemeDelete, xOffset, yOffset, nHeight);

    CtlAdjustHeightAndShift(m_bnItemClr1, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnItemClr2, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontClr1, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontBold, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontItalic, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontUndrln, xOffset, yOffset, nHeight);

    CtlAdjustHeight(m_edItemSize1, nHeight);
    CtlAdjustHeight(m_udItemSize1, nHeight);
    CtlAdjustHeight(m_edItemSize2, nHeight);
    CtlAdjustHeight(m_udItemSize2, nHeight);
    CtlAdjustHeight(m_edFontWidth, nHeight);
    CtlAdjustHeight(m_udFontWidth, nHeight);
    CtlAdjustHeight(m_edFontAngle, nHeight);
    CtlAdjustHeight(m_udFontAngle, nHeight);
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
    m_bnFontClr1.Attach(GetDlgItem(IDC_APP_FONT_COLOR_SEL));
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

    m_bnItemClr1.ModifyStyle(0, BS_BITMAP | BS_PUSHBUTTON);
    m_bnItemClr2.ModifyStyle(0, BS_BITMAP | BS_PUSHBUTTON);
    m_bnFontClr1.ModifyStyle(0, BS_BITMAP | BS_PUSHBUTTON);

    CtlAdjustPositions();
    CTheme::PerformStaticInit(*this, pApp);
    InitResizeMap();
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

void CPageAppearance::OnDestroy()
{
    CPageImpl::OnDestroy();
}

void CPageAppearance::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    switch (nID) {
    case IDC_APP_ITEM_SEL:{
        switch (uNotifyCode) {
        case CBN_SELENDOK:
            OnItemSelect(m_cbItem.GetCurSel());
            break;
        }
        return ;
    }
    case IDC_APP_ITEM_COLOR1_SEL: 
    case IDC_APP_ITEM_COLOR2_SEL:
    case IDC_APP_FONT_COLOR_SEL: {
        if (BN_CLICKED == uNotifyCode) {
            WTL::CColorDialog dlgColorPicker;
            dlgColorPicker.DoModal(m_hWnd);
        }
        return ;
    }
    default:
        break;
    }
    if constexpr (true) {
        DebugThreadPrintf(LTH_WM_NOTIFY L" APPRNCE CMD: id:%-4d nc:%-4d %s\n", 
            nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
    }
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
    //m_bnThemeDelete.EnableWindow(bEnable); /// DELETE button!
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

void CPageAppearance::FontClr1Enable(BOOL bEnable)
{
    m_stFontClr.EnableWindow(bEnable);
    m_bnFontClr1.EnableWindow(bEnable);
}

void CPageAppearance::BtnColorFill(WTL::CButton& bnControl, int nBtn, int iColor)
{
    ATLASSUME(m_pTheme != nullptr);
    HBITMAP hBitmapToSet = nullptr;
    {
        CDrawRoutine   draw{*m_pTheme};
        COLORREF     nColor{m_pTheme->GetColor(iColor)};
        HBRUSH    hbrBorder{AtlGetStockBrush(BLACK_BRUSH)};
        WTL::CWindowDC   dc{m_hWnd};
        WTL::CDC     dcTemp{};
        WTL::CBitmap bmTemp{};
        CRect         rcBtn{};

        bnControl.GetClientRect(rcBtn);
        rcBtn.DeflateRect(rcBtn.Width() / 5, rcBtn.Height() / 5);
        CRect rcBitmap{0, 0, rcBtn.Width(), rcBtn.Height()};

        dcTemp.CreateCompatibleDC(dc);
        bmTemp.CreateCompatibleBitmap(dc, rcBitmap.Width(), rcBitmap.Height());
        const HBITMAP hbmPrev = dcTemp.SelectBitmap(bmTemp);

        dcTemp.FillSolidRect(rcBitmap, nColor);
        draw.DrawBorder(dcTemp.m_hDC, rcBitmap, 1, hbrBorder);

        dcTemp.SelectBitmap(hbmPrev);
        m_bmColor[nBtn].Attach(bmTemp.Detach());
        hBitmapToSet = m_bmColor[nBtn];
    }
    bnControl.SetBitmap(hBitmapToSet);
}

bool CPageAppearance::BtnSetColor(WTL::CButton& bnControl, int nBtn, int iColor)
{
    if ((nBtn < 0) || (nBtn > BTN_ColorCount) || (iColor < 0)) {
        bnControl.SetBitmap(nullptr);
        return false;
    }
    BtnColorFill(bnControl, nBtn, iColor);
    return true;
}

void CPageAppearance::ItemColorSet(int nItem)
{
    ItemClr1Enable(FALSE);
    ItemClr2Enable(FALSE);
    FontClr1Enable(FALSE);
    auto const* pAssignment = CTheme::GetItemAssignment(nItem);
    if (!pAssignment) {
        return ;
    }
    if (BtnSetColor(m_bnItemClr1, BTN_ItemColor1, pAssignment->color1)) {
        ItemClr1Enable(TRUE);
    }
    if (BtnSetColor(m_bnItemClr2, BTN_ItemColor2, pAssignment->color2)) {
        ItemClr2Enable(TRUE);
    }
    if (BtnSetColor(m_bnFontClr1, BTN_FontColor1, pAssignment->fontColor)) {
        FontClr1Enable(TRUE);
    }
}

BOOL CPageAppearance::ItemSizeSet(int metric, int nSizeCtlID, WTL::CUpDownCtrl& udSize)
{
    if (metric < 0 || !m_pTheme) {
        SetDlgItemTextW(nSizeCtlID, L"");
        return FALSE;
    }
    if (auto const* pSizeRange = m_pTheme->GetSizeRange(metric)) {
        udSize.SetRange(pSizeRange->min, pSizeRange->max);
    }
    int nSize = CTheme::GetNcMetricSize(&m_pTheme->GetNcMetrcs(), metric);
    SetDlgItemInt(nSizeCtlID, nSize, FALSE);
    return TRUE;
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
    auto const* pAssignment = CTheme::GetItemAssignment(nItem);
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
    ItemSizeSet(iFont, IDC_APP_FONT_WDTH_EDIT, m_udFontWidth);
    //SetDlgItemInt(IDC_APP_FONT_WDTH_EDIT, FontLogToPt<int>(pLogFont->lfWidth), FALSE);
    SetDlgItemInt(IDC_APP_FONT_STYLE_ANGLE_EDIT, pLogFont->lfEscapement / 10, FALSE);

    m_bnFontBold.SetCheck(pLogFont->lfWeight >= FW_BOLD);
    m_bnFontItalic.SetCheck(pLogFont->lfItalic);
    m_bnFontUndrln.SetCheck(pLogFont->lfUnderline);

    ComboSetCurSelByData(m_cbFontSmooth, (DWORD_PTR)pLogFont->lfQuality);
}

void CPageAppearance::OnItemSelect(int nItem)
{
    m_stPreview.OnSelectItem(nItem);
    m_cbItem.SetCurSel(nItem);
    ItemEnable(TRUE);
    ItemColorSet(nItem);
    ItemSize1Enable(FALSE);
    ItemSize2Enable(FALSE);
    if (auto const* pAssignment = CTheme::GetItemAssignment(nItem)) {
        BOOL bEnable = ItemSizeSet(pAssignment->size1, IDC_APP_ITEM_SIZE1_EDIT, m_udItemSize1);
        ItemSize1Enable(bEnable);
        bEnable = ItemSizeSet(pAssignment->size2, IDC_APP_ITEM_SIZE2_EDIT, m_udItemSize2);
        ItemSize2Enable(bEnable);
    }
    FontOnItemChaged(nItem);
}

void CPageAppearance::OnThemeSelect(int nThemeIndex)
{
    auto const* pApp = CLUIApp::App();
    m_pTheme = &pApp->GetTheme(nThemeIndex);
    ATLASSERT(m_pTheme != nullptr);
    m_stPreview.OnSelectTheme(m_pTheme, &m_cbItem);
    m_stPreview.EnableWindow(TRUE);
    m_cbTheme.SetCurSel(nThemeIndex);
    ThemeEnable(TRUE);
    OnItemSelect(EN_Desktop);
    m_bcGradientCapts.SetCheck(m_pTheme->IsGradientCaptions() ? TRUE : FALSE);
    m_bcFlatMenus.SetCheck(m_pTheme->IsFlatMenus() ? TRUE : FALSE);
}
