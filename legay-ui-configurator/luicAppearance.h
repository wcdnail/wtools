#pragma once

#include "luicPageImpl.h"

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
