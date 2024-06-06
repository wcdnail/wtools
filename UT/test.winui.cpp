#include "pch.hxx"
#include "rez/resource.h"
#include <clr.dracula.h>
#include <dh.tracing.h>
#include <wtl.compositor/wtl.compositor.h>
#include <dev.assistance/err.printer.h>
#include <dev.assistance/debug.assistance.h>
#include <gtest/gtest.h>

struct CTestWinUIDlg: CDialogImpl<CTestWinUIDlg>, // WTL::CDialogResize<CTestWinUIDlg>
                      CF::UI::Compositor
{
    using       Thiz = CTestWinUIDlg;
    using      Super = CDialogImpl<CTestWinUIDlg>;
    using Compositor = CF::UI::Compositor;

    enum : LONG
    {
        IDD = IDD_STD_CONTROLS,
        DLG_WIDHT = 1200,
        DLG_HEIGHT = 650,
    };

    NormWindow m_normal {};

    void PrepareWindow()
    {
        CRect rcWin;
        GetWindowRect(rcWin);
        rcWin.right = rcWin.left + DLG_WIDHT;
        rcWin.bottom = rcWin.top + DLG_HEIGHT;
        MoveWindow(rcWin, TRUE);

        m_normal.dwExStyle |= WS_EX_COMPOSITED | WS_EX_LAYERED;
        m_normal.MakeItNorm(m_hWnd, MAKEINTRESOURCEW(IDI_WTL_APP));

        MoveToMonitor{}.Move(m_hWnd, 3 /*MoveToMonitor::FirstNotPrimary*/, PutAt::YCenter | PutAt::XCenter);
    }

    BEGIN_MSG_MAP(CAboutDlg)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        CHAIN_MSG_MAP(Compositor)
    END_MSG_MAP()

    LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        WORD const notifyCode{HIWORD(wParam)};
        WORD const      cmdId{LOWORD(wParam)};
        HWND const    hCtlWnd{reinterpret_cast<HWND>(lParam)};
        switch (cmdId) {
        case IDCANCEL:
            OnCloseCmd(notifyCode, cmdId, hCtlWnd, bHandled);
            break;
        default:
            DBGTPrint(0, L"WM_COMMAND Unknown: n:%04d c:%04d w:%08x\n", notifyCode, cmdId, hCtlWnd);
        }
        return 0;
    }

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        PrepareWindow();
        ShowWindow(SW_SHOW);
        EnableComposition(*this);
        return TRUE;
    }

    // Composition --------------------------------------------------
};

struct TestWinUI : ::testing::Test
{
};

TEST_F(TestWinUI, Simple)
{
    try {
        CTestWinUIDlg dlg;
        auto rv = dlg.DoModal();
        if (rv <= 0) {
            PrintLastError("Error in 'CTestWinUIDlg::DoModal'");
        }
    }
    catch (winrt::hresult_error const& ex) {
        PrintException("WINRT ERROR", ex);
    }
    catch (std::exception const& ex) {
        PrintException("ERROR", ex);
    }
}
