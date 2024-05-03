#pragma once

#include "luicPageImpl.h"
#include <atlctrls.h>
#include <string>

class CIconCollectionFile;

struct CPageDllIcons: CPageImpl
{
    ~CPageDllIcons() override;
    CPageDllIcons();

private:
    WTL::CEdit         m_edPath;
    WTL::CButton     m_bnBrowse;
    WTL::CButton     m_bnExport;
    WTL::CListViewCtrl m_lvView;
    WTL::CImageList     m_ilBig;
    WTL::CImageList   m_ilSmall;
    std::wstring m_CurrFilename;

    void Reset();
    void SetError(HRESULT code, PCWSTR format, ...);
    void AttemptToLoadNew(std::wstring const& filename);
    void ExportMultiple(UINT count);
    void ExportSinle();
    bool ExportIconOLE(int it, bool needBig, std::wstring const& filename);
    bool ExportIconGDIP(int it, bool needBig, std::wstring const& filename);
    bool ExportIconOLE2(int it, bool needBig, std::wstring const& filename);
    bool ExportIconPLAIN(int it, bool needBig, std::wstring const& filename);
    void AttemptToSaveSelected(std::wstring const& filename, UINT count);
    void OnCollectionLoad(CIconCollectionFile const& collection);
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    void OnResizeNotify() override;
    void OnDropFiles(HDROP hDropInfo) override;
};
