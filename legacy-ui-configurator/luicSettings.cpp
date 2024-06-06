#include "stdafx.h"
#include "luicSettings.h"
#include "resz/resource.h"
#include <dev.assistance/debug.assistance.h>

CPageSettings::~CPageSettings()
{
}

CPageSettings::CPageSettings(std::wstring&& caption)
    : CPageImpl(IDD_PAGE_SETTINGS, std::move(caption))
{
}

BOOL CPageSettings::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA6, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
