#pragma once

#include "shell.imagelist.h"
#include "mfc.resizable.window.h"
#include "res/resource.h"
#include <afxdialogex.h>
#include <afxcmn.h>

class CShellIconsDlg : public CDialogEx
{
public:
    enum { IDD = IDD_SHELL_ICONS };

	CShellIconsDlg(CWnd* parent = NULL);
	virtual ~CShellIconsDlg();

private:
    Sh::Imagelist SmallIcons;
    Sh::Imagelist LargeIcons;
    CListCtrl View;
    CWndResizer Resizer;

    void SelectViewMode(int mode, bool invalidate);

	virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_DYNAMIC(CShellIconsDlg)
	DECLARE_MESSAGE_MAP()
};

/*
 * LV_VIEW_DETAILS
 * LV_VIEW_ICON
 * LV_VIEW_LIST
 * LV_VIEW_SMALLICON
 * LV_VIEW_TILE
 * 
 */
inline void CShellIconsDlg::SelectViewMode(int mode, bool invalidate)
{
    View.SetView(mode);

    if (invalidate)
        View.Invalidate(FALSE);
}
