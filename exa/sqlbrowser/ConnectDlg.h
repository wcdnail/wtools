#pragma once

#include <afxdialogex.h>

class CConnectDlg: public CDialogEx
{
	DECLARE_DYNAMIC(CConnectDlg)

public:
    static const UINT IDD;

	CConnectDlg(CWnd* parent);
	virtual ~CConnectDlg();

private:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
