#include "stdafx.h"
#include "main.dialog.h"
#include "shell.helpers.h"
#include <string.utils.error.code.h>

#ifdef _DEBUG
#  define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(MainDialog, CDialogEx)
    ON_WM_QUERYDRAGICON()
          ON_BN_CLICKED(IDCANCEL,             &MainDialog::OnBnClickedCancel)
          ON_BN_CLICKED(IDOK,                     &MainDialog::OnBnClickedOk)
              ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &MainDialog::OnTreeDoubleClick)
END_MESSAGE_MAP()

MainDialog::MainDialog(CWnd* pParent /*=nullptr*/)
    :      CDialogEx(MainDialog::IDD, pParent)
    ,           Icon(AfxGetApp()->LoadIcon(IDR_MAINFRAME))
    , ShellImagelist()
    ,           Tree()
    ,        Folders()
    ,       OpStatus()
{}

MainDialog::~MainDialog()
{}

void MainDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE1, Tree);
    DDX_Control(pDX, IDC_OP_STATUS, OpStatus);
}

HCURSOR MainDialog::OnQueryDragIcon() { return static_cast<HCURSOR>(Icon); }
BOOL MainDialog::OnCommand(WPARAM wParam, LPARAM lParam) { return CDialogEx::OnCommand(wParam, lParam); }
void MainDialog::OnBnClickedCancel() { CDialogEx::OnCancel(); }
void MainDialog::OnBnClickedOk() { /*CDialogEx::OnOK();*/ }

BOOL MainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    ModifyStyle(0, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, SWP_FRAMECHANGED);

	SetIcon(Icon, TRUE);
	SetIcon(Icon, FALSE);

    HIMAGELIST shellImagelist = GetShellImageList();
    if (!shellImagelist) {
        SetOperationStatus(_T("GetShellImageList"), ::GetLastError());
        Tree.EnableWindow(FALSE);
    }
    else {
        ShellImagelist.Attach(shellImagelist);
        SetupTree();
        Start();
        Tree.SetFocus();
        
    }

	return TRUE;
}

void MainDialog::SetupTree()
{
    Tree.SetImageList(&ShellImagelist, TVSIL_NORMAL);
    //Tree.ModifyStyleEx(0, TVS_EX_DOUBLEBUFFER | TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DIMMEDCHECKBOXES);
    //Tree.ModifyStyle(TVS_HASLINES, TVS_FULLROWSELECT);
    Tree.ModifyStyle(TVS_FULLROWSELECT, TVS_HASLINES);
}

void MainDialog::SetOperationStatus(PCTSTR caption, HRESULT hr)
{
    CString message;
    message.Format(_T("%s [0x%x `%s`]"), caption, hr, Str::ErrorCode<TCHAR>::SystemMessage(hr));
    OpStatus.SetWindowText(message);
}

void MainDialog::Start()
{
    Tree.DeleteAllItems();
    Folders.RemoveAll();

    CComPtr<IShellFolder> folder;
    HRESULT hr = ::SHGetDesktopFolder(&folder);
    if (folder)
    {
        CComHeapPtr<ITEMIDLIST> idList(nullptr);
        hr = ::SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &idList);
        if (idList)
        {
            HTREEITEM root = AppendToTree(folder, idList, TVI_ROOT);
            hr = PopulateTree(folder, root);
            Tree.Expand(root, TVE_EXPAND);
        }
    }

    SetOperationStatus(_T("MainDialog::Start"), hr);
}

HTREEITEM MainDialog::AppendToTree(HTREEITEM rootItem, Info const& info) const
{
    TVINSERTSTRUCTW it = {0};

    it.hParent = rootItem;
    it.hInsertAfter = TVI_SORT; //info.IsDir() ? TVI_FIRST : TVI_LAST;
    it.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    it.item.pszText = (PWSTR)(PCWSTR)info.name;
    it.item.cchTextMax = info.name.GetLength();
    it.item.iImage = info.icon;
    it.item.iSelectedImage = info.selectedIcon;

    return Tree.InsertItem(&it);
}

MainDialog::Info::Info()
{}

static int ShGetIcon(MainDialog::HeapPtr const& idList, UINT flags)
{
    SHFILEINFO info = {0};
    ::SHGetFileInfoW((PCWSTR)idList.m_pData, -1, &info, sizeof(info), SHGFI_PIDL | SHGFI_LINKOVERLAY | flags);
    return info.iIcon;
}

static CStringW ShGetPath(MainDialog::HeapPtr const& idList)
{
    CStringW path;
    ::SHGetPathFromIDListW(idList, path.GetBuffer(4096));
    path.ReleaseBuffer();

    return path;
}

static CStringW ShGetName(MainDialog::HeapPtr const& idList)
{
    SHFILEINFO info = {0};
    ::SHGetFileInfoW((PCWSTR)idList.m_pData, -1, &info, sizeof(info), SHGFI_PIDL | SHGFI_DISPLAYNAME);
    return CStringW(info.szDisplayName);
}

bool MainDialog::Info::IsDir() const
{
    return (SFGAO_FOLDER == (SFGAO_FOLDER & attributes));
}

HRESULT MainDialog::Info::Get(IFolderPtr const& folder, HeapPtr const& idList)
{
    path.Empty();
    name.Empty();
    icon = selectedIcon = 0;
    attributes = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_FILESYSTEM | SFGAO_SHARE | SFGAO_REMOVABLE | SFGAO_COMPRESSED | SFGAO_BROWSABLE | SFGAO_STREAM;

    folder->GetAttributesOf(1, (LPCITEMIDLIST*)&idList.m_pData, &attributes);
    
    name = ShGetName(idList);
    if (name.IsEmpty())
    {
        STRRET tempName = {0};
        HRESULT hr = folder->GetDisplayNameOf(idList, SHGDN_INFOLDER, &tempName);
        if (SUCCEEDED(hr))
            name = tempName.pOleStr;

        //IFolderPtr current;
        //hr = folder->BindToObject(idList, nullptr, IID_IShellFolder, (void**)&current);
        //if (SUCCEEDED(folder->BindToObject(idList, )))
    }

    path = ShGetPath(idList);
    icon = ShGetIcon(idList, SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    selectedIcon = ShGetIcon(idList, SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);

#ifdef _DEBUG
    name += CStringW(L" [") + ShAttributesToString(attributes) + L"]";

    if (!path.IsEmpty())
        name += CStringW(L" `") + path + L"`";
#endif 

    return S_OK;
}

HTREEITEM MainDialog::AppendToTree(IFolderPtr& folder, HeapPtr& idList, HTREEITEM rootItem) const
{
    Info info;
    info.Get(folder, idList);

    HTREEITEM item = AppendToTree(rootItem, info);
    if (nullptr != item)
    {
        IFolderPtr current;
        HRESULT hr = folder->BindToObject(idList, nullptr, IID_IShellFolder, (void**)&current);

        if (SUCCEEDED(hr))
            Folders[item] = current;
    }

    return item;
}

HRESULT MainDialog::PopulateTree(IFolderPtr& folder, HTREEITEM rootItem)
{
    CWaitCursor wc;

    if (!folder) {
        return E_INVALIDARG;
    }

    CComPtr<IEnumIDList> enumeration;
    HRESULT hr = folder->EnumObjects(nullptr, SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN 
                                   | SHCONTF_NONFOLDERS | SHCONTF_STORAGE 
                                   | SHCONTF_INCLUDESUPERHIDDEN
                                   , &enumeration);
    if (!enumeration)
        return hr;

    DWORD fetched = 1;
    HeapPtr idl(nullptr);
    while (SUCCEEDED(enumeration->Next(1, &idl, &fetched)) && fetched)
    {
        AppendToTree(folder, idl, rootItem);
        idl.Free();
    }

    return S_OK;
}

static int GetChildCount(CTreeCtrl& tree, HTREEITEM item)
{
    int count = 0;

    if (tree.ItemHasChildren(item))
    {
        item = tree.GetNextItem(item, TVGN_CHILD);
        while (item)
        {
            ++count;
            item = tree.GetNextItem(item, TVGN_NEXT);
        }
    }

    return count;
}

void MainDialog::OnTreeDoubleClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM item = Tree.GetSelectedItem();
    if (nullptr != item && (GetChildCount(Tree, item) < 1))
    {
        auto it = Folders.PLookup(item);
        if (nullptr != it) {
            *pResult = 1;

            HRESULT hr = PopulateTree(it->value, item);
            SetOperationStatus(_T("PopulateTree"), hr);
            if (SUCCEEDED(hr)) {
                Tree.Expand(item, TVE_EXPAND);
            }
        }
    }
    else {
        *pResult = 0;
    }
}
