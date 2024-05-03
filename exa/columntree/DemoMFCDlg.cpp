// DemoMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DemoMFC.h"
#include "DemoMFCDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDemoMFCDlg dialog




CDemoMFCDlg::CDemoMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemoMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLUMNTREE, m_columnTree);
	DDX_Control(pDX, IDC_CHKICONS, m_chkIcons);
	DDX_Control(pDX, IDC_CHKLARGRICONS, m_chkLargeIcons);
	DDX_Control(pDX, IDC_CHKHASLINES, m_chkHasLines);
	DDX_Control(pDX, IDC_CHKHASCHKBOXES, m_chkHasCheckBoxes);
	DDX_Control(pDX, IDC_CHKFULLROWSELECT, m_chkFullRowSelect);
	DDX_Control(pDX, IDC_CHKHASBUTTONS, m_chkHasButtons);
}

BEGIN_MESSAGE_MAP(CDemoMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHKICONS, &CDemoMFCDlg::OnBnClickedChkicons)
	ON_BN_CLICKED(IDC_CHKLARGRICONS, &CDemoMFCDlg::OnBnClickedChklargricons)
	ON_BN_CLICKED(IDC_CHKHASLINES, &CDemoMFCDlg::OnBnClickedChkhaslines)
	ON_BN_CLICKED(IDC_CHKHASCHKBOXES, &CDemoMFCDlg::OnBnClickedChkhaschkboxes)
	ON_BN_CLICKED(IDC_CHKFULLROWSELECT, &CDemoMFCDlg::OnBnClickedChkfullrowselect)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHKHASBUTTONS, &CDemoMFCDlg::OnBnClickedChkhasbuttons)
	ON_NOTIFY(NM_RCLICK, IDC_COLUMNTREE, &CDemoMFCDlg::OnRclickedColumntree)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CDemoMFCDlg)
    EASYSIZE(IDC_COLUMNTREE,ES_BORDER,ES_BORDER, ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CDemoMFCDlg message handlers

BOOL CDemoMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	INIT_EASYSIZE;

	// set style for tree view
	UINT uTreeStyle = TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_CHECKBOXES;
	m_columnTree.GetTreeCtrl().ModifyStyle(0,uTreeStyle);
	
	InitOptions();
	InitTree();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDemoMFCDlg::InitOptions()
{
	m_chkIcons.SetCheck(TRUE);
	m_chkHasButtons.SetCheck(TRUE);
	m_chkHasLines.SetCheck(TRUE);
	m_chkHasCheckBoxes.SetCheck(TRUE);

}

void CDemoMFCDlg::InitTree()
{
	/*
	 * Set background image (works with owner-drawn tree only)
	 */

	LVBKIMAGE bk;
	bk.xOffsetPercent = bk.yOffsetPercent = 70;
	bk.hbm = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BKGND));
	m_columnTree.GetTreeCtrl().SetBkImage(&bk);
    
	/*
	 * Create image list for tree & load icons
	 */

    m_imgList.Create (16, 16, ILC_COLOR32|ILC_MASK,5,1);
	
	int nIconFloppy = m_imgList.Add(AfxGetApp()->LoadIcon(IDI_FLOPPY));
	int nIconFixedDisk = m_imgList.Add(AfxGetApp()->LoadIcon(IDI_FIXEDDISK));
	int nIconHardDisk = m_imgList.Add(AfxGetApp()->LoadIcon(IDI_HARDDISK));
	int nIconCDROM = m_imgList.Add(AfxGetApp()->LoadIcon(IDI_CDROM));
	int nIconNetworkPlace = m_imgList.Add(AfxGetApp()->LoadIcon(IDI_NETWORKPLACE));
	int nIconOverlay = m_imgList.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY1));
	m_imgList.SetOverlayImage(nIconOverlay,1);
	
	// assign image list to tree control
	m_columnTree.GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
		
	/*
	 *  Insert columns to tree control
	 */

	m_columnTree.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 180);
	m_columnTree.InsertColumn(1, _T("Type"), LVCFMT_LEFT, 80);
	m_columnTree.InsertColumn(2, _T("File System"), LVCFMT_CENTER, 80);
	m_columnTree.InsertColumn(3, _T("Size"), LVCFMT_RIGHT, 70);
	m_columnTree.InsertColumn(4, _T("Free"), LVCFMT_RIGHT, 70);

	/* 
	 *  Insert items
	 */

	HTREEITEM hRoot,hItem;
	
	hRoot = m_columnTree.GetTreeCtrl().InsertItem(_T("3.5\" Floppy (A:)"),
		nIconFloppy,nIconFloppy);
	m_columnTree.SetItemText(hRoot,1,_T("Removable"));
	m_columnTree.SetItemText(hRoot,2,_T("FAT12"));
	m_columnTree.SetItemText(hRoot,3,_T("1.44 Mb"));
	m_columnTree.SetItemText(hRoot,4,_T("100 Kb"));

	hRoot = m_columnTree.GetTreeCtrl().InsertItem(_T("S312026AS"),
		nIconHardDisk,nIconHardDisk);
	m_columnTree.SetItemText(hRoot,1,_T("Hard Disk"));
	
	hItem = m_columnTree.GetTreeCtrl().InsertItem(_T("Local Disk (C:)"),
		nIconFixedDisk,nIconFixedDisk,hRoot);
	m_columnTree.SetItemText(hItem,1,_T("Fixed Disk"));
	m_columnTree.SetItemText(hItem,2,_T("NTFS"));
	m_columnTree.SetItemText(hItem,3,_T("200 Gb"));
	m_columnTree.SetItemText(hItem,4,_T("150 Gb"));

	hItem = m_columnTree.GetTreeCtrl().InsertItem(_T("Local Disk (E:)"),
		nIconFixedDisk,nIconFixedDisk,hRoot);
	m_columnTree.SetItemText(hItem,1,_T("Fixed"));
	m_columnTree.SetItemText(hItem,2,_T("NTFS"));
	m_columnTree.SetItemText(hItem,3,_T("128 Gb"));
	m_columnTree.SetItemText(hItem,4,_T("120 Gb"));

	m_columnTree.GetTreeCtrl().Expand(hRoot,TVE_EXPAND);

	hRoot = m_columnTree.GetTreeCtrl().InsertItem(_T("Maxtor DiamondMax 92048U8"),
		nIconHardDisk,nIconHardDisk);
	m_columnTree.SetItemText(hRoot,1,_T("Hard Disk"));
	
	hItem = m_columnTree.GetTreeCtrl().InsertItem(_T("Work (D:)"),
		nIconFixedDisk,nIconFixedDisk,hRoot);
	m_columnTree.SetItemText(hItem,1,_T("Fixed Disk"));
	m_columnTree.SetItemText(hItem,2,_T("FAT32"));
	m_columnTree.SetItemText(hItem,3,_T("40 Gb"));
	m_columnTree.SetItemText(hItem,4,_T("12 Gb"));

	m_columnTree.GetTreeCtrl().Expand(hRoot,TVE_EXPAND);

	hRoot = m_columnTree.GetTreeCtrl().InsertItem(_T("_NEC CD-RW NR9400-A"),
		nIconHardDisk,nIconHardDisk);
	m_columnTree.SetItemText(hRoot,1,_T("CD-ROM"));

	hItem = m_columnTree.GetTreeCtrl().InsertItem(_T("My Music (F:)"),
		nIconCDROM,nIconCDROM,hRoot);
	m_columnTree.SetItemText(hItem,1,_T("CD-Rewritable"));
	m_columnTree.SetItemText(hItem,2,_T("CDFS"));
	m_columnTree.SetItemText(hItem,3,_T("700 Mb"));
	m_columnTree.SetItemText(hItem,4,_T("0 Kb"));

	m_columnTree.GetTreeCtrl().Expand(hRoot,TVE_EXPAND);
	
	hRoot = m_columnTree.GetTreeCtrl().InsertItem(_T("_NEC DVD-RW ND3550-A"),
		nIconHardDisk,nIconHardDisk);
	m_columnTree.SetItemText(hRoot,1,_T("CD-ROM"));

	hItem = m_columnTree.GetTreeCtrl().InsertItem(_T("DVD-ROM Drive (G:)"),
		nIconCDROM, nIconCDROM, hRoot);
	m_columnTree.SetItemText(hItem,1,_T("DVD-RW"));
	
	m_columnTree.GetTreeCtrl().Expand(hRoot,TVE_EXPAND);

	hItem = m_columnTree.GetTreeCtrl().InsertItem(_T("Music on Multimedia"),
		nIconNetworkPlace,nIconNetworkPlace,TVI_ROOT);
	m_columnTree.SetItemText(hItem,1,_T("Network Place"));
	
	m_columnTree.GetTreeCtrl().SetItemState(hItem, 
		TVIS_OVERLAYMASK, INDEXTOOVERLAYMASK(1));

}

void CDemoMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDemoMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDemoMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDemoMFCDlg::OnBnClickedChkicons()
{
	// TODO: Add your control notification handler code here
	BOOL bIcons = m_chkIcons.GetCheck();
	if(!bIcons)
		m_columnTree.GetTreeCtrl().SetImageList(NULL,TVSIL_NORMAL);
	else
		m_columnTree.GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
}

void CDemoMFCDlg::OnBnClickedChklargricons()
{
	// TODO: Add your control notification handler code here
	BOOL bCheckBoxes = m_chkHasCheckBoxes.GetCheck();
	m_columnTree.GetTreeCtrl().SetImageList(NULL,TVSIL_STATE);
	BOOL b = m_chkLargeIcons.GetCheck();
	if(m_imgList)
	{
		m_columnTree.GetTreeCtrl().SetImageList(NULL,TVSIL_NORMAL);
		m_imgList.DeleteImageList();
	}
	//m_columnTree.GetTreeCtrl().ModifyStyle(bCheckBoxes?0:TVS_CHECKBOXES,bCheckBoxes?TVS_CHECKBOXES:0);
	m_imgList.Create (b?32:16,b?32:16, ILC_COLOR32|ILC_MASK,5,1);
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_FLOPPY));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_FIXEDDISK));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_HARDDISK));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_CDROM));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_NETWORKPLACE));
	m_columnTree.GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
	
	BOOL bHasLines = m_chkHasLines.GetCheck();
	
	// ensure changes have effect
	m_columnTree.SetWindowPos(0, 0, 0, 0, 0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

}

void CDemoMFCDlg::OnBnClickedChkhaslines()
{
	// TODO: Add your control notification handler code here
	BOOL b = m_chkHasLines.GetCheck();
	m_columnTree.GetTreeCtrl().ModifyStyle(b?0:TVS_HASLINES, b?TVS_HASLINES:0);
}

void CDemoMFCDlg::OnBnClickedChkhaschkboxes()
{
	// TODO: Add your control notification handler code here
	BOOL b = m_chkHasCheckBoxes.GetCheck();
	m_columnTree.GetTreeCtrl().ModifyStyle(b?0:TVS_CHECKBOXES, b?TVS_CHECKBOXES:0);
	// ensure changes have effect
	m_columnTree.Invalidate();
}


void CDemoMFCDlg::OnBnClickedChkfullrowselect()
{
	// TODO: Add your control notification handler code here
	BOOL b = m_chkFullRowSelect.GetCheck();
	m_columnTree.GetTreeCtrl().ModifyStyle(b?0:TVS_FULLROWSELECT, b?TVS_FULLROWSELECT:0);
}

void CDemoMFCDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	UPDATE_EASYSIZE;

}

void CDemoMFCDlg::OnBnClickedChkhasbuttons()
{
	// TODO: Add your control notification handler code here
	BOOL b = m_chkHasButtons.GetCheck();
	m_columnTree.GetTreeCtrl().ModifyStyle(b?0:TVS_HASBUTTONS, b?TVS_HASBUTTONS:0);
}

void CDemoMFCDlg::OnRclickedColumntree(LPNMHDR pNMHDR, LRESULT* pResult)
{
	CPoint pt;
	::GetCursorPos(&pt);

	m_columnTree.ScreenToClient(&pt);


	CTVHITTESTINFO htinfo;
	htinfo.pt = pt;
	HTREEITEM hItem = m_columnTree.HitTest(&htinfo);

	if(hItem)
	{
		CString szState;

		if(htinfo.flags&TVHT_ONITEMBUTTON)
			szState += _T("Clicked on item's button.");

		if(htinfo.flags&TVHT_ONITEMICON)
			szState += _T("Clicked on item's icon.");

		if(htinfo.flags&TVHT_ONITEMSTATEICON)
			szState += _T("Clicked on item's state icon.");

		if(htinfo.flags&TVHT_ONITEMINDENT)
			szState += _T("Clicked on item's indent.");

		if(htinfo.flags&TVHT_ONITEMLABEL)
			szState += _T("Clicked on item's label.");

		CString szItemText = m_columnTree.GetItemText(hItem, htinfo.iSubItem);
		MessageBox(szState + _T(" Item text: ") + szItemText);
	}
	
}
