#pragma once

#include "luicPageImpl.h"

struct CPageScreenSaver: CPageImpl
{
    ~CPageScreenSaver() override;
    CPageScreenSaver(std::wstring&& caption);

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
