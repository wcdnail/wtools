#pragma once

#include "luicPageImpl.h"

struct CPageWeb: CPageImpl
{
    ~CPageWeb() override;
    CPageWeb();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
