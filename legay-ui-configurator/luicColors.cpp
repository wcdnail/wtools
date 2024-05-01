#include "stdafx.h"
#include "luicColors.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageAppearance::~CPageAppearance()
{
}

CPageAppearance::CPageAppearance()
{
}

BOOL CPageAppearance::OnInitDlg(HWND, LPARAM)
{
    DlgResize_Init(false, false);
    return TRUE;
}
