// DemoMFCDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "ColumnTreeCtrl.h"
#include "EasySize.h"

// CDemoMFCDlg dialog
class CDemoMFCDlg : public CDialog
{
// Construction
public:
	DECLARE_EASYSIZE;
	CDemoMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DEMOMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CColumnTreeCtrl m_columnTree;
	CImageList m_imgList;
private:
	void InitOptions();
	void InitTree();

public:
	CButton m_chkIcons;
	CButton m_chkLargeIcons;
	CButton m_chkHasLines;
	CButton m_chkHasCheckBoxes;
	CButton m_chkTrackSelect;
	CButton m_chkFullRowSelect;
	afx_msg void OnBnClickedChkicons();
	afx_msg void OnBnClickedChklargricons();
	afx_msg void OnBnClickedChkhaslines();
	afx_msg void OnBnClickedChkhaschkboxes();
	afx_msg void OnBnClickedChkfullrowselect();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_chkHasButtons;
	afx_msg void OnBnClickedChkhasbuttons();
	afx_msg void OnRclickedColumntree(LPNMHDR pNMHDR, LRESULT* pResult);
};
