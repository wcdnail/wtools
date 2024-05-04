#pragma once

#include "luicPageImpl.h"

struct CPageEffects: CPageImpl
{
    ~CPageEffects() override;
    CPageEffects(std::wstring&& caption);

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
