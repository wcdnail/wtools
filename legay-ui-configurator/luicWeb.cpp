#include "stdafx.h"
#include "luicWeb.h"
#include "UT/debug.assistance.h"

CPageWeb::~CPageWeb()
{
}

CPageWeb::CPageWeb()
{
}

BOOL CPageWeb::OnInitDlg(HWND, LPARAM)
{
    DlgResize_Init(false, false);
    return TRUE;
}
