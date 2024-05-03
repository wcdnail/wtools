#include "stdafx.h"
#include "shell.icons.dlg.h"

IMPLEMENT_DYNAMIC(CShellIconsDlg, CDialogEx)

BEGIN_MESSAGE_MAP(CShellIconsDlg, CDialogEx)
END_MESSAGE_MAP()

CShellIconsDlg::CShellIconsDlg(CWnd* parent /*=NULL*/)
	: CDialogEx(CShellIconsDlg::IDD, parent)
    , SmallIcons(true)
    , LargeIcons(false)
    , View()
    , Resizer()
{}

CShellIconsDlg::~CShellIconsDlg()
{}

void CShellIconsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LV_ICONS, View);
}

BOOL CShellIconsDlg::OnInitDialog()
{
    BOOL rv = CDialogEx::OnInitDialog();
    if (rv) {
        rv = Resizer.Hook(this);
        Resizer.SetAnchor(IDC_LV_ICONS, ANCHOR_VERTICALLY | ANCHOR_HORIZONTALLY);
        Resizer.SetAnchor(IDC_ST_STATUS, ANCHOR_BOTTOM | ANCHOR_HORIZONTALLY);
        Resizer.SetAnchor(IDCANCEL, ANCHOR_RIGHT | ANCHOR_BOTTOM);
    }

    CImageList smallIcons;
    CImageList largeIcons;

    smallIcons.Attach(SmallIcons.Get());
    largeIcons.Attach(LargeIcons.Get());

    View.SetImageList(&smallIcons, LVSIL_SMALL);
    View.SetImageList(&largeIcons, LVSIL_NORMAL);

    smallIcons.Detach();
    largeIcons.Detach();
    
    SelectViewMode(LV_VIEW_ICON, false);

    return rv;
}
