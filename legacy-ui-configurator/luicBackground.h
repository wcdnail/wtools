#pragma once

#include "luicPageImpl.h"

struct CPageBackground: CPageImpl
{
    ~CPageBackground() override;
    CPageBackground(std::wstring&& caption);

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
