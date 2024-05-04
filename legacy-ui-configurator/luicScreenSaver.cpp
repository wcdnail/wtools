#include "stdafx.h"
#include "luicScreenSaver.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageScreenSaver::~CPageScreenSaver()
{
}

CPageScreenSaver::CPageScreenSaver(std::wstring&& caption)
    : CPageImpl(IDD_PAGE_SCREENSAVER, std::move(caption))
{
}

BOOL CPageScreenSaver::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA3, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}