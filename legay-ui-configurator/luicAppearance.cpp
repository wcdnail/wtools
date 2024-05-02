#include "stdafx.h"
#include "luicAppearance.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageAppearance::~CPageAppearance()
{
}

CPageAppearance::CPageAppearance()
    : CPageImpl(IDD_PAGE_APPEARANCE)
    , m_Preview()
{
}

void CPageAppearance::InitResizeMap()
{
    static const WTL::_AtlDlgResizeMap ctrlResizeMap[] = {
        { IDC_APP_THEME_CAP,                DLSZ_MOVE_Y },
        { IDC_APP_THEME_SEL,                DLSZ_SIZE_X | DLSZ_MOVE_Y },
        { IDC_APP_SIZE_CAP,                 DLSZ_MOVE_X | DLSZ_MOVE_Y },
        { IDC_APP_SIZE_SEL,                 DLSZ_MOVE_X | DLSZ_MOVE_Y },
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
    if (!m_Preview.SubclassWindow(GetDlgItem(IDC_APP_PREVIEW))) {
        HRESULT code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Previewer SubclassWindow failure!"), code, true, MB_ICONERROR);
    }
    InitResizeMap();
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

void CPageAppearance::OnDestroy()
{
    if (m_Preview.DestroyWindow()) {
        m_Preview.m_hWnd = nullptr;
    }
    CPageImpl::OnDestroy();
}
