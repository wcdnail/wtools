#include "stdafx.h"
#include "luicAppearance.h"
#include "luicUtils.h"
#include "luicMain.h"
#include "resz/resource.h"
#include <string.utils.format.h>
#include <dev.assistance/dev.assist.h>
#include <UT/debug.assistance.h>
#include <atldlgs.h>

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

    CtlAdjustHeightAndShift(m_bnImport, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnSave, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnRename, xOffset, yOffset, nHeight);
    CtlAdjustHeightAndShift(m_bnDelete, xOffset, yOffset, nHeight);

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
    auto const* pApp{CLUIApp::App()};

#ifdef _DEBUG_CONTROLS
    ShowWindow(SW_SHOW);
    DoForEach(CtlShow);
#endif
    DoForEach(CtlDisable);

    m_stScheme.Attach(GetDlgItem(IDC_APP_THEME_CAP));
    m_cbScheme.Attach(GetDlgItem(IDC_APP_THEME_SEL));
    m_stSchemeScale.Attach(GetDlgItem(IDC_APP_SIZE_CAP));
    m_cbSchemeScale.Attach(GetDlgItem(IDC_APP_SIZE_SEL));
    m_bnImport.Attach(GetDlgItem(IDC_APP_THEME_BN_IMPORT));
    m_bnSave.Attach(GetDlgItem(IDC_APP_THEME_BN_SAVE));
    m_bnRename.Attach(GetDlgItem(IDC_APP_THEME_BN_RENAME));
    m_bnDelete.Attach(GetDlgItem(IDC_APP_THEME_BN_REMOVE));

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

    m_bnSave.SetIcon(pApp->GetIcon(IconFloppy));
    m_bnRename.SetIcon(pApp->GetIcon(IconEditField));
    m_bnDelete.SetIcon(pApp->GetIcon(IconHatchCross));
    m_bnImport.SetIcon(pApp->GetIcon(IconFolderOpen));

    for (int i = 0; i < IT_ColorCount; i++) {
        m_bnItemColor[i].SetDefaultText(_T("Revert"));
        m_bnItemColor[i].SetCustomText(_T("Customize"));
    }
#ifdef _DEBUG_CONTROLS
    DoForEach(CtlShow);
#endif
    InitializeItems();
    InitializeFonts(pApp->GetFontMap());

    CtlAdjustPositions();
    InitResizeMap();
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

void CPageAppearance::OnDestroy()
{
    CPageImpl::OnDestroy();
}

void CPageAppearance::InitializeScheme(CSchemeManager const& schemeManager)
{
    WTL::CComboBox& lbCtl{m_cbScheme};
    lbCtl.ResetContent();
    int nIndex = 0;
    for (const auto& it: schemeManager.Schemes()) {
        const int nItem = lbCtl.AddString(it->m_Name.c_str());
        if (nItem < 0) {
            const auto code{static_cast<HRESULT>(GetLastError())};
            ReportError(Str::ElipsisW::Format(L"LB AddString [w:%08x] [%d] '%s' ERROR",
                    lbCtl.m_hWnd, nIndex, it->m_Name.c_str()), code);
            continue;
        }
        lbCtl.SetItemData(nItem, static_cast<DWORD_PTR>(nIndex));
        ++nIndex;
    }
}

void CPageAppearance::InitializeScale(CScheme const& sourceScheme)
{
    UNREFERENCED_PARAMETER(sourceScheme);
    m_cbSchemeScale.AddString(L"Normal");
}

void CPageAppearance::InitializeItems()
{
    WTL::CComboBox& lbCtl{m_cbItem};
    lbCtl.ResetContent();
    for (int nIndex = 0; nIndex < IT_Count; nIndex++) {
        auto const& itemDef{CScheme::ItemDef(nIndex)};
        const int     nItem{lbCtl.AddString(itemDef.name)};
        if (nItem < 0) {
            const auto code{static_cast<HRESULT>(GetLastError())};
            ReportError(Str::ElipsisW::Format(L"LB AddString [w:%08x] [%d] '%s' ERROR",
                    lbCtl.m_hWnd, nIndex, itemDef.name), code);
            continue;
        }
        lbCtl.SetItemData(nItem, static_cast<DWORD_PTR>(nIndex));
    }
}

void CPageAppearance::InitializeFonts(FontMap const& fontsMap)
{
    WTL::CComboBox& lbCtl{m_cbFont};
    lbCtl.ResetContent();
    int nIndex = 0;
    for (auto const& it: fontsMap) {
        if (L'@' == it.first[0]) {
            // ##TODO: skip @'at'ed font?
            continue;
        }
        const int nItem{lbCtl.AddString(it.first.c_str())};
        if (nItem < 0) {
            const auto code{static_cast<HRESULT>(GetLastError())};
            ReportError(Str::ElipsisW::Format(L"LB AddString [w:%08x] [%d] '%s' ERROR",
                    lbCtl.m_hWnd, nIndex, it.first.c_str()), code);
            continue;
        }
        lbCtl.SetItemData(nItem, static_cast<DWORD_PTR>(nIndex));
        ++nIndex;
    }
    InitializeFontSizes();
    InitializeFontSmooth();
    InitializeFontButtons();
    m_udItemSize[IT_FontWidth].SetRange32(0, 24);
    m_udItemSize[IT_FontAngle].SetRange32(0, 359);
}

void CPageAppearance::InitializeFontSizes()
{
    WTL::CComboBox& lbCtl{m_cbFontSize};
    lbCtl.ResetContent();
    for (int nIndex = MIN_FONT_SIZE; nIndex <= MAX_FONT_SIZE; nIndex++) {
        auto str = std::to_wstring(nIndex);
        int item = lbCtl.AddString(str.c_str());
        if (item < 0) {
            const auto code{static_cast<HRESULT>(GetLastError())};
            ReportError(Str::ElipsisW::Format(L"LB AddString [w:%08x] [%d] '%s' ERROR",
                    lbCtl.m_hWnd, nIndex, str.c_str()), code);
            continue;
        }
        lbCtl.SetItemData(item, static_cast<DWORD_PTR>(nIndex));
    }
    lbCtl.SetCurSel(0);
}

void CPageAppearance::InitializeFontSmooth()
{
    struct FontSmoothDef
    {
        PCWSTR szName;
        int     nType;
    };
    static constexpr FontSmoothDef fsdData[] = {
        { L"Default", DEFAULT_QUALITY }, 
        { L"Disabled", NONANTIALIASED_QUALITY }, 
        { L"ClearType", CLEARTYPE_QUALITY }, 
        { L"Natural ClearType", CLEARTYPE_NATURAL_QUALITY }, 
    };
    WTL::CComboBox& lbCtl{m_cbFontSmooth};
    lbCtl.ResetContent();
    int nIndex = 0;
    for (auto const& it: fsdData) {
        const int nItem = lbCtl.AddString(it.szName);
        if (nItem < 0) {
            const auto code{static_cast<HRESULT>(GetLastError())};
            ReportError(Str::ElipsisW::Format(L"LB AddString [w:%08x] [%d] '%s' ERROR",
                    lbCtl.m_hWnd, nIndex, it.szName), code);
            continue;
        }
        lbCtl.SetItemData(nItem, it.nType);
        ++nIndex;
    }
}

void CPageAppearance::InitializeFontButtons()
{
    static WTL::CFont gs_fntBold;
    static WTL::CFont gs_fntItalic;
    static WTL::CFont gs_fntUnderline;

    if (!gs_fntBold.m_hFont) {
        HFONT hFont = m_bnFontBold.GetFont();
        LOGFONTW lf;
        GetObjectW(hFont, sizeof(lf), &lf);
        lf.lfWeight = FW_BOLD;
        lf.lfItalic = FALSE;
        lf.lfUnderline = FALSE;
        gs_fntBold.CreateFontIndirectW(&lf);
        lf.lfWeight = FW_NORMAL;
        lf.lfItalic = TRUE;
        gs_fntItalic.CreateFontIndirectW(&lf);
        lf.lfItalic = FALSE;
        lf.lfUnderline = TRUE;
        gs_fntUnderline.CreateFontIndirectW(&lf);
    }

    m_bnFontBold.SetFont(gs_fntBold);
    m_bnFontItalic.SetFont(gs_fntItalic);
    m_bnFontUndrln.SetFont(gs_fntUnderline);
}

void CPageAppearance::OnSchemesChanged(CLUIApp const* pApp, int initialIndex)
{
    CScheme const& sourceScheme = pApp->SchemeManager()[initialIndex];
    InitializeScheme(pApp->SchemeManager());
    InitializeScale(sourceScheme);
    {
        ScopedBoolGuard guard(m_bLoadValues);
        m_cbScheme.SetCurSel(initialIndex);
        m_cbSchemeScale.SetCurSel(0);
        m_cbItem.SetCurSel(IT_Desktop);
        m_cbFont.SetCurSel(0);
        m_cbFontSmooth.SetCurSel(0);
    }
    sourceScheme.CopyTo(m_SchemeCopy);
    OnSchemeChanged(initialIndex);
}

void CPageAppearance::NotifySchemesChanged()
{
    OnSchemesChanged(CLUIApp::App(), 0);
}
