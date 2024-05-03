#include "stdafx.h"
#include "CGridListCtrlExDlg.h"
#include "CGridListCtrlEx\CGridColumnTraitDateTime.h"
#include "CGridListCtrlEx\CGridColumnTraitEdit.h"
#include "CGridListCtrlEx\CGridColumnTraitCombo.h"
#include "CGridListCtrlEx\CGridColumnTraitHyperLink.h"
#include "CGridListCtrlEx\CGridRowTraitXP.h"
#include "CGridListCtrlEx\ViewConfigSection.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGridListCtrlExDlg::CGridListCtrlExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridListCtrlExDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGridListCtrlExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CGridListCtrlExDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CGridListCtrlExDlg message handlers

BOOL CGridListCtrlExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_ImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 1, 0);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGDEN));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGGERM));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGFRAN));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGGREEC));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGSWED));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGSPAIN));
	int nStateImageIdx = CGridColumnTraitDateTime::AppendStateImages(m_ListCtrl, m_ImageList);
	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);
	
	// Give better margin to editors
	m_ListCtrl.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
	m_ListCtrl.SetDefaultRowTrait(pRowTrait);

	// Create Columns
	m_ListCtrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	for(int col = 0; col < m_DataModel.GetColCount() ; ++col)
	{
		const CString& title = m_DataModel.GetColTitle(col);
		CGridColumnTrait* pTrait = NULL;
		if (col==0)	// Country
		{
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
			const std::vector<CString>& countries = m_DataModel.GetCountries();
			for(size_t i=0; i < countries.size() ; ++i)
				pComboTrait->AddItem((DWORD_PTR)i, countries[i]);
			pTrait = pComboTrait;
		}
		if (col==1)	// City
		{
			pTrait = new CGridColumnTraitEdit;
		}
		if (col==2)	// Year won
		{
			CGridColumnTraitDateTime* pDateTimeTrait = new CGridColumnTraitDateTime;
			pDateTimeTrait->AddImageIndex(nStateImageIdx, _T(""), false);		// Unchecked (and not editable)
			pDateTimeTrait->AddImageIndex(nStateImageIdx+1, COleDateTime(1970,1,1,0,0,0).Format(), true);	// Checked (and editable)
			pDateTimeTrait->SetToggleSelection(true);
			pTrait = pDateTimeTrait;
		}
		if (col==3)	// Year won
		{
			CGridColumnTraitHyperLink* pHyperLinkTrait = new CGridColumnTraitHyperLink;
			pHyperLinkTrait->SetShellFilePrefix(_T("http://en.wikipedia.org/wiki/UEFA_Euro_"));
			pTrait = pHyperLinkTrait;
		}

		m_ListCtrl.InsertColumnTrait(col+1, title, LVCFMT_LEFT, 100, col, pTrait);
	}

	// Insert data into list-control by copying from datamodel
	int nItem = 0;
	for(size_t rowId = 0; rowId < m_DataModel.GetRowIds() ; ++rowId)
	{
		nItem = m_ListCtrl.InsertItem(++nItem, m_DataModel.GetCellText(rowId,0));
		m_ListCtrl.SetItemData(nItem, rowId);
		for(int col = 0; col < m_DataModel.GetColCount() ; ++col)
		{
			int nCellCol = col+1;	// +1 because of hidden column
			const CString& strCellText = m_DataModel.GetCellText(rowId, col);
			m_ListCtrl.SetItemText(nItem, nCellCol, strCellText);
			if (nCellCol==3)
			{
				if (strCellText==_T(""))
					m_ListCtrl.SetCellImage(nItem, nCellCol, nStateImageIdx);	// unchecked
				else
					m_ListCtrl.SetCellImage(nItem, nCellCol, nStateImageIdx+1);	// checked
			}
		}
		m_ListCtrl.SetCellImage(nItem, 1, nItem); // Assign flag-images
	}

	CViewConfigSectionWinApp* pColumnProfile = new CViewConfigSectionWinApp(_T("Sample List"));
	pColumnProfile->AddProfile(_T("Default"));
	pColumnProfile->AddProfile(_T("Special"));
	m_ListCtrl.SetupColumnConfig(pColumnProfile);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGridListCtrlExDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGridListCtrlExDlg::OnPaint() 
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
HCURSOR CGridListCtrlExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
