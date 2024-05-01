#pragma once

#include "luicPageImpl.h"

struct CPageEffects: CPageImpl
{
    ~CPageEffects() override;
    CPageEffects();

private:
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
