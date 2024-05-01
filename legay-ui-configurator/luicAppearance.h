#pragma once

#include "luicPageImpl.h"
#include <atlctrls.h>

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance();

private:
    WTL::CStatic m_Preview;

    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
