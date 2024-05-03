#pragma once

#include "res/resource.h"
#include <afxwin.h>
#include <afxcmn.h>
#include <afxstr.h>
#include <afxcoll.h>
#include <afxdialogex.h>
#include <atlcom.h>
#include <shobjidl.h>

class MainDialog : public CDialogEx
{
public:
    enum { IDD = IDD_MFCTESTS_DIALOG };

    typedef CComPtr<IShellFolder> IFolderPtr;
    typedef CComHeapPtr<ITEMIDLIST> HeapPtr;

    struct Info
    {
        DWORD attributes;
        CStringW path;
        CStringW name;
        int icon;
        int selectedIcon;

        Info();
        HRESULT Get(IFolderPtr const& folder, HeapPtr const& idList);
        bool IsDir() const;
    };

	MainDialog(CWnd* pParent = NULL);
    virtual ~MainDialog();

    HRESULT PopulateTree(IFolderPtr& folder, HTREEITEM rootItem);

private:
    typedef CMap<HTREEITEM, HTREEITEM, IFolderPtr, IFolderPtr&> ShellFolderMap;

    HICON Icon;
    CImageList ShellImagelist;
    mutable CTreeCtrl Tree;
    mutable ShellFolderMap Folders;
    mutable CStatic OpStatus;

    void SetupTree();
    void SetOperationStatus(PCTSTR caption, HRESULT hr);
    void Start();
    HTREEITEM AppendToTree(HTREEITEM rootItem, Info const& info) const;
    HTREEITEM AppendToTree(IFolderPtr& folder, HeapPtr& idList, HTREEITEM rootItem) const;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
    afx_msg void OnTreeDoubleClick(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};
