#include "stdafx.h"
#include "luicColors.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CColorsPage::~CColorsPage()
{
}

CColorsPage::CColorsPage()
{
}

BOOL CColorsPage::OnInitDlg(HWND, LPARAM)
{
    DlgResize_Init(false, false);
    return TRUE;
}
