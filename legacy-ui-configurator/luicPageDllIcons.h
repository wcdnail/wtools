#pragma once

#include "luicPageImpl.h"
#include <atlctrls.h>
#include <string>

class CIconCollectionFile;

struct CPageDllIcons: CPageImpl
{
    ~CPageDllIcons() override;
    CPageDllIcons(std::wstring&& caption);

private:
    WTL::CEdit         m_edPath;
    WTL::CButton     m_bnBrowse;
    WTL::CButton     m_bnExport;
    WTL::CListViewCtrl m_lvView;
    WTL::CImageList   m_il32x32;
    WTL::CImageList   m_il16x16;
    std::wstring m_CurrFilename;
    bool           m_bManagedIl;

    void OnDestroy() override;
    void SelectAll() override;
    void DetachImageLists();
    void Reset();
    void ResetView();
    void SetError(HRESULT code, PCWSTR format, ...);
    void OnCollectionLoad(CIconCollectionFile& collection);
    void PopulateViews();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    void OnResizeNotify() override;
    void OnDropFiles(HDROP hDropInfo) override;
    void AttemptToLoadNew(std::wstring const& filename);
    void ExportMultiple(UINT count);
    void ExportSingle();
    bool ExportIconOLE(int it, bool needBig, std::wstring const& filename); // OK, 4bpp
    bool ExportIconGDIP(int it, bool needBig, std::wstring const& filename); // error, need investigate
    bool ExportIconOLE2(int it, bool needBig, std::wstring const& filename);  // OK, 4bpp
    bool ExportIconPLAIN(int it, bool needBig, std::wstring const& filename);  // OK
    void AttemptToSaveSelected(std::wstring const& filename, UINT count);
};
