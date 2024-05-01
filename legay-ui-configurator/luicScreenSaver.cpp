#include "stdafx.h"
#include "luicScreenSaver.h"
#include "UT/debug.assistance.h"

CPageScreenSaver::~CPageScreenSaver()
{
}

CPageScreenSaver::CPageScreenSaver()
{
}

BOOL CPageScreenSaver::OnInitDlg(HWND, LPARAM)
{
    DlgResize_Init(false, false);
    return TRUE;
}
