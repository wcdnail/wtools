#pragma once

#include "resource.h"
#include <WTL/CColorButton.h>
#include <WTL/CColorCell.h>
#include <WTL/CColorPickerDlg.h>
#include <color.stuff.h>
#include <dh.tracing.h>
#include <dh.tracing.defs.h>
#include <dev.assistance/dev.assist.h>
#include <atlwin.h>
#include <atlcrack.h>

struct CDefaultWin32Dlg: ATL::CDialogImpl<CDefaultWin32Dlg>,
                         WTL::CMessageFilter
{
    enum { IDD = IDD_DIALOG1 };

    CColorButton m_btnMyColor1{};
    CColorButton m_btnMyColor2{};
    CColorButton m_btnMyColor3{};

    CColorCell       m_crCell1{0x000000, RGB_MAX_INT};
    CColorPickerDlg    m_cpDlg{};

    BOOL PreTranslateMessage(MSG* pMsg) override
    {
        return IsDialogMessageW(pMsg);
    }

    HRESULT Initialize()
    {
        return m_cpDlg.Initialize();
    }

    BEGIN_MSG_MAP_EX(CDefaultWin32Dlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        REFLECT_NOTIFICATIONS()
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
        case IDCANCEL:      OnEndDialog(nID); return ;
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

        m_cpDlg.GetMasterColor().SetTarget(m_btnMyColor1);
        m_btnMyColor1.SetTarget(m_crCell1);
        m_crCell1.ColorTarget().SetTarget(m_cpDlg.GetMasterColor());

        m_cpDlg.Show(m_hWnd);

        m_crCell1.SetColor(0x7f3a21, RGB_MAX_INT);
        m_crCell1.SetHolder(GetDlgItem(IDC_COLOR1));
        return TRUE;
    }
};
