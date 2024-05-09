#include "stdafx.h"
#include "luicAppearance.h"
#include "luicUtils.h"
#include "resz/resource.h"
#include <string.utils.format.h>
#include <dev.assistance/dev.assist.h>
#include <UT/debug.assistance.h>
#include <atldlgs.h>

CPageAppearance::~CPageAppearance() = default;

CPageAppearance::CPageAppearance(std::wstring&& caption)
    : CPageImpl{IDD_PAGE_APPEARANCE, std::move(caption)}
    , m_bLoadValues{false}
{
}

void CPageAppearance::ThemeEnable(BOOL bEnable)
{
    m_stScheme.EnableWindow(bEnable);
    m_cbScheme.EnableWindow(bEnable);
    m_stSchemeScale.EnableWindow(bEnable);
    m_cbSchemeScale.EnableWindow(bEnable);
    m_bnImport.EnableWindow(bEnable);
    m_bnSave.EnableWindow(bEnable);
    m_bnRename.EnableWindow(bEnable);
    m_bnDelete.EnableWindow(bEnable);
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
    const auto rItemDef = CScheme::ItemDef(nItem);
    if (ItemColorSetBtn(IT_Color1, rItemDef.color1)) {
        ItemClr1Enable(TRUE);
    }
    if (ItemColorSetBtn(IT_Color2, rItemDef.color2)) {
        ItemClr2Enable(TRUE);
    }
    if (ItemColorSetBtn(IT_FontColor1, rItemDef.fontColor)) {
        FontClr1Enable(TRUE);
    }
}

void CPageAppearance::ItemSizeClear(int nSize)
{
    ScopedBoolGuard guard(m_bLoadValues);
    if ((nSize < 0) ||  (nSize > IT_SizeCount - 1)) {
        return ;
    }
    m_stItemSize[nSize].EnableWindow(FALSE);
    m_edItemSize[nSize].EnableWindow(FALSE);
    m_udItemSize[nSize].EnableWindow(FALSE);
    SetDlgItemTextW(m_edItemSize[nSize].GetDlgCtrlID(), L"");
}

bool CPageAppearance::ItemSizeChanged(int nItem, ItemDef rItemDef, int iSizeControl, bool bApply /*= false*/)
{
    if ((iSizeControl < 0) ||  (iSizeControl > IT_SizeCount - 1)) {
        return false;
    }
    if (IT_Invalid == nItem) {
        return false;
    }
    int nMetric = IT_Invalid;
    int  nValue = bApply ? GetDlgItemInt(m_edItemSize[iSizeControl].GetDlgCtrlID(), nullptr, FALSE) : IT_Invalid;
    switch (iSizeControl) {
    case IT_Size1:
        nMetric = rItemDef.size1;
        goto GetSizeVal;
    case IT_Size2:
        nMetric = rItemDef.size2;
    GetSizeVal:
        if (-1 == nMetric) {
            return false;
        }
        //if (bApply) {
        //    CTheme::GetNcMetricSize(&m_pTheme->GetNcMetrcs(), nMetric) = nValue;
        //}
        //else {
        //    nValue = CTheme::GetNcMetricSize(&m_pTheme->GetNcMetrcs(), nMetric);
        //}
        break;
    case IT_FontWidth:
        if (-1 == rItemDef.font) {
            return false;
        }
        //if (bApply) {
        //    LOGFONT& lf = m_pTheme->GetLogFont(rItemDef.font);
        //    lf.lfWidth = FontPtToLog<int>(nValue);
        //    m_pTheme->RefreshHFont(rItemDef.font, lf);
        //}
        //else {
        //    nValue = FontLogToPt<int>(m_pTheme->GetLogFont(nMetric).lfWidth);
        //}
        break;
    case IT_FontAngle:
        //if (bApply) {
        //    LOGFONT& lf = m_pTheme->GetLogFont(rItemDef.font);
        //    lf.lfEscapement = nValue * 10;
        //    m_pTheme->RefreshHFont(rItemDef.font, lf);
        //}
        //else {
        //    //m_udItemSize[nSize].SetRange(0, 359);
        //    nValue = m_pTheme->GetLogFont(nMetric).lfEscapement / 10;
        //}
        break;
    default:
        return false;
    }
    if (bApply) {
        return true;
    }
    if (IT_Invalid != nMetric) {
        //if (auto const* pSizeRange = m_pTheme->GetSizeRange(nMetric)) {
        //    m_udItemSize[iSizeControl].SetRange(pSizeRange->min, pSizeRange->max);
        //}
    }
    SetDlgItemInt(m_edItemSize[iSizeControl].GetDlgCtrlID(), nValue, FALSE);
    m_stItemSize[iSizeControl].EnableWindow(TRUE);
    m_edItemSize[iSizeControl].EnableWindow(TRUE);
    m_udItemSize[iSizeControl].EnableWindow(TRUE);
    return true;
}

void CPageAppearance::FontSetFamily(LOGFONT const& logFont)
{
    ScopedBoolGuard guard(m_bLoadValues);
    const int nFont = m_cbFont.FindStringExact(-1, logFont.lfFaceName);
    if (nFont < 0) {
        // ##TODO: trace error
        return ;
    }
    m_cbFont.SetCurSel(nFont);
}

void CPageAppearance::FontSetSizes(LOGFONT const& logFont)
{
    ScopedBoolGuard guard(m_bLoadValues);
    const LONG size = FontLogToPt<LONG>(logFont.lfHeight);
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

static bool CBGetCurText(WTL::CComboBox const& ctlCombo, ATL::CString& result)
{
    const int item = ctlCombo.GetCurSel();
    if (CB_ERR == item) {
        return false;
    }
    return CB_ERR != ctlCombo.GetLBText(item, result);
}

#if 0
static bool CBGetCurData(WTL::CComboBox const& ctlCombo, int& result)
{
    const int item = ctlCombo.GetCurSel();
    if (CB_ERR == item) {
        return false;
    }
    int temp = ctlCombo.GetItemData(item);
    if (CB_ERR == temp) {
        return false;
    }
    result = temp;
}
#endif

static bool CBGetCurTextInt(WTL::CComboBox const& ctlCombo, int& result)
{
    ATL::CString strTemp;
    if (!CBGetCurText(ctlCombo, strTemp)) {
        return false;
    }
    int temp = CB_ERR;
    if (_stscanf_s(strTemp.GetString(), _T("%d"), &temp) < 0) {
        return false;
    }
    result = temp;
    return true;
}


bool CPageAppearance::ItemFontApplyChanges(int nItem, ItemDef rItemDef, int iFont, int iFontControl) const
{
    WTL::CLogFont lfCopy = m_pTheme->GetLogFont(iFont);
    switch (iFontControl) {
    case IDC_APP_FONT_SEL:{
        ATL::CString strFamily;
        if (!CBGetCurText(m_cbFont, strFamily)) {
            return false;
        }
        ZeroMemory(lfCopy.lfFaceName, sizeof(lfCopy.lfFaceName));
        _tcsncpy_s(lfCopy.lfFaceName, strFamily.GetString(), std::min<size_t>(strFamily.GetLength(), sizeof(lfCopy.lfFaceName)));
        break;
    }
    case IDC_APP_FONT_SIZE_SEL:{
        int nSize = IT_Invalid;
        if (!CBGetCurTextInt(m_cbFontSize, nSize) || IT_Invalid == nSize) {
            return false;
        }
        lfCopy.lfHeight = FontPtToLog<LONG>(nSize);
        break;
    }
    case IDC_APP_FONT_SMOOTH_SEL:
        break;
    case IDC_APP_FONT_STYLE_BOLD:
        lfCopy.lfWeight = m_bnFontBold.GetCheck() ? FW_BOLD : FW_NORMAL;
        break;
    case IDC_APP_FONT_STYLE_ITALIC:
        lfCopy.lfItalic = m_bnFontItalic.GetCheck();
        break;
    case IDC_APP_FONT_STYLE_UNDERLINE:
        lfCopy.lfUnderline = m_bnFontUndrln.GetCheck();
        break;
    default:
        return false;
    }
    WTL::CLogFont lfNew{};
    WTL::CFont    fnNew{lfCopy.CreateFontIndirectW()};
    if (!fnNew.m_hFont) {
        // TODO: report CreateFontIndirectW
        return false;
    }
    if (!fnNew.GetLogFont(lfNew)) {
        // TODO: report GetLogFont
        return false;
    }
    m_pTheme->SetFont(iFont, lfNew, fnNew.Detach());
    // FTODO: trace/check lfTarget
    return true;
}

bool CPageAppearance::ItemFontChanged(int nItem, ItemDef rItemDef, int iFontControl, bool bApply)
{
    const int iFont = rItemDef ? rItemDef.font : IT_Invalid;
    if (iFont < 0) {
        if (!bApply) {
            FontEnable(FALSE);
        }
        return false;
    }
    if (bApply) {
        return ItemFontApplyChanges(nItem, rItemDef, iFont, iFontControl);
    }
    LOGFONT const& logFont = m_pTheme->GetLogFont(iFont);
    FontEnable(TRUE);
    FontSetFamily(logFont);
    FontSetSizes(logFont);
    ItemSizeClear(IT_FontWidth);
    ItemSizeChanged(iFont, rItemDef, IT_FontWidth);
    ItemSizeChanged(iFont, rItemDef, IT_FontAngle);
    {
        ScopedBoolGuard guard(m_bLoadValues);
        m_bnFontBold.SetCheck(logFont.lfWeight >= FW_BOLD);
        m_bnFontItalic.SetCheck(logFont.lfItalic);
        m_bnFontUndrln.SetCheck(logFont.lfUnderline);
        ComboSetCurSelByData(m_cbFontSmooth, (DWORD_PTR)logFont.lfQuality);
    }
    return true;
}

void CPageAppearance::OnItemSelect(int nItem)
{
    auto const* rItemDef = CTheme::GetItemAssignment(nItem);
    m_stPreview.OnSelectItem(nItem);
    m_cbItem.SetCurSel(nItem);
    ItemEnable(TRUE);
    ItemColorSet(nItem);
    ItemSizeClear(IT_Size1);
    ItemSizeClear(IT_Size2);
    ItemSizeChanged(nItem, rItemDef, IT_Size1, false);
    ItemSizeChanged(nItem, rItemDef, IT_Size2, false);
    ItemFontChanged(nItem, rItemDef);
}

void CPageAppearance::OnThemeSelect(int nThemeIndex)
{
    m_stPreview.OnSelectTheme(m_pTheme, &m_cbItem);
    m_stPreview.EnableWindow(TRUE);
    m_cbScheme.SetCurSel(nThemeIndex);
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
    // skip CColorButton reflected & other
    switch (uNotifyCode) {
    case BN_PAINT:
    case BN_UNHILITE:
    case BN_DISABLE:
    case CBN_SELENDCANCEL:
        SetMsgHandled(FALSE);
        return ;
    }
    // skip during controls initialization
    if (m_bLoadValues) {
        return ;
    }
    int iItem = IT_Invalid;
    int iSize = IT_Invalid;
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
        iItem = ItemGetSel(rItemDef);
        if (ItemSizeChanged(iItem, rItemDef, iSize, true)) {
            m_stPreview.InvalidateRect(nullptr, FALSE);
        }
        return ;
    case IDC_APP_FONT_SEL:
    case IDC_APP_FONT_SIZE_SEL:
    case IDC_APP_FONT_SMOOTH_SEL:
    case IDC_APP_FONT_STYLE_BOLD:
    case IDC_APP_FONT_STYLE_ITALIC:
    case IDC_APP_FONT_STYLE_UNDERLINE:
        switch (uNotifyCode) {
        case CBN_SELENDOK:
        case BN_CLICKED:
            iItem = ItemGetSel(rItemDef);
            if (ItemFontChanged(iItem, rItemDef, nID, true)) {
                m_stPreview.InvalidateRect(nullptr, FALSE);
            }
            return ;
        }
        break;
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

int CPageAppearance::ItemGetSel(ItemDef& rItemDef) const
{
    if (!m_cbItem.m_hWnd) {
        return IT_Invalid;
    }
    const int nItem = m_cbItem.GetCurSel();
    if (IT_Invalid == nItem) {
        return IT_Invalid;
    }
    rItemDef = CTheme::GetItemAssignment(nItem);
    return nItem;
}

void CPageAppearance::ItemColorTryChange(int nButton)
{
    if (!m_pTheme) {
        return ;
    }
    const COLORREF  clrTryed = m_bnItemColor[nButton].GetColor();
    ItemDef rItemDef = nullptr;
    const int nItem = ItemGetSel(rItemDef);
    if (IT_Invalid == nItem || !rItemDef) {
        DH::TPrintf(LTH_APPEARANCE L" <<FAILED>> '%s' (%d) ==> #%08x\n", _T(__FUNCTION__), nButton, clrTryed);
        return ;
    }
    int nWhich = IT_Invalid;
    switch (nButton) {
    case IT_Color1:     nWhich = rItemDef.color1; break;
    case IT_Color2:     nWhich = rItemDef.color2; break;
    case IT_FontColor1: nWhich = rItemDef.fontColor; break;
    }
    const bool bSuccess = m_pTheme->SetColor(nWhich, clrTryed);
    DH::TPrintf(LTH_APPEARANCE L" Item: '%s' SetColor '%s'[%d] ==> #%08x == %s\n", 
        rItemDef.name,
        CTheme::ColorName(nWhich),
        nWhich,
        clrTryed,
        bSuccess ? L"OK" : L"FAIL"
    );
    if (bSuccess) {
        m_stPreview.InvalidateRect(nullptr, FALSE);
    }
    
}
