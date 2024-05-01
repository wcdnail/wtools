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
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
