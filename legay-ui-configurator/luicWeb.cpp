#include "stdafx.h"
#include "luicWeb.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageWeb::~CPageWeb()
{
}

CPageWeb::CPageWeb()
    : CPageImpl(IDD_PAGE_WEB)
{
}

BOOL CPageWeb::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
