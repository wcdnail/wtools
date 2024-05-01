#include "stdafx.h"
#include "luicPageImpl.h"
#include "UT/debug.assistance.h"

CPageImpl::~CPageImpl()
{
}

CPageImpl::CPageImpl(UINT idd)
    : IDD(idd)
{
}

BOOL CPageImpl::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);
    DlgResize_Init(false, false);
    return TRUE;
}
