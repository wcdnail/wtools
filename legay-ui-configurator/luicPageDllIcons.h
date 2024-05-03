#pragma once

#include "luicPageImpl.h"
#include <atlctrls.h>
#include <string>

class CIconCollectionFile;

struct CPageDllIcons: CPageImpl
{
    using ImageList = WTL::CImageListManaged;

    ~CPageDllIcons() override;
    CPageDllIcons();

private:
    WTL::CEdit         m_edPath;
    WTL::CButton     m_bnBrowse;
    WTL::CButton     m_bnExport;
    WTL::CListViewCtrl m_lvView;
    ImageList           m_ilBig;
    ImageList         m_ilSmall;
    std::wstring m_CurrFilename;

    void OnDestroy() override;
    void DetachImageLists();
    void Reset();
    void ResetView();
    void SetError(HRESULT code, PCWSTR format, ...);
    void OnCollectionLoad(CIconCollectionFile const& collection);
    void PopulateViews();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    void OnResizeNotify() override;
    void OnDropFiles(HDROP hDropInfo) override;
    void AttemptToLoadNew(std::wstring const& filename);
    void ExportMultiple(UINT count);
    void ExportSinle();
    bool ExportIconOLE(int it, bool needBig, std::wstring const& filename); // OK, 4bpp
    bool ExportIconGDIP(int it, bool needBig, std::wstring const& filename); // error, need investigate
    bool ExportIconOLE2(int it, bool needBig, std::wstring const& filename);  // OK, 4bpp
    bool ExportIconPLAIN(int it, bool needBig, std::wstring const& filename);  // OK
    void AttemptToSaveSelected(std::wstring const& filename, UINT count);
};
