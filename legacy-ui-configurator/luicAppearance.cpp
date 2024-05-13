#include "stdafx.h"
#include "luicAppearance.h"
#include "luicUtils.h"
#include "luicMain.h"
#include "resz/resource.h"
#include <string.utils.format.h>
#include <dev.assistance/dev.assist.h>
#include <UT/debug.assistance.h>
#include <atldlgs.h>

namespace
{
    bool CBGetCurText(WTL::CComboBox const& ctlCombo, ATL::CString& result)
    {
        const int item = ctlCombo.GetCurSel();
        if (CB_ERR == item) {
            return false;
        }
        return CB_ERR != ctlCombo.GetLBText(item, result);
    }

    bool CBGetCurData(WTL::CComboBox const& ctlCombo, int& result)
    {
        const int item = ctlCombo.GetCurSel();
        if (CB_ERR == item) {
            return false;
        }
        const int temp = static_cast<int>(ctlCombo.GetItemData(item));
        if (CB_ERR == temp) {
            return false;
        }
        result = temp;
        return true;
    }

    bool CBGetCurTextInt(WTL::CComboBox const& ctlCombo, int& result)
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
}

CPageAppearance::~CPageAppearance() = default;

CPageAppearance::CPageAppearance(std::wstring&& caption)
    :     CPageImpl{IDD_PAGE_APPEARANCE, std::move(caption)}
    ,     m_pSource{}
    ,  m_SchemeCopy{L"<<COPY>>"}
    , m_bLoadValues{false}
    , m_nPrevScheme{IT_Invalid}
    ,   m_nPrevSize{IT_Invalid}
{
    const HRESULT code = m_stPreview.PreCreateWindow();
    if (FAILED(code)) {
        ReportError(L"CThemePreviewer registration failed!", code, true);
    }
}

void CPageAppearance::SchemeEnable(BOOL bEnable)
{
    m_stScheme.EnableWindow(bEnable);
    m_cbScheme.EnableWindow(bEnable);
    m_stSchemeScale.EnableWindow(bEnable);
    m_cbSchemeSize.EnableWindow(bEnable);
    m_bnImport.EnableWindow(bEnable);
    m_bnSave.EnableWindow(bEnable);
    m_bnRename.EnableWindow(bEnable);
    m_bnDelete.EnableWindow(bEnable);
    m_bcGradientCapts.EnableWindow(bEnable);
    m_bcFlatMenus.EnableWindow(bEnable);
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
    if ((nButton < 0) || (nButton > IT_ColorCount - 1) || (iColor < 0)) {
        return false;
    }
    const COLORREF clrToSet = m_SchemeCopy.GetColor(iColor);
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

bool CPageAppearance::ItemSizeChanged(int nItem, int iSizeControl, bool bApply /*= false*/)
{
    if ((iSizeControl < 0) ||  (iSizeControl > IT_SizeCount - 1)) {
        return false;
    }
    if (IT_Invalid == nItem) {
        return false;
    }
    WTL::CLogFont logFont{};
    auto const&  rItemDef{CScheme::ItemDef(nItem)};
    int           nMetric{IT_Invalid};
    int             nFont{IT_Invalid};
    int            nValue{bApply ? static_cast<int>(GetDlgItemInt(m_edItemSize[iSizeControl].GetDlgCtrlID(), nullptr, FALSE)) : IT_Invalid};
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
        if (bApply) {
            m_SchemeCopy.GetNCMetric(m_sCurrentSize, nMetric) = nValue;
        }
        else {
            nValue = m_SchemeCopy.GetNCMetric(m_sCurrentSize, nMetric);
        }
        break;
    case IT_FontWidth:
        if (-1 == rItemDef.font) {
            return false;
        }
        nFont = rItemDef.font;
        if (bApply) {
            logFont = m_SchemeCopy.GetLogFont(m_sCurrentSize, nFont);
            logFont.lfWidth = FontPtToLog<int>(nValue);
        }
        else {
            nValue = FontLogToPt<int>(m_SchemeCopy.GetLogFont(m_sCurrentSize, nFont).lfWidth);
        }
        break;
    case IT_FontAngle:
        if (-1 == rItemDef.font) {
            return false;
        }
        nFont = rItemDef.font;
        if (bApply) {
            logFont = m_SchemeCopy.GetLogFont(m_sCurrentSize, nFont);
            logFont.lfEscapement = nValue * 10;
        }
        else {
            nValue = m_SchemeCopy.GetLogFont(m_sCurrentSize, nFont).lfEscapement / 10;
        }
        break;
    default:
        return false;
    }
    if (bApply) {
        if (nFont != IT_Invalid) {
            m_SchemeCopy.GetFontPair(m_sCurrentSize, nFont).Reset(logFont);
        }
        return true;
    }
    if (IT_Invalid != nMetric) {
        auto const& sizeRange = m_SchemeCopy.GetSizeRange(nMetric);
        m_udItemSize[iSizeControl].SetRange(sizeRange.min, sizeRange.max);
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

bool CPageAppearance::ItemFontApplyChanges(int nItem, ItemDef rItemDef, int iFont, int iFontControl)
{
    WTL::CLogFont lfCopy = m_SchemeCopy.GetLogFont(m_sCurrentSize, iFont);
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
    case IDC_APP_FONT_SMOOTH_SEL:{
        int nSmooth = IT_Invalid;
        if (!CBGetCurData(m_cbFontSmooth, nSmooth) || IT_Invalid == nSmooth) {
            return false;
        }
        lfCopy.lfQuality = nSmooth;
        break;
    }
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
    if (!m_SchemeCopy.GetFontPair(m_sCurrentSize, iFont).Reset(lfCopy)) {
        return false;
    }
    return true;
}

bool CPageAppearance::ItemFontChanged(int nItem, int iFontControl, bool bApply)
{
    auto const& rItemDef = CScheme::ItemDef(nItem);
    const int      iFont = rItemDef.font;
    if (iFont < 0) {
        if (!bApply) {
            FontEnable(FALSE);
        }
        return false;
    }
    if (bApply) {
        return ItemFontApplyChanges(nItem, rItemDef, iFont, iFontControl);
    }
    WTL::CLogFont const& logFont = m_SchemeCopy.GetLogFont(m_sCurrentSize, iFont);
    FontEnable(TRUE);
    FontSetFamily(logFont);
    FontSetSizes(logFont);
    ItemSizeClear(IT_FontWidth);
    ItemSizeChanged(iFont, IT_FontWidth);
    ItemSizeChanged(iFont, IT_FontAngle);
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
    auto const& rItemDef = CScheme::ItemDef(nItem);
    m_stPreview.OnItemSelected(nItem);
    m_cbItem.SetCurSel(nItem);
    ItemEnable(TRUE);
    ItemColorSet(nItem);
    ItemSizeClear(IT_Size1);
    ItemSizeClear(IT_Size2);
    ItemSizeChanged(nItem, IT_Size1, false);
    ItemSizeChanged(nItem, IT_Size2, false);
    ItemFontChanged(nItem);
}

void CPageAppearance::OnSchemeSizeChanged()
{
    ATL::CString sTemp;
    if (CBGetCurText(m_cbSchemeSize, sTemp)) {
        String{sTemp.GetString(), static_cast<size_t>(sTemp.GetLength())}.swap(m_sCurrentSize);
    }
    else {
        // TODO: report m_cbSchemeSize.GetCurSel
        m_sCurrentSize = m_SchemeCopy.GetSizesMap().cbegin()->first;
    }
    m_stPreview.OnSchemeChanged(m_SchemeCopy, m_SchemeCopy.GetSizePair(m_sCurrentSize), m_cbItem);
    m_stPreview.InvalidateRect(nullptr, FALSE);
    m_nPrevSize = m_cbSchemeSize.GetCurSel();
}

void CPageAppearance::OnSchemeSelected(CSchemePtr& pSource)
{
    m_pSource = pSource;
    ATLASSUME(pSource.get() != nullptr);
    m_pSource->CopyTo(m_SchemeCopy);
    InitializeSizes();
    {
        ScopedBoolGuard guard(m_bLoadValues);
        m_cbSchemeSize.SetCurSel(0);
        m_cbItem.SetCurSel(IT_Desktop);
        m_cbFont.SetCurSel(0);
        m_cbFontSize.SetCurSel(0);
        m_cbFontSmooth.SetCurSel(0);
    }
    OnSchemeSizeChanged();
    SchemeEnable(TRUE);
    OnItemSelect(IT_Desktop);
    m_bcGradientCapts.SetCheck(m_SchemeCopy.IsGradientCaptions() ? TRUE : FALSE);
    m_bcFlatMenus.SetCheck(m_SchemeCopy.IsFlatMenus() ? TRUE : FALSE);
    m_nPrevScheme = m_cbScheme.GetCurSel();
}

#if 0
void CPageAppearance::ColorPicker(int nButton)
{
    UNREFERENCED_ARG(nButton);
    // TODO: implement custom color picker dialog
    constexpr COLORREF srcColor = 0x00ff00ff;
    WTL::CColorDialog dlgColorPicker(srcColor, CC_FULLOPEN, m_hWnd);
    dlgColorPicker.DoModal(m_hWnd);
}
#endif

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

int CPageAppearance::ItemGetSel() const
{
    if (!m_cbItem.m_hWnd) {
        return IT_Invalid;
    }
    const int nItem = m_cbItem.GetCurSel();
    if (IT_Invalid == nItem) {
        return IT_Invalid;
    }
    return nItem;
}

void CPageAppearance::ItemColorTryChange(int nButton)
{
    const COLORREF  clrTryed = m_bnItemColor[nButton].GetColor();
    const int nItem = ItemGetSel();
    if (IT_Invalid == nItem) {
        DH::TPrintf(LTH_APPEARANCE L" <<FAILED>> '%s' (%d) ==> #%08x\n", _T(__FUNCTION__), nButton, clrTryed);
        return ;
    }
    auto const& rItemDef = CScheme::ItemDef(nItem);
    int           nWhich = IT_Invalid;
    switch (nButton) {
    case IT_Color1:     nWhich = rItemDef.color1; break;
    case IT_Color2:     nWhich = rItemDef.color2; break;
    case IT_FontColor1: nWhich = rItemDef.fontColor; break;
    }
    const bool bSuccess = m_SchemeCopy.GetColorPair(nWhich).Reset(clrTryed);
    DH::TPrintf(LTH_APPEARANCE L" Item: '%s' SetColor '%s'[%d] ==> #%08x == %s\n", 
        rItemDef.name,
        CColors::Title(nWhich),
        nWhich,
        clrTryed,
        bSuccess ? L"OK" : L"FAIL"
    );
    if (bSuccess) {
        m_stPreview.InvalidateRect(nullptr, FALSE);
    }
}

void CPageAppearance::ApplyPendingChanges() const
{
    if (!m_pSource->IsNotEqual(m_SchemeCopy)) {
        return ;
    }
    const int nAnswer = ::MessageBoxW(m_hWnd, L"Reject current changes?", L"Changes pending...", MB_YESNO | MB_ICONQUESTION);
    if (IDYES == nAnswer) {
        SetMFStatus(STA_Info, L"Pending changes rejected...");
        return ;
    }
    m_SchemeCopy.CopyTo(*m_pSource);
}

void CPageAppearance::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    // skip during controls initialization
    if (m_bLoadValues) {
        return ;
    }
    // skip CColorButton reflected & other
    switch (uNotifyCode) {
    case BN_PAINT:
    case BN_UNHILITE:
    case BN_DISABLE:
    case BN_KILLFOCUS:
    case CBN_SELENDCANCEL:
    case CBN_CLOSEUP:
        SetMsgHandled(FALSE);
        return ;
    }
    int       iItem{IT_Invalid};
    int       iSize{IT_Invalid};
    PCWSTR oDlgExts{nullptr};
    switch (nID) {
    case IDC_APP_THEME_SEL: {
        int nScheme = IT_Invalid;
        if (m_nPrevScheme == m_cbScheme.GetCurSel()) {
            return ;
        }
        ApplyPendingChanges();
        if (!CBGetCurData(m_cbScheme, nScheme)) {
            m_cbScheme.SetCurSel(m_nPrevScheme);
            return ;
        }
        auto& manager{CLUIApp::App()->SchemeManager()};
        auto&  source{manager[nScheme]};
        if (!source) {
            m_cbScheme.SetCurSel(m_nPrevScheme);
            return ;
        }
        OnSchemeSelected(source);
        return ;
    }
    case IDC_APP_SIZE_SEL: {
        if (m_nPrevSize == m_cbSchemeSize.GetCurSel()) {
            return ;
        }
        OnSchemeSizeChanged();
        return ;
    }
    case IDC_APP_ITEM_SEL: {
        OnItemSelect(m_cbItem.GetCurSel());
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
        iItem = ItemGetSel();
        if (ItemSizeChanged(iItem, iSize, true)) {
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
            iItem = ItemGetSel();
            if (ItemFontChanged(iItem, nID, true)) {
                m_stPreview.InvalidateRect(nullptr, FALSE);
            }
            return ;
        }
        break;
    case IDC_APP_CB_TTILEBAR_GRAD:
        m_SchemeCopy.SetGradientCaptions(m_bcGradientCapts.GetCheck() != 0);
        goto UpdatePreview;
    case IDC_APP_CB_FLATMENUS:
        m_SchemeCopy.SetFlatMenus(m_bcFlatMenus.GetCheck() != 0);
    UpdatePreview:
        m_stPreview.InvalidateRect(nullptr, FALSE);
        return ;

    case IDC_APP_THEME_BN_IMPORT: {
        CPoint      pt{};
        CRect rcButton{};
        m_bnImport.GetWindowRect(rcButton);
        pt.x = rcButton.left < 0 ? 0 : rcButton.left;
        pt.y = rcButton.bottom;
        m_mnuImport.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, m_hWnd);
        return ;
    }
    case IDM_IMPORT_REGISTRY:{
        auto*       pApp{CLUIApp::App()};
        const int nIndex{pApp->SchemeManager().LoadRegistry()};
        if (IT_Invalid != nIndex) {
            ApplyPendingChanges();
            OnSchemesLoad(pApp, nIndex);
        }
        return ;
    }
    case IDM_IMPORT_WIN98THEME: oDlgExts = L"*.theme;*.the"; goto ImportFile;
    case IDM_IMPORT_WINXPREGFILE: {
        oDlgExts = L"*.reg";
    ImportFile:
        WTL::CShellFileOpenDialog dlg{ oDlgExts, FOS_FORCESHOWHIDDEN | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST };
        const auto rv = dlg.DoModal(m_hWnd);
        if (IDOK != rv) {
            SetMFStatus(STA_Warning, L"Import canceled");
            return ;
        }
        std::wstring tempFilename;
        const HRESULT code = IFileDialog_GetDisplayName(*dlg.m_spFileDlg, tempFilename);
        if (FAILED(code)) {
            ReportError(L"File dialog failed", code);
            return ;
        }
        const auto path{CSchemeManager::Path{tempFilename}};
        auto*      pApp{CLUIApp::App()};
        auto&   manager{pApp->SchemeManager()};
        int       index{IT_Invalid};
        switch (nID) {
        case IDM_IMPORT_WIN98THEME:   index = manager.LoadWin98THeme(path); break;
        case IDM_IMPORT_WINXPREGFILE: index = manager.LoadXPRegistry(path); break;
        }
        if (IT_Invalid != index) {
            ApplyPendingChanges();
            OnSchemesLoad(pApp, index);
        }
        return ;
    }
    case IDM_CLEAR_SCHEMES: {
        auto*       pApp{CLUIApp::App()};
        const int nIndex{pApp->SchemeManager().VanishAllExceptLast()};
        if (IT_Invalid != nIndex) {
            ApplyPendingChanges();
            OnSchemesLoad(pApp, nIndex);
        }
        return ;
    }
    case IDC_APP_THEME_BN_REMOVE: {
        auto*    pApp{CLUIApp::App()};
        auto& manager{pApp->SchemeManager()};
        if (manager.GetSchemes().size() < 2) {
            SetMFStatus(STA_Warning, L"Remove is imposible - no more schemes loaded");
            return ;
        }
        const int nAnswer{::MessageBoxW(m_hWnd, L"Remove current scheme?", L"Remove confirmation", MB_YESNO | MB_ICONQUESTION)};
        if (IDYES != nAnswer) {
            return ;
        }
        const int nIndex{manager.Remove(m_SchemeCopy)};
        if (IT_Invalid != nIndex) {
            ApplyPendingChanges();
            OnSchemesLoad(pApp, nIndex);
        }
        return ;
    }
    default:
        break;
    }
    SetMsgHandled(FALSE);
    if constexpr (true) {
        DBGTPrint(LTH_WM_NOTIFY L" APPRNCE CMD: id:%-4d nc:%-4d %s\n", 
            nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
    }
}
