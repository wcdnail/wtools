#pragma once

#include "WTL/CColorButton.h"
#include "WTL/CColorPicker.h"
#include "resource.h"
#include <atlframe.h>
#include <atlcrack.h>
#include <atldlgs.h>
#include <atlddx.h>

struct CTatorMainDlg: WTL::CIndirectDialogImpl<CTatorMainDlg>,
                      WTL::CDialogResize<CTatorMainDlg>,
                      WTL::CWinDataExchange<CTatorMainDlg>,
                      WTL::CMessageFilter
{
    enum Sizes: short
    {
        DIALOG_CX = 500,
        DIALOG_CY = 280,
    };

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 771,
        IDC_GRP_BEVEL,
        IDC_CUSTOM_CTL1,
        IDC_DLG_ICON,
    };

    CSpectrumColorPicker m_ccColorPicker;

    BOOL PreTranslateMessage(MSG* pMsg) override
    {
        if (IsDialogMessageW(pMsg)) {
            return TRUE;
        }
        return FALSE;
    }

    HRESULT Initialize()
    {
        return m_ccColorPicker.PreCreateWindow();
    }

    BEGIN_DDX_MAP(CTatorMainDlg)
        //DDX_CONTROL_HANDLE(IDC_CUSTOM_CTL1, m_ccColorPicker)
    END_DDX_MAP()

    BEGIN_DIALOG(0, 0, DIALOG_CX, DIALOG_CY)
        DIALOG_CAPTION(_T("TATOR [XYZ]"))
        DIALOG_STYLE(WS_OVERLAPPEDWINDOW)
        DIALOG_EXSTYLE(WS_EX_APPWINDOW)
        DIALOG_MENU(MAKEINTRESOURCE(IDR_MENU1))
        DIALOG_FONT(8, _T("MS Shell Dlg 2"))
    END_DIALOG()

    BEGIN_CONTROLS_MAP()
        CONTROL_GROUPBOX(_T("Tator Expo"), IDC_GRP_BEVEL, 6, 6, DIALOG_CX-12, DIALOG_CY-12, WS_GROUP, 0)
        CONTROL_CONTROL(_T(""), IDC_CUSTOM_CTL1, _T("CSpectrumColorPicker"), WS_BORDER, 14, 14, DIALOG_CX-28, DIALOG_CY-54, 0)
        CONTROL_ICON(MAKEINTRESOURCE(IDI_ICON1), IDC_DLG_ICON, 14, DIALOG_CY-32, 18, 20, 0, 0)
        //CONTROL_DEFPUSHBUTTON(_T("OK"), IDOK, 130, 81, 50, 14, 0, 0)
    END_CONTROLS_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(IDC_GRP_BEVEL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_CUSTOM_CTL1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_DLG_ICON, DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP_EX(CTatorMainDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        CHAIN_MSG_MAP(CDialogResize<CTatorMainDlg>)
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
    {
        UNREFERENCED_PARAMETER(wndFocus);
        UNREFERENCED_PARAMETER(lInitParam);

        WTL::CIcon const icon(LoadIconW(WTL::ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
        SetIcon(icon, TRUE);
        SetIcon(icon, FALSE);

        DlgResize_Init(true, true, 0);
        CenterWindow(GetParent());
        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if (m_bModal) {
            EndDialog(wID);
        }
        else {
            DestroyWindow();
            PostQuitMessage(wID);
        }
        return 0;
    }
};
