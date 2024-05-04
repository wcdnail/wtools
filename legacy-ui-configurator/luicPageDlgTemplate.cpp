#include "stdafx.h"
#include "luicPageDlgTemplate.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageDlgTemplate::~CPageDlgTemplate()
{
}

CPageDlgTemplate::CPageDlgTemplate(std::wstring&& caption)
    : CPageImpl{ 0, std::move(caption) }
{
}

BOOL CPageDlgTemplate::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
