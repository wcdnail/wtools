#include "stdafx.h"
#include "luicEffects.h"
#include "resz/resource.h"
#include <dev.assistance/debug.assistance.h>

CPageEffects::~CPageEffects()
{
}

CPageEffects::CPageEffects(std::wstring&& caption)
    : CPageImpl(IDD_PAGE_EFFECTS, std::move(caption))
{
}

BOOL CPageEffects::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    DlgResizeAdd(IDC_TEST_STA4, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
