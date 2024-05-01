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
    DlgResizeAdd(IDC_TEST_STA1, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
