#include "pch.hxx"
#include "dh.tracing.h"
#include "err.printer.h"
#include "debug.assistance.h"
#include <gtest/gtest.h>

struct CTestWinUIDlg: CDialogImpl<CTestWinUIDlg, CWindow>,
                      WTL::CDialogResize<CTestWinUIDlg>
{
    using  Thiz = CTestWinUIDlg;
    using Super = CDialogImpl<CTestWinUIDlg, CWindow>;

    enum { IDD = IDD_EMPTY };

    BEGIN_MSG_MAP(CAboutDlg)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    END_MSG_MAP()

    LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        MakeDlgAppWnd(m_hWnd);
        PlaceWndToMonitor(m_hWnd);
        return TRUE;
    }
};

struct TestWinUI : ::testing::Test
{
};

TEST_F(TestWinUI, Simple)
{
    CTestWinUIDlg dlg;
    auto rv = dlg.DoModal();
    if (rv <= 0) {
        PrintLastError("Error in 'CTestWinUIDlg::DoModal'");
    }
}
