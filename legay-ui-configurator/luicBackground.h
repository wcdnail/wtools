#pragma once

#include "luicPageImpl.h"

struct CPageBackground: CPageImpl
{
    ~CPageBackground() override;
    CPageBackground();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
