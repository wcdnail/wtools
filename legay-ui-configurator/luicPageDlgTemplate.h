#pragma once

#include "luicPageImpl.h"

struct CPageDlgTemplate: CPageImpl
{
    ~CPageDlgTemplate() override;
    CPageDlgTemplate();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
