#pragma once

#include "luicPageImpl.h"

struct CPageWeb: CPageImpl
{
    ~CPageWeb() override;
    CPageWeb(std::wstring&& caption);

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
