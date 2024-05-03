#pragma once

#include <afxcmn.h>
#include "CGridListCtrlEx\CGridListCtrlGroups.h"
#include "CListCtrl_DataModel.h"
#include "resource.h"

class CGridListCtrlExDlg : public CDialog
{
public:
    enum { IDD = IDD_CGRIDLISTCTRLEX_DIALOG };

	CGridListCtrlExDlg(CWnd* pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CGridListCtrlGroups m_ListCtrl;
	CListCtrl_DataModel m_DataModel;
	CImageList m_ImageList;

	CGridListCtrlExDlg(const CGridListCtrlExDlg&);
	CGridListCtrlExDlg& operator=(const CGridListCtrlExDlg&);
};
