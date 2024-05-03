#include "stdafx.h"
#include "luicPageDllIcons.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

enum : int { LV_MakeInsert = 0 };

CPageDllIcons::~CPageDllIcons()
{
}

CPageDllIcons::CPageDllIcons()
    : CPageImpl(IDD_PAGE_DLL_FILE_ICONS)
{
}

void CPageDllIcons::Reset()
{
    m_lvView.DeleteAllItems();
    m_lvView.RemoveImageList(LVSIL_NORMAL);
    m_lvView.RemoveImageList(LVSIL_SMALL);
    m_ilSmall.Attach(nullptr);
    m_ilBig.Attach(nullptr);
    m_lvView.SetView(LV_VIEW_ICON);
    WTL::CButton(GetDlgItem(IDC_RADIO5)).SetCheck(FALSE);
    WTL::CButton(GetDlgItem(IDC_RADIO4)).SetCheck(FALSE);
    WTL::CButton(GetDlgItem(IDC_RADIO3)).SetCheck(FALSE);
    WTL::CButton(GetDlgItem(IDC_RADIO1)).SetCheck(TRUE);
}

void CPageDllIcons::SetError(HRESULT code, PCWSTR format, ...)
{
    Reset();
    {
        va_list ap;
        va_start(ap, format);
        Str::ElipsisW::String message = Str::ElipsisW::FormatV(format, ap);
        va_end(ap);
        m_edPath.SetWindowTextW(message.GetString());
    }
    {
        auto const* app = CLegacyUIConfigurator::App();
        m_lvView.SetImageList(app->GetImageList(), LVSIL_NORMAL);
        m_lvView.AddItem(0, LV_MakeInsert, Str::ErrorCode<>::SystemMessage(code).GetString(), IconHatchCross);
        WTL::CButton(GetDlgItem(IDC_RADIO1)).SetCheck(FALSE);
        WTL::CButton(GetDlgItem(IDC_RADIO5)).SetCheck(TRUE);
        m_lvView.SetView(LV_VIEW_TILE);
    }
}

void CPageDllIcons::OnCollectionLoad(CIconCollectionFile const& collection)
{
    Reset();

    m_bnBrowse.EnableWindow(FALSE);
    m_lvView.LockWindowUpdate(TRUE);

    try {
        do {
            WTL::CImageList tempIl = collection.MakeImageList(true);
            if (!tempIl.m_hImageList) {
                const auto code = static_cast<HRESULT>(GetLastError());
                SetError(code, L"ImageList creation from collection '%s' failed", collection.GetFilename().c_str());
                break;
            }
            WTL::CImageList tempIlSm = collection.MakeImageList(false);

            m_ilSmall.Attach(tempIlSm.Detach());
            m_ilBig.Attach(tempIl.Detach());

            m_lvView.SetImageList(m_ilBig, LVSIL_NORMAL);
            if (m_ilSmall.GetImageCount() > 0) {
                m_lvView.SetImageList(m_ilSmall, LVSIL_SMALL);
            }

            m_CurrFilename = collection.GetFilename();
            m_edPath.SetWindowTextW(m_CurrFilename.c_str());

            ATL::CStringW itemStr;
            for (int i = 0; i < m_ilBig.GetImageCount(); i++) {
                itemStr.Format(L"#%3d", i);
                m_lvView.AddItem(i, LV_MakeInsert, itemStr, i);
            }
        }
        while (false);
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

    m_lvView.LockWindowUpdate(FALSE);
    m_bnBrowse.EnableWindow(TRUE);
}

BOOL CPageDllIcons::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    auto const* app = CLegacyUIConfigurator::App();

    m_edPath.Attach(GetDlgItem(IDC_ED_OPEN_DLL_PATHNAME));
    m_bnBrowse.Attach(GetDlgItem(IDC_BN_OPEN_DLG));
    m_bnExport.Attach(GetDlgItem(IDC_BN_EXPORT_SEL));
    m_lvView.Attach(GetDlgItem(IDC_LV_VIEW));
    m_lvView.ModifyStyleEx(0, LVS_EX_DOUBLEBUFFER);
    m_bnBrowse.SetIcon(app->GetIcon(IconFolderOpen));
    m_bnExport.SetIcon(app->GetIcon(IconFloppy));

    OnCollectionLoad(app->ShellIcons());
    DragAcceptFiles(m_hWnd, TRUE);

    DlgResizeAdd(IDC_ED_OPEN_DLL_PATHNAME, DLSZ_SIZE_X);
    DlgResizeAdd(IDC_BN_OPEN_DLG, DLSZ_MOVE_X);
    DlgResizeAdd(IDC_BN_EXPORT_SEL, DLSZ_MOVE_X);
    DlgResizeAdd(IDC_LV_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

static HRESULT IFileDialog_GetDisplayName(IFileDialog& dlgImpl, std::wstring& target)
{
    ATL::CComPtr<IShellItem> pShellItem;
    HRESULT code = dlgImpl.GetResult(&pShellItem);
    if (FAILED(code)) {
        return code;
    }
    PWSTR lpstrName;
    code = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &lpstrName);
    if (FAILED(code)) {
        return code;
    }
    int len = lstrlenW(lpstrName);
    std::wstring{ lpstrName, static_cast<size_t>(len) }.swap(target);
    CoTaskMemFree(lpstrName);
    return S_OK;
}

void CPageDllIcons::AttemptToLoadNew(std::wstring const& filename)
{
    CIconCollectionFile tempFile;
    if (!tempFile.Load(filename.c_str(), true)) {
        auto code = static_cast<HRESULT>(GetLastError());
        SetError(code, L"Loading new collections from '%s' failed.", filename.c_str());
        return ;
    }
    OnCollectionLoad(tempFile);
}

void CPageDllIcons::AttemptToSaveSelected(std::wstring const& filename)
{

}

void CPageDllIcons::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
    DWORD viewType = LV_VIEW_ICON;
    switch (nID) {
    case IDC_BN_EXPORT_SEL: {
        WTL::CShellFileSaveDialog dlg{};
        const auto rv = dlg.DoModal(m_hWnd);
        if (IDOK != rv) {
            return ;
        }
        std::wstring tempFilename;
        HRESULT code = IFileDialog_GetDisplayName(*dlg.m_spFileDlg, tempFilename);
        if (FAILED(code)) {
            SetError(code, L"File dialog failed.");
            return ;
        }
        AttemptToSaveSelected(tempFilename);
        return;
    }
    case IDC_BN_OPEN_DLG: {
        WTL::CShellFileOpenDialog dlg{ m_CurrFilename.c_str() };
        const auto rv = dlg.DoModal(m_hWnd);
        if (IDOK != rv) {
            return ;
        }
        std::wstring tempFilename;
        HRESULT code = IFileDialog_GetDisplayName(*dlg.m_spFileDlg, tempFilename);
        if (FAILED(code)) {
            SetError(code, L"File dialog failed.");
            return ;
        }
        AttemptToLoadNew(tempFilename);
        return;
    }

    case IDC_RADIO1: viewType = LV_VIEW_ICON; break;
  //case IDC_RADIO2: viewType = LV_VIEW_DETAILS; break;
    case IDC_RADIO3: viewType = LV_VIEW_SMALLICON; break;
    case IDC_RADIO4: viewType = LV_VIEW_LIST; break;
    case IDC_RADIO5: viewType = LV_VIEW_TILE; break;
    default:
        CPageImpl::OnCommand(uNotifyCode, nID, wndCtl);
        return;
    }
    m_lvView.SetView(viewType);
}

void CPageDllIcons::OnResizeNotify()
{
    m_lvView.Arrange(LVA_ALIGNTOP);
}

namespace
{
struct HDropWrapper
{
    HDROP m_hDrop;
    HDropWrapper(HDROP hDrop)
        : m_hDrop { hDrop }
    {
    }
    ~HDropWrapper()
    {
        if (m_hDrop) {
            DragFinish(m_hDrop);
        }
    }
    static BOOL IsInClientRect(HDROP hDrop, HWND hWnd)
    {
        CPoint p;
        CRect rc;
        DragQueryPoint(hDrop, &p);
        GetClientRect(hWnd, rc);
        return PtInRect(&rc, p);
    }
    UINT FileCount(void) const
    {
        return DragQueryFileW(m_hDrop, static_cast<UINT>(-1), nullptr, 0);
    }

    ATL::CStringW QueryFile(UINT index)
    {
        ATL::CStringW temp;
        UINT dlen = MAX_PATH * 4;
        UINT qlen = DragQueryFileW(m_hDrop, index, temp.GetBufferSetLength(dlen), dlen - 1);
        temp.ReleaseBufferSetLength(qlen);
        return temp;
    }
};
}

void CPageDllIcons::OnDropFiles(HDROP hDropInfo)
{
    if (!HDropWrapper::IsInClientRect(hDropInfo, m_hWnd)) {
        return;
    }
    HDropWrapper drop(hDropInfo);
    UINT count = drop.FileCount();
    for (UINT i = 0; i < count; i++) {
        auto pathname = drop.QueryFile(i);
        if (!pathname.IsEmpty()) {
            AttemptToLoadNew(std::wstring{ pathname.GetString(), static_cast<size_t>(pathname.GetLength()) });
            break; // https://www.codeproject.com/Articles/6166/Dropping-Files-into-a-WTL-Window-The-Easy-Way
            // Вместо break; в оригинале было i = count + 10 )))
        }
    }
}
