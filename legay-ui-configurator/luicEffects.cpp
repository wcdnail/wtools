#include "stdafx.h"
#include "luicEffects.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageEffects::~CPageEffects()
{
}

CPageEffects::CPageEffects()
    : CPageImpl(IDD_PAGE_EFFECTS)
{
}

BOOL CPageEffects::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
