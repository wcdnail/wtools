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
    , m_Preview{}
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

    m_Preview.SubclassIt(GetDlgItem(IDC_APP_PREVIEW));
    m_ThemeSel.Attach(GetDlgItem(IDC_APP_THEME_SEL));
    m_ThemeSizeSel.Attach(GetDlgItem(IDC_APP_SIZE_SEL));
    m_ThemeImport.Attach(GetDlgItem(IDC_APP_THEME_BN_IMPORT));
    m_ThemeSave.Attach(GetDlgItem(IDC_APP_THEME_BN_SAVE));
    m_ThemeRename.Attach(GetDlgItem(IDC_APP_THEME_BN_RENAME));
    m_ThemeDelete.Attach(GetDlgItem(IDC_APP_THEME_BN_REMOVE));
    m_ElementSel.Attach(GetDlgItem(IDC_APP_ITEM_SEL));
    m_FontSel.Attach(GetDlgItem(IDC_APP_FONT_SEL));

    m_ThemeSave.SetIcon(pApp->GetIcon(IconFloppy));
    m_ThemeRename.SetIcon(pApp->GetIcon(IconEditField));
    m_ThemeDelete.SetIcon(pApp->GetIcon(IconHatchCross));
    m_ThemeImport.SetIcon(pApp->GetIcon(IconFolderOpen));
    m_ThemeSizeSel.AddString(L"Normal");
    m_ThemeSizeSel.SetCurSel(0);

    CTheme::PerformStaticInit(*this, pApp);
    InitResizeMap();
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

void CPageAppearance::OnDestroy()
{
    CPageImpl::OnDestroy();
}

void CPageAppearance::OnSelectFont(LOGFONT const* lfFont)
{
    if (!lfFont) {
        // ##FIXME: trace error?
        return ;
    }
    const int nFont = m_FontSel.FindStringExact(-1, lfFont->lfFaceName);
    if (nFont < 0) {
        // ##TODO: trace error
        return ;
    }
    m_FontSel.SetCurSel(nFont);
}

void CPageAppearance::OnSelectTheme(int nThemeIndex)
{
    auto const* pApp = CLUIApp::App();
    m_pTheme = &pApp->GetTheme(nThemeIndex);
    m_Preview.OnSelectTheme(m_pTheme);

    m_ThemeSel.SetCurSel(nThemeIndex);
    
    m_Preview.EnableWindow(TRUE);
    m_ThemeSel.EnableWindow(TRUE);
    m_ThemeSave.EnableWindow(TRUE);
    m_ThemeRename.EnableWindow(TRUE);
    m_ElementSel.EnableWindow(TRUE);

    OnSelectItem(EN_Desktop);
}

void CPageAppearance::OnSelectItem(int nItem)
{
    m_Preview.OnSelectItem(nItem);
    m_ElementSel.SetCurSel(nItem);
    if (m_pTheme) {
        auto const* pLogFont = m_pTheme->GetLogFont(nItem);
        if (!pLogFont) {
            // ##TODO: report why pLogFont is NULL
        }
        else {
            OnSelectFont(pLogFont);
        }
    }
}
