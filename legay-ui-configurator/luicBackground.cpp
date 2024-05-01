#include "stdafx.h"
#include "luicBackground.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageBackground::~CPageBackground()
{
}

CPageBackground::CPageBackground()
    : CPageImpl(IDD_PAGE_BACKGROUND)
{
}

BOOL CPageBackground::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA2, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
