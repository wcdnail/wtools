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
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
