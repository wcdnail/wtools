#pragma once

#include "resource.h"
#include <WTL/CColorButton.h>
#include <WTL/CColorCell.h>
#include <WTL/CColorPickerDlg.h>
#include <color.stuff.h>
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
        COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
        COMMAND_ID_HANDLER(IDOK, OnEndDialog)
        COMMAND_ID_HANDLER(IDCANCEL, OnEndDialog)
        MSG_WM_DRAWITEM(OnDrawItem)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

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

        m_crCell1.SetColor(0xff00ff, 255);
        m_crCell1.SetHolder(GetDlgItem(IDC_COLOR1));
        m_btnMyColor1.SetColorTarget({&m_crCell1});
        m_cpDlg.Show(m_hWnd, {&m_btnMyColor1});

        return TRUE;
    }

    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        //ATL::CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
        //dlg.DoModal();
        return 0;
    }

    LRESULT OnEndDialog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        // TODO: Add validation code 
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
};
