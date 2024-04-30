#include "pch.hxx"
#include "dh.tracing.h"
#include "err.printer.h"
#include "rez/resource.h"
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
        //CompositionHost::GetInstance().Initialize(m_hWnd);
        //srand(time(nullptr));
        //
        //for (int i=0; i<10; i++) {
        //    double size = (double)(rand() % 150 + 50);
        //    double x = (double)(rand() % 600);
        //    double y = (double)(rand() % 200);
        //    CompositionHost::GetInstance().AddElement(size, x, y);
        //}
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
        PrintLastError();
    }
}
