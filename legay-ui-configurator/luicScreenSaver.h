#pragma once

#include "luicPageImpl.h"

struct CPageScreenSaver: CPageImpl
{
    ~CPageScreenSaver() override;
    CPageScreenSaver();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
