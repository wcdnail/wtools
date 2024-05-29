#pragma once

#include <WTL/CColorPicker.h>
#include <rect.screen.h>
#include <atlwin.h>
#include <atlcrack.h>
#include "resource.h"

struct CWCDColorPicker: ATL::CDialogImpl<CWCDColorPicker>,
                        WTL::CMessageFilter,
                        WTL::CDialogResize<CWCDColorPicker>
{
    enum { IDD = IDD_DIALOG1 };

    CColorPicker m_ColorPicker;

    ~CWCDColorPicker() = default;

    CWCDColorPicker()
        : m_ColorPicker{}
    {
    }

    BOOL PreTranslateMessage(MSG* pMsg) override
    {
        return IsDialogMessageW(pMsg);
    }

    HRESULT Initialize()
    {
        return m_ColorPicker.PreCreateWindow();
    }

    BEGIN_MSG_MAP(CWCDColorPicker)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnEndDialog)   
        COMMAND_ID_HANDLER(IDCANCEL, OnEndDialog)
        MSG_WM_MOVE(OnMove)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(WTL::CDialogResize<CWCDColorPicker>)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CWCDColorPicker)
        DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_CUSTOM1, DLSZ_SIZE_Y | DLSZ_SIZE_X)
    END_DLGRESIZE_MAP()

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        ATLASSERT(m_ColorPicker.IsWindow());

        CenterWindow();

        WTL::CIcon const icon{LoadIconW(WTL::ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1))};
        SetIcon(icon, TRUE);
        SetIcon(icon, FALSE);

        DlgResize_Init(true, true, 0);
        return TRUE;
    }

    LRESULT OnEndDialog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }

    void OnMove(CPoint pt)
    {
        if (pt.y > Rc::Screen.bottom) {
            // TODO: investigate drawing out screen issues
            m_ColorPicker.InvalidateRect(nullptr, FALSE);
        }
    }
};
