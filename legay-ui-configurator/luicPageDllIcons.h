#pragma once

#include "luicPageImpl.h"
#include <atlctrls.h>

class CIconCollectionFile;

struct CPageDllIcons: CPageImpl
{
    ~CPageDllIcons() override;
    CPageDllIcons();

private:
    WTL::CEdit        m_edPath;
    WTL::CButton    m_bnBrowse;
    WTL::CDragListBox m_lbView;

    void OnFileLoaded(CIconCollectionFile const& collection);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
};
