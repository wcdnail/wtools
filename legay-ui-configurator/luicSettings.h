#pragma once

#include "luicPageImpl.h"

struct CPageSettings: CPageImpl
{
    ~CPageSettings() override;
    CPageSettings();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
