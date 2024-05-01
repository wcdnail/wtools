#include "stdafx.h"
#include "luicPageDlgTemplate.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageDlgTemplate::~CPageDlgTemplate()
{
}

CPageDlgTemplate::CPageDlgTemplate()
    : CPageImpl(0)
{
}

BOOL CPageDlgTemplate::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
