#include "stdafx.h"
#include "luicAppearance.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageAppearance::~CPageAppearance()
{
}

CPageAppearance::CPageAppearance()
    : CPageImpl(IDD_PAGE_APPEARANCE)
{
}

BOOL CPageAppearance::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    // Preview ---
    m_Preview.Attach(GetDlgItem(IDC_TEST_STA1));
    m_Preview.ModifyStyle(0, SS_OWNERDRAW);
    DlgResizeAdd(IDC_TEST_STA1, DLSZ_SIZE_X | DLSZ_SIZE_Y);

    // Scheme sel/save/delete ---
    DlgResizeAdd(IDC_STA_SCHEME, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_CMB_SCHEME, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_BN_SAVE_AS, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_BN_DELETE, DLSZ_MOVE_Y);

    // Item ---
    DlgResizeAdd(IDC_STA_ITEM, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_CMB_ITEM, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_STA_ITEM_SIZE_CAP, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_ITEM_SIZE_SPIN, DLSZ_MOVE_Y);

    // Font ---
    DlgResizeAdd(IDC_STA_FONT, DLSZ_MOVE_Y);
    DlgResizeAdd(IDC_CMB_FONT, DLSZ_MOVE_Y);

    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
