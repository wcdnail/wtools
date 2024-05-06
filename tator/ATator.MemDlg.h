#pragma once

#include "resource.h"
#include <atlframe.h>
#include <atldlgs.h>
#include <atlcrack.h>

class CTatorMainDlg : public CIndirectDialogImpl<CTatorMainDlg>,
                      public CDialogResize<CTatorMainDlg>
    
{
public:
    enum Sizes: short
    {
        DIALOG_X = 0,
        DIALOG_Y = 0,
        DIALOG_CX = 500,
        DIALOG_CY = 250,
        DIALOG_BRD_CX = 6,
        DIALOG_BRD_CY = 6,
    };

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 26999,
        IDC_DLG_ICON,
        IDC_GRP_BEVEL,
    };

    BEGIN_DIALOG(DIALOG_X, DIALOG_Y, DIALOG_CX, DIALOG_CY)
        DIALOG_CAPTION(_T("TATOR [XYZ]"))
        DIALOG_STYLE(WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU) // DS_MODALFRAME
        DIALOG_EXSTYLE(WS_EX_APPWINDOW)
        DIALOG_MENU(MAKEINTRESOURCE(IDR_MENU1))
        DIALOG_FONT(8, _T("MS Shell Dlg 2"))
    END_DIALOG()

    BEGIN_CONTROLS_MAP()
        //CONTROL_DEFPUSHBUTTON(_T("OK"), IDOK, 130, 81, 50, 14, 0, 0)
        CONTROL_CTEXT(_T(""), IDC_GRP_BEVEL, DIALOG_BRD_CX, DIALOG_BRD_CY, DIALOG_CX - DIALOG_BRD_CX*2, DIALOG_CY - DIALOG_BRD_CY*2, SS_SUNKEN | WS_GROUP, 0)
        CONTROL_ICON(MAKEINTRESOURCE(IDI_ICON1), IDC_DLG_ICON, DIALOG_BRD_CX+8, DIALOG_CY - DIALOG_BRD_CY*2 - 20, 18, 20, WS_GROUP, 0)
    END_CONTROLS_MAP()

    BEGIN_DLGRESIZE_MAP(CTatorMainDlg)
        DLGRESIZE_CONTROL(IDC_GRP_BEVEL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
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

        CIcon icon(LoadIconW(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
        SetIcon(icon, TRUE);
        SetIcon(icon, FALSE);

        DlgResize_Init(false, false);
        CenterWindow(GetParent());
        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }
};
