// RedirDemoDlg.h : header file
//

#pragma once

#include "DemoRedir.h"

// CRedirDemoDlg dialog
class CRedirDemoDlg : public CDialog
{
// Construction
public:
	CRedirDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_REDIRDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CDemoRedirector m_redir;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CString m_strCommand;
	CString m_strInput;

public:
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonInput();
};
