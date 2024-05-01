#include "stdafx.h"
#include "luicEffects.h"
#include "UT/debug.assistance.h"

CPageEffects::~CPageEffects()
{
}

CPageEffects::CPageEffects()
{
}

BOOL CPageEffects::OnInitDlg(HWND, LPARAM)
{
    DlgResize_Init(false, false);
    return TRUE;
}
