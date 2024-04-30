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

    enum : LONG
    {
        IDD = IDD_EMPTY,
        DLG_WIDHT = 1200,
        DLG_HEIGHT = 650,
    };

    NormWindow m_normal;

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
        CRect rcWin;
        GetWindowRect(rcWin);
        rcWin.right = rcWin.left + DLG_WIDHT;
        rcWin.bottom = rcWin.top + DLG_HEIGHT;
        MoveWindow(rcWin, TRUE);

        m_normal.MakeItNorm(m_hWnd);

        MoveToMonitor{}.Move(m_hWnd, MoveToMonitor::FirstNotPrimary, PutAt::YCenter | PutAt::XCenter);
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
