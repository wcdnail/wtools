#include "pch.hxx"
#include "wtl.aero.h"
#include "rez/resource.h"
#include <gtest/gtest.h>

#include "err.printer.h"

#pragma comment (lib, "dwmapi.lib")

struct TestAeroDialogs: ::testing::Test
{
};

class CTestAeroDlg : public CAeroDialogImpl<CTestAeroDlg>
{
public:
    using Super = CAeroDialogImpl<CTestAeroDlg>;

    enum { IDD = IDD_TEST_GENERIC };

    CAeroStatic m_Icon;
    CAeroStatic m_Text;
    CAeroButton   m_OK;

    BEGIN_MSG_MAP(CAboutDlg)
        CHAIN_MSG_MAP(Super)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());
        MARGINS m = {7, 7, 7, 7};
        SetMargins(m);
        m_Icon.SubclassWindow(GetDlgItem(IDC_APPICON));
        m_Text.SubclassWindow(GetDlgItem(IDC_TEXT));
        m_OK.SubclassWindow(GetDlgItem(IDOK));
        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }
};


TEST_F(TestAeroDialogs, Customizabe)
{
    CTestAeroDlg dlg;
    auto rv = dlg.DoModal();
    if (rv <= 0) {
        PrintLastError();
    }
}
