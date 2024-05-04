#pragma once

#include "luicPageImpl.h"

struct CPageSettings: CPageImpl
{
    ~CPageSettings() override;
    CPageSettings(std::wstring&& caption);

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
