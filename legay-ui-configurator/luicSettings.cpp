#include "stdafx.h"
#include "luicSettings.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageSettings::~CPageSettings()
{
}

CPageSettings::CPageSettings()
    : CPageImpl(IDD_PAGE_SETTINGS)
{
}

BOOL CPageSettings::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA6, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}