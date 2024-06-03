#pragma once

#include "resource.h"
#include <WTL/CColorButton.h>
#include <WTL/CColorCell.h>
#include <WTL/CColorPickerDlg.h>
#include <color.stuff.h>
#include <dh.tracing.h>
#include <dh.tracing.defs.h>
#include <dev.assistance/dev.assist.h>
#include <dev.assistance/debug.console/debug.console.h>
#include <atlwin.h>
#include <atlcrack.h>

struct CDefaultWin32Dlg: ATL::CDialogImpl<CDefaultWin32Dlg>,
                         WTL::CDialogResize<CDefaultWin32Dlg>,
                         WTL::CMessageFilter
{
    enum { IDD = IDD_DIALOG1 };

    CColorButton m_btnMyColor1{};
    CColorButton m_btnMyColor2{};
    CColorButton m_btnMyColor3{};

    CColorCellEx     m_crCell1{0x000000, RGB_MAX_INT};
    CColorPickerDlg    m_cpDlg{};

    BOOL PreTranslateMessage(MSG* pMsg) override
    {
        return IsDialogMessageW(pMsg);
    }

    HRESULT Initialize()
    {
        HRESULT hCode{S_OK};
        hCode = m_cpDlg.Initialize();
        if (FAILED(hCode)) {
            return hCode;
        }
        hCode = DH::DebugConsole::Instance().Initialize();
        if (FAILED(hCode)) {
            return hCode;
        }
        return S_OK;
    }

    BEGIN_DLGRESIZE_MAP(CDefaultWin32Dlg)
        DLGRESIZE_CONTROL(IDC_BUTTON1, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_BUTTON2, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_BUTTON3, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_COLOR1, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_CUSTOM1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP_EX(CDefaultWin32Dlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(WTL::CDialogResize<CDefaultWin32Dlg>)
    END_MSG_MAP()

    LRESULT OnNotify(int nID, LPNMHDR pnmh)
    {
        DBGTPrint(LTH_WM_NOTIFY L" id:%-4d nc:%-5d %s\n", nID, pnmh->code, DH::WM_NC_C2SW(pnmh->code));
        SetMsgHandled(FALSE);
        return 0;
    }

    void OnCommand(UINT uNotifyCode, int nID, HWND)
    {
        switch (nID) {
        case IDOK:
        case IDCANCEL: OnEndDialog(nID); return ;
        default:
            DBGTPrint(LTH_WM_COMMAND L" id:%-4d nc:%-5d %s\n", nID, uNotifyCode, DH::WM_NC_C2SW(uNotifyCode));
            break;
        }
        SetMsgHandled(FALSE);
    }

    LRESULT OnEndDialog(int wID)
    {
        EndDialog(wID);
        return 0;
    }

    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI)
    {
        if (IDC_COLOR1 == nID) {
            m_crCell1.Draw(pDI->hDC, pDI->rcItem, nullptr);
        }
        else {
            SetMsgHandled(FALSE);
        }
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        DH::DebugConsole::Instance().ReceiveDebugOutput(true, "", false);

        m_btnMyColor1.SubclassWindow(GetDlgItem(IDC_BUTTON1));
        m_btnMyColor2.SubclassWindow(GetDlgItem(IDC_BUTTON2));
        m_btnMyColor3.SubclassWindow(GetDlgItem(IDC_BUTTON3));

        const WTL::CIcon icon(LoadIconW(WTL::ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
        SetIcon(icon, TRUE);
        SetIcon(icon, FALSE);

        m_btnMyColor2.SetDefaultText(_T(""));
        m_btnMyColor2.SetCustomText(_T(""));
        m_btnMyColor3.SetDefaultText(_T("My Default Text"));
        m_btnMyColor3.SetCustomText(_T(""));

        CenterWindow();

        m_crCell1.AddObserver(m_cpDlg.GetMasterObserver());
        m_crCell1.AddObserver(m_btnMyColor1);
        m_crCell1.AddObserver(m_btnMyColor2);
        m_crCell1.AddObserver(m_btnMyColor3);

        m_cpDlg.GetMasterColor().AddObservers(m_crCell1);
        m_cpDlg.GetMasterColor().AddObserver(m_crCell1);

        m_btnMyColor1.AddObservers(m_cpDlg.GetMasterColor());
        m_btnMyColor2.AddObservers(m_cpDlg.GetMasterColor());
        m_btnMyColor3.AddObservers(m_cpDlg.GetMasterColor());

        m_cpDlg.Show(m_hWnd, Rc::Bottom | Rc::XCenter, false);

        m_crCell1.SetColor(0x7f3a21, RGB_MAX_INT);
        m_crCell1.SetHolder(GetDlgItem(IDC_COLOR1));

        DlgResize_Init(false, true);
        return TRUE;
    }
};
