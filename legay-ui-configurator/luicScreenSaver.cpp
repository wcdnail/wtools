#include "stdafx.h"
#include "luicScreenSaver.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageScreenSaver::~CPageScreenSaver()
{
}

CPageScreenSaver::CPageScreenSaver()
    : CPageImpl(IDD_PAGE_SCREENSAVER)
{
}

BOOL CPageScreenSaver::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
