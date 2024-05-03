#include "stdafx.h"
#include "luicPageDllIcons.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CPageDllIcons::~CPageDllIcons()
{
}

CPageDllIcons::CPageDllIcons()
    : CPageImpl(IDD_PAGE_DLL_FILE_ICONS)
{
}

void CPageDllIcons::OnFileLoaded(CIconCollectionFile const& collection)
{
    m_bnBrowse.EnableWindow(FALSE);
    m_lbView.LockWindowUpdate(TRUE);

    try {
        if (collection.GetFilename().empty()) {
            m_edPath.SetWindowTextW(L"error loading icons...");
        }
        else {
            m_edPath.SetWindowTextW(collection.GetFilename().c_str());
        }

    }
    catch (std::exception const& ex) {
        auto code = static_cast<HRESULT>(GetLastError());
        if (ERROR_SUCCESS == code) {
            code = ERROR_INVALID_FUNCTION;
        }
        ReportError(Str::ElipsisA::Format("Load icon view '%s' failed! %s",
            collection.GetFilename(), ex.what()
        ), code, false, MB_ICONWARNING);
    }

    m_lbView.LockWindowUpdate(FALSE);
    m_bnBrowse.EnableWindow(TRUE);
}

BOOL CPageDllIcons::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    auto const* app = CLegacyUIConfigurator::App();

    m_edPath.Attach(GetDlgItem(IDC_ED_OPEN_DLL_PATHNAME));
    m_bnBrowse.Attach(GetDlgItem(IDC_BN_OPEN_DLG));
    m_lbView.Attach(GetDlgItem(IDC_LB_VIEW));

    m_bnBrowse.SetIcon(app->GetIcon(IconFolderOpen));
    OnFileLoaded(app->ShellIcons());

    DlgResizeAdd(IDC_ED_OPEN_DLL_PATHNAME, DLSZ_SIZE_X);
    DlgResizeAdd(IDC_BN_OPEN_DLG, DLSZ_MOVE_X);
    DlgResizeAdd(IDC_LB_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}
