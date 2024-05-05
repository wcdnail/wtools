#pragma once

#include "luicPageImpl.h"
#include "luicThemePreviewer.h"
#include <atlctrls.h>

struct CTheme;

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance(std::wstring&& caption);

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
    WTL::CComboBox      m_FontSel;

    void InitResizeMap();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;
    void OnSelectFont(LOGFONT const* lfFont);
    void OnSelectTheme(int nThemeIndex);
    void OnSelectItem(int nItem);
};
