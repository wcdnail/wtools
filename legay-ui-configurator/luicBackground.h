#pragma once

#include "luicPageImpl.h"
#include "resz/resource.h"

struct CPageBackground: CPageImpl
{
    ~CPageBackground() override;
    CPageBackground();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
