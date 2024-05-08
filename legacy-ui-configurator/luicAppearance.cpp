#include "stdafx.h"
#include "luicAppearance.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "dev.assistance/dev.assist.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"
#include <atldlgs.h>

namespace 
{
    struct ScopedBoolGuard
    {
        ScopedBoolGuard(bool& target, bool initial = false) : target_{target}, initial_{initial} { target_ = !initial_; }
        ~ScopedBoolGuard() { target_ = initial_; }
        bool& target_;
        bool initial_;
    };
}

CPageAppearance::~CPageAppearance() = default;

CPageAppearance::CPageAppearance(std::wstring&& caption)
    : CPageImpl{IDD_PAGE_APPEARANCE, std::move(caption)}
    , m_bLoadValues{false}
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
    const int xOffset = -rcDialog.left;
    const int yOffset = -rcDialog.top;

    CtlAdjustHeightAndShift(m_bnThemeImport, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnThemeSave, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnThemeRename, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnThemeDelete, xOffset, yOffset, nHeight);

    CtlAdjustHeightAndShift(m_bnItemColor[IT_Color1], xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnItemColor[IT_Color2], xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnItemColor[IT_FontColor1], xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontBold, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontItalic, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnFontUndrln, xOffset, yOffset, nHeight);

    CtlAdjustHeight(m_edItemSize[IT_Size1], nHeight);
    CtlAdjustHeight(m_udItemSize[IT_Size1], nHeight);
    CtlAdjustHeight(m_edItemSize[IT_Size2], nHeight);
    CtlAdjustHeight(m_udItemSize[IT_Size2], nHeight);
    CtlAdjustHeight(m_edItemSize[IT_FontWidth], nHeight);
    CtlAdjustHeight(m_udItemSize[IT_FontWidth], nHeight);
    CtlAdjustHeight(m_edItemSize[IT_FontAngle], nHeight);
    CtlAdjustHeight(m_udItemSize[IT_FontAngle], nHeight);
}

BOOL CPageAppearance::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
#ifdef _DEBUG_CONTROLS
    ShowWindow(SW_SHOW);
    DoForEach(CtlShow);
#endif
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
    m_stItemSize[IT_Size1].Attach(GetDlgItem(IDC_APP_ITEM_SIZE1_CAP));
    m_edItemSize[IT_Size1].Attach(GetDlgItem(IDC_APP_ITEM_SIZE1_EDIT));
    m_udItemSize[IT_Size1].Attach(GetDlgItem(IDC_APP_ITEM_SIZE1_SPIN));
    m_stItemSize[IT_Size2].Attach(GetDlgItem(IDC_APP_ITEM_SIZE2_CAP));
    m_edItemSize[IT_Size2].Attach(GetDlgItem(IDC_APP_ITEM_SIZE2_EDIT));
    m_udItemSize[IT_Size2].Attach(GetDlgItem(IDC_APP_ITEM_SIZE2_SPIN));
    m_stItemColor[IT_Color1].Attach(GetDlgItem(IDC_APP_ITEM_COLOR1_CAP));
    m_bnItemColor[IT_Color1].SubclassWindow(GetDlgItem(IDC_APP_ITEM_COLOR1_SEL));
    m_stItemColor[IT_Color2].Attach(GetDlgItem(IDC_APP_ITEM_COLOR2_CAP));
    m_bnItemColor[IT_Color2].SubclassWindow(GetDlgItem(IDC_APP_ITEM_COLOR2_SEL));

    m_stFont.Attach(GetDlgItem(IDC_APP_FONT_CAP));
    m_cbFont.Attach(GetDlgItem(IDC_APP_FONT_SEL));
    m_stFontSize.Attach(GetDlgItem(IDC_APP_FONT_SIZE_CAP));
    m_cbFontSize.Attach(GetDlgItem(IDC_APP_FONT_SIZE_SEL));
    m_stItemSize[IT_FontWidth].Attach(GetDlgItem(IDC_APP_FONT_WDTH_CAP));
    m_edItemSize[IT_FontWidth].Attach(GetDlgItem(IDC_APP_FONT_WDTH_EDIT));
    m_udItemSize[IT_FontWidth].Attach(GetDlgItem(IDC_APP_FONT_WDTH_SPIN));
    m_stItemColor[IT_FontColor1].Attach(GetDlgItem(IDC_APP_FONT_COLOR_CAP));
    m_bnItemColor[IT_FontColor1].SubclassWindow(GetDlgItem(IDC_APP_FONT_COLOR_SEL));
    m_stFontStyle.Attach(GetDlgItem(IDC_APP_FONT_STYLE_CAP));
    m_bnFontBold.Attach(GetDlgItem(IDC_APP_FONT_STYLE_BOLD));
    m_bnFontItalic.Attach(GetDlgItem(IDC_APP_FONT_STYLE_ITALIC));
    m_bnFontUndrln.Attach(GetDlgItem(IDC_APP_FONT_STYLE_UNDERLINE));
    m_stItemSize[IT_FontAngle].Attach(GetDlgItem(IDC_APP_FONT_STYLE_ANGLE_CAP));
    m_edItemSize[IT_FontAngle].Attach(GetDlgItem(IDC_APP_FONT_STYLE_ANGLE_EDIT));
    m_udItemSize[IT_FontAngle].Attach(GetDlgItem(IDC_APP_FONT_STYLE_ANGLE_SPIN));
    m_stFontSmooth.Attach(GetDlgItem(IDC_APP_FONT_SMOOTH_CAP));
    m_cbFontSmooth.Attach(GetDlgItem(IDC_APP_FONT_SMOOTH_SEL));

    m_bcGradientCapts.Attach(GetDlgItem(IDC_APP_CB_TTILEBAR_GRAD));
    m_bcFlatMenus.Attach(GetDlgItem(IDC_APP_CB_FLATMENUS));

    m_stPreview.SubclassIt(GetDlgItem(IDC_APP_PREVIEW));

    m_bnThemeSave.SetIcon(pApp->GetIcon(IconFloppy));
    m_bnThemeRename.SetIcon(pApp->GetIcon(IconEditField));
    m_bnThemeDelete.SetIcon(pApp->GetIcon(IconHatchCross));
    m_bnThemeImport.SetIcon(pApp->GetIcon(IconFolderOpen));

    for (int i = 0; i < IT_ColorCount; i++) {
        m_bnItemColor[i].SetDefaultText(_T("Revert"));
        m_bnItemColor[i].SetCustomText(_T("Customize"));
    }

#ifdef _DEBUG_CONTROLS
    DoForEach(CtlShow);
#endif

    CtlAdjustPositions();
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
    m_bnThemeDelete.EnableWindow(bEnable);
}

void CPageAppearance::ItemEnable(BOOL bEnable)
{
    m_stItem.EnableWindow(bEnable);
    m_cbItem.EnableWindow(bEnable);
}

void CPageAppearance::ItemClr1Enable(BOOL bEnable)
{
    m_stItemColor[IT_Color1].EnableWindow(bEnable);
    m_bnItemColor[IT_Color1].EnableWindow(bEnable);
}

void CPageAppearance::ItemClr2Enable(BOOL bEnable)
{
    m_stItemColor[IT_Color2].EnableWindow(bEnable);
    m_bnItemColor[IT_Color2].EnableWindow(bEnable);
}

void CPageAppearance::FontEnable(BOOL bEnable)
{
    m_stFont.EnableWindow(bEnable);
    m_cbFont.EnableWindow(bEnable);
    m_stFontSize.EnableWindow(bEnable);
    m_cbFontSize.EnableWindow(bEnable);
    m_stItemSize[IT_FontWidth].EnableWindow(bEnable);
    m_edItemSize[IT_FontWidth].EnableWindow(bEnable);
    m_udItemSize[IT_FontWidth].EnableWindow(bEnable);
    m_stFontStyle.EnableWindow(bEnable);
    m_bnFontBold.EnableWindow(bEnable);
    m_bnFontItalic.EnableWindow(bEnable);
    m_bnFontUndrln.EnableWindow(bEnable);
    m_stItemSize[IT_FontAngle].EnableWindow(bEnable);
    m_edItemSize[IT_FontAngle].EnableWindow(bEnable);
    m_udItemSize[IT_FontAngle].EnableWindow(bEnable);
    m_stFontSmooth.EnableWindow(bEnable);
    m_cbFontSmooth.EnableWindow(bEnable);
}

void CPageAppearance::FontClr1Enable(BOOL bEnable)
{
    m_stItemColor[IT_FontColor1].EnableWindow(bEnable);
    m_bnItemColor[IT_FontColor1].EnableWindow(bEnable);
}

bool CPageAppearance::ItemColorSetBtn(int nButton, int iColor)
{
    if ((nButton < 0) || (nButton > IT_ColorCount) || (iColor < 0)) {
        return false;
    }
    const COLORREF clrToSet = m_pTheme->GetColor(iColor);
    m_bnItemColor[nButton].SetDefaultColor(clrToSet);
    m_bnItemColor[nButton].SetDefaultText(Str::ElipsisW::Format(L"Revert #%06X", clrToSet));
    m_bnItemColor[nButton].SetColor(clrToSet);
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
    if (ItemColorSetBtn(IT_Color1, pAssignment->color1)) {
        ItemClr1Enable(TRUE);
    }
    if (ItemColorSetBtn(IT_Color2, pAssignment->color2)) {
        ItemClr2Enable(TRUE);
    }
    if (ItemColorSetBtn(IT_FontColor1, pAssignment->fontColor)) {
        FontClr1Enable(TRUE);
    }
}

void CPageAppearance::ItemSizeClear(int nSize)
{
    ScopedBoolGuard guard(m_bLoadValues, m_bLoadValues);
    if ((nSize < 0) ||  (nSize > IT_SizeCount - 1)) {
        return ;
    }
    m_stItemSize[nSize].EnableWindow(FALSE);
    m_edItemSize[nSize].EnableWindow(FALSE);
    m_udItemSize[nSize].EnableWindow(FALSE);
    SetDlgItemTextW(m_edItemSize[nSize].GetDlgCtrlID(), L"");
}

bool CPageAppearance::ItemSizeChanged(int nItem, int nSize, PCItemAssign pAssignment, bool bApply)
{
    if ((nSize < 0) ||  (nSize > IT_SizeCount - 1)) {
        return false;
    }
    if (!m_pTheme || (IT_Invalid == nItem) || !pAssignment) {
        return false;
    }
    int nMetric = IT_Invalid;
    int  nValue = bApply ? GetDlgItemInt(m_edItemSize[nSize].GetDlgCtrlID(), nullptr, FALSE) : IT_Invalid;
    switch (nSize) {
    case IT_Size1:
        nMetric = pAssignment->size1;
        goto GetSizeVal;
    case IT_Size2:
        nMetric = pAssignment->size2;
    GetSizeVal:
        if (-1 == nMetric) {
            return false;
        }
        if (bApply) {
            CTheme::GetNcMetricSize(&m_pTheme->GetNcMetrcs(), nMetric) = nValue;
        }
        else {
            nValue = CTheme::GetNcMetricSize(&m_pTheme->GetNcMetrcs(), nMetric);
        }
        break;
    case IT_FontWidth:
        if (-1 == pAssignment->font) {
            return false;
        }
        if (bApply) {
            LOGFONT& lf = m_pTheme->GetLogFont(pAssignment->font);
            lf.lfWidth = FontPtToLog<int>(nValue);
            m_pTheme->RefreshHFont(pAssignment->font, lf);
        }
        else {
            nValue = FontLogToPt<int>(m_pTheme->GetLogFont(nMetric).lfWidth);
        }
        break;
    case IT_FontAngle:
        if (bApply) {
            LOGFONT& lf = m_pTheme->GetLogFont(pAssignment->font);
            lf.lfEscapement = nValue * 10;
            m_pTheme->RefreshHFont(pAssignment->font, lf);
        }
        else {
            //m_udItemSize[nSize].SetRange(0, 359);
            nValue = m_pTheme->GetLogFont(nMetric).lfEscapement / 10;
        }
        break;
    }
    if (bApply) {
        return true;
    }
    if (IT_Invalid != nMetric) {
        if (auto const* pSizeRange = m_pTheme->GetSizeRange(nMetric)) {
            m_udItemSize[nSize].SetRange(pSizeRange->min, pSizeRange->max);
        }
    }
    SetDlgItemInt(m_edItemSize[nSize].GetDlgCtrlID(), nValue, FALSE);
    m_stItemSize[nSize].EnableWindow(TRUE);
    m_edItemSize[nSize].EnableWindow(TRUE);
    m_udItemSize[nSize].EnableWindow(TRUE);
    return true;
}

void CPageAppearance::FontSetFamily(LOGFONT const& logFont)
{
    const int nFont = m_cbFont.FindStringExact(-1, logFont.lfFaceName);
    if (nFont < 0) {
        // ##TODO: trace error
        return ;
    }
    m_cbFont.SetCurSel(nFont);
}

void CPageAppearance::FontSetSizes(LOGFONT const& logFont)
{
    LONG size = FontLogToPt<LONG>(logFont.lfHeight);
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

void CPageAppearance::FontOnItemChaged(int nItem, PCItemAssign pAssignment)
{
    const int iFont = pAssignment ? pAssignment->font : IT_Invalid;
    if (iFont < 0) {
        FontEnable(FALSE);
        return;
    }
    auto& logFont = m_pTheme->GetLogFont(nItem);
    FontEnable(TRUE);
    FontSetFamily(logFont);
    FontSetSizes(logFont);
    ItemSizeClear(IT_FontWidth);
    ItemSizeChanged(iFont, IT_FontWidth, pAssignment, false);
    ItemSizeChanged(iFont, IT_FontAngle, pAssignment, false);
    m_bnFontBold.SetCheck(logFont.lfWeight >= FW_BOLD);
    m_bnFontItalic.SetCheck(logFont.lfItalic);
    m_bnFontUndrln.SetCheck(logFont.lfUnderline);
    ComboSetCurSelByData(m_cbFontSmooth, (DWORD_PTR)logFont.lfQuality);
}

void CPageAppearance::OnItemSelect(int nItem)
{
    auto const* pAssignment = CTheme::GetItemAssignment(nItem);
    m_stPreview.OnSelectItem(nItem);
    m_cbItem.SetCurSel(nItem);
    ItemEnable(TRUE);
    ItemColorSet(nItem);
    ItemSizeClear(IT_Size1);
    ItemSizeClear(IT_Size2);
    ItemSizeChanged(nItem, IT_Size1, pAssignment, false);
    ItemSizeChanged(nItem, IT_Size2, pAssignment, false);
    FontOnItemChaged(nItem, pAssignment);
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

void CPageAppearance::ColorPicker(int nButton)
{
    UNREFERENCED_ARG(nButton);
    // TODO: implement custom color picker dialog
    //COLORREF srcColor = 0x00ff00ff;
    //WTL::CColorDialog dlgColorPicker(srcColor, CC_FULLOPEN, m_hWnd);
    //dlgColorPicker.DoModal(m_hWnd);
}

void CPageAppearance::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    if (m_bLoadValues) {
        return ;
    }
    PCItemAssign pAssignment = nullptr;
    int                iItem = IT_Invalid;
    int                iSize = IT_Invalid;
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
    case IDC_APP_FONT_COLOR_SEL:
        SetMsgHandled(FALSE);
        return ;
    case IDC_APP_ITEM_SIZE1_EDIT:       iSize = IT_Size1; goto SizeChanged;
    case IDC_APP_ITEM_SIZE2_EDIT:       iSize = IT_Size2; goto SizeChanged;
    case IDC_APP_FONT_WDTH_EDIT:        iSize = IT_FontWidth; goto SizeChanged;
    case IDC_APP_FONT_STYLE_ANGLE_EDIT: iSize = IT_FontAngle; goto SizeChanged;
    SizeChanged:
        iItem = ItemGetSel(pAssignment);
        if (ItemSizeChanged(iItem, iSize, pAssignment, true)) {
            m_stPreview.InvalidateRect(nullptr, FALSE);
        }
        return ;
    case IDC_APP_FONT_SEL:
    case IDC_APP_FONT_SIZE_SEL:
    case IDC_APP_FONT_SMOOTH_SEL:
    case IDC_APP_FONT_STYLE_BOLD:
    case IDC_APP_FONT_STYLE_ITALIC:
    case IDC_APP_FONT_STYLE_UNDERLINE:
        return ;
    default:
        break;
    }
    SetMsgHandled(FALSE);
    if constexpr (true) {
        DBGTPrint(LTH_WM_NOTIFY L" APPRNCE CMD: id:%-4d nc:%-4d %s\n", 
            nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
    }
}

LRESULT CPageAppearance::OnNotify(int idCtrl, LPNMHDR pnmh)
{
    if (CPN_SELENDOK == pnmh->code) {
        int nButton = CB_ERR;
        switch (idCtrl) {
        case IDC_APP_ITEM_COLOR1_SEL: nButton = IT_Color1; goto TryChange;
        case IDC_APP_ITEM_COLOR2_SEL: nButton = IT_Color2; goto TryChange;
        case IDC_APP_FONT_COLOR_SEL:  nButton = IT_FontColor1;
     TryChange:
            ItemColorTryChange(nButton);
            break;
        default:
            break;
        }
        SetMsgHandled(TRUE);
        return 0;
    }
    SetMsgHandled(FALSE);
    if constexpr (false) {
        DBGTPrint(LTH_WM_NOTIFY L" APPRNCE NTY: id:%-4d nc:%-4d %s\n", 
            idCtrl, pnmh->code, DH::WM_NC_C2SW(pnmh->code));
    }
    return 0;
}

int CPageAppearance::ItemGetSel(PCItemAssign& pAssignment) const
{
    if (!m_cbItem.m_hWnd) {
        return IT_Invalid;
    }
    const int nItem = m_cbItem.GetCurSel();
    if (IT_Invalid == nItem) {
        return IT_Invalid;
    }
    pAssignment = CTheme::GetItemAssignment(nItem);
    return nItem;
}

void CPageAppearance::ItemColorTryChange(int nButton)
{
    if (!m_pTheme) {
        return ;
    }
    const COLORREF  clrTryed = m_bnItemColor[nButton].GetColor();
    PCItemAssign pAssignment = nullptr;
    const int nItem = ItemGetSel(pAssignment);
    if (IT_Invalid == nItem || !pAssignment) {
        DH::TPrintf(LTH_APPEARANCE L" <<FAILED>> '%s' (%d) ==> #%08x\n", _T(__FUNCTION__), nButton, clrTryed);
        return ;
    }
    int nWhich = IT_Invalid;
    switch (nButton) {
    case IT_Color1:     nWhich = pAssignment->color1; break;
    case IT_Color2:     nWhich = pAssignment->color2; break;
    case IT_FontColor1: nWhich = pAssignment->fontColor; break;
    }
    const bool bSuccess = m_pTheme->SetColor(nWhich, clrTryed);
    DH::TPrintf(LTH_APPEARANCE L" Item: '%s' SetColor '%s'[%d] ==> #%08x == %s\n", 
        pAssignment->name,
        CTheme::ColorName(nWhich),
        nWhich,
        clrTryed,
        bSuccess ? L"OK" : L"FAIL"
    );
    if (bSuccess) {
        m_stPreview.InvalidateRect(nullptr, FALSE);
    }
    
}
