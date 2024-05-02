#pragma once

#include "luicPageImpl.h"
#include "luicThemePreviewer.h"
#include <atlctrls.h>

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance();

private:
    CThemePreviewer m_Preview;

    void InitResizeMap();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;
};
