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
#include <atlddx.h>
#include <atlcrack.h>

struct CDefaultWin32Dlg: ATL::CDialogImpl<CDefaultWin32Dlg>,
                         WTL::CDialogResize<CDefaultWin32Dlg>,
                         WTL::CWinDataExchange<CDefaultWin32Dlg>,
                         WTL::CMessageFilter
{
    enum { IDD = IDD_DIALOG1 };

    CColorButton m_btnMyColor1{};
    CColorButton m_btnMyColor2{};
    CColorButton m_btnMyColor3{};

    CColorCellEx     m_crCell1{0x000000, RGB_MAX_INT};
    CColorPickerDlg    m_cpDlg{0x1f2f3f};

    BOOL              m_brLeft{TRUE};
    BOOL           m_brXCenter{FALSE};
    BOOL             m_brRight{FALSE};
    BOOL               m_brTop{TRUE};
    BOOL           m_brYCenter{FALSE};
    BOOL            m_brBottom{FALSE};
    ATL::CString    m_sMvFlags{};
    UINT         m_cpMoveFlags{0};

    void UpdateMoveFlags()
    {
        m_cpMoveFlags = 0;

        if         (m_brLeft) { m_cpMoveFlags |= Rc::Left; }
        else if   (m_brRight) { m_cpMoveFlags |= Rc::Right; }
        else if (m_brXCenter) { m_cpMoveFlags |= Rc::XCenter; }

        if          (m_brTop) { m_cpMoveFlags |= Rc::Top; }
        else if  (m_brBottom) { m_cpMoveFlags |= Rc::Bottom; }
        else if (m_brYCenter) { m_cpMoveFlags |= Rc::YCenter; }

        m_sMvFlags.Format(_T("0x%04x"), m_cpMoveFlags);
        DoDataExchange(DDX_LOAD, IDC_PICKER_MV_FLAGS);
    }

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
        return S_OK;
    }

    BEGIN_DDX_MAP(CDefaultWin32Dlg)
        DDX_CHECK(IDC_RADIO_LEFT, m_brLeft)
        DDX_CHECK(IDC_RADIO_XCENTER, m_brXCenter)
        DDX_CHECK(IDC_RADIO_RIGHT, m_brRight)
        DDX_CHECK(IDC_RADIO_TOP, m_brTop)
        DDX_CHECK(IDC_RADIO_YCENTER, m_brYCenter)
        DDX_CHECK(IDC_RADIO_BOTTOM, m_brBottom)
        if (!bSaveAndValidate) {
            DDX_TEXT(IDC_PICKER_MV_FLAGS, m_sMvFlags)
        }
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CDefaultWin32Dlg)
        DLGRESIZE_CONTROL(IDC_BUTTON1, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_BUTTON2, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_BUTTON3, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_COLOR1, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_DEBUG_CONSOLE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP(CDefaultWin32Dlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(WTL::CDialogResize<CDefaultWin32Dlg>)
    END_MSG_MAP()

    LRESULT OnNotify(int nID, LPNMHDR pnmh)
    {
        switch (pnmh->code) {
        case BCN_HOTITEMCHANGE: //
        case NM_CUSTOMDRAW:     //
            break;              // -- skip
        default:
            DBGTPrint(LTH_WM_NOTIFY L" id:%-4d nc:%-5d %s\n", nID, pnmh->code, DH::WM_NC_C2SW(pnmh->code));
            break;
        }
        SetMsgHandled(FALSE);
        return 0;
    }

    void OnCommand(UINT uNotifyCode, int nID, HWND)
    {
        switch (nID) {
        case IDOK:
        case IDCANCEL:
            OnEndDialog(nID);
            return ;
        case IDC_PICKER_MV_FLAGS:
            return ;
        case IDC_BUTTON1:
        case IDC_BUTTON2:
        case IDC_BUTTON3:
            break;
        case IDC_RADIO_LEFT:
        case IDC_RADIO_XCENTER:
        case IDC_RADIO_RIGHT:
        case IDC_RADIO_TOP:
        case IDC_RADIO_YCENTER:
        case IDC_RADIO_BOTTOM:
            DoDataExchange(DDX_SAVE);
            UpdateMoveFlags();
        case IDC_BN_MOVEPICKER:
            m_cpDlg.MoveWindow(m_cpMoveFlags);
            return ;
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
        CRect         rcLog{};
        ATL::CWindow wndLog{GetDlgItem(IDC_DEBUG_CONSOLE)};
        wndLog.GetWindowRect(rcLog);
        wndLog.DestroyWindow();
        ScreenToClient(rcLog);
        DH::DebugConsole::Instance().Create(m_hWnd, rcLog, WS_CHILD | WS_VISIBLE, 0, IDC_DEBUG_CONSOLE);

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

        DoDataExchange(DDX_LOAD);
        UpdateMoveFlags();

        m_cpDlg.Show(m_hWnd, m_cpMoveFlags, false);

        m_crCell1.SetColor(0x7f3a21, RGB_MAX_INT);
        m_crCell1.SetHolder(GetDlgItem(IDC_COLOR1));

        DlgResize_Init(false, true, 0);
        return TRUE;
    }
};
