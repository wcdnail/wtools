#pragma once

#include "luicPageImpl.h"
#include "luicThemePreviewer.h"
#include <atlctrls.h>

struct CTheme;

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance();

private:
    friend struct CTheme;

    CThemePreviewer     m_Preview;
    WTL::CComboBox     m_ThemeSel;
    WTL::CComboBox m_ThemeSizeSel;
    WTL::CButton      m_ThemeSave;
    WTL::CButton    m_ThemeImport;
    WTL::CButton    m_ThemeRename;
    WTL::CButton    m_ThemeDelete;
    WTL::CComboBox   m_ElementSel;

    void InitResizeMap();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;
};
