#pragma once

#include "luicPageImpl.h"

struct CPageDlgTemplate: CPageImpl
{
    ~CPageDlgTemplate() override;
    CPageDlgTemplate(std::wstring&& caption);

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
