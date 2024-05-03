#include "stdafx.h"
#include "ConnectDlg.h"
#include "resource.h"

const UINT CConnectDlg::IDD = IDD_CONN;

IMPLEMENT_DYNAMIC(CConnectDlg, CDialogEx)

CConnectDlg::CConnectDlg(CWnd* parent)
	: CDialogEx(CConnectDlg::IDD, parent)
{}

CConnectDlg::~CConnectDlg()
{}

void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConnectDlg, CDialogEx)
END_MESSAGE_MAP()
