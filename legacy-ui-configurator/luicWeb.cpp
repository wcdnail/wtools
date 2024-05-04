#include "stdafx.h"
#include "luicWeb.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageWeb::~CPageWeb()
{
}

CPageWeb::CPageWeb(std::wstring&& caption)
    : CPageImpl(IDD_PAGE_WEB, std::move(caption))
{
}

BOOL CPageWeb::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA5, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
