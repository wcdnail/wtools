#include "stdafx.h"
#include "luicEffects.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageEffects::~CPageEffects()
{
}

CPageEffects::CPageEffects()
    : CPageImpl(IDD_PAGE_EFFECTS)
{
}

BOOL CPageEffects::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA4, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
