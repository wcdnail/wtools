#pragma once

#include "luicPageImpl.h"
#include "resz/resource.h"

struct CPageSettings: CPageImpl
{
    ~CPageSettings() override;
    CPageSettings();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
