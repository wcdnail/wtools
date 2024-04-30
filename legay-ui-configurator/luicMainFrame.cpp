#include "pch.h"
#include "luicMainFrame.h"
#include "luicMain.h"
#include "UT/debug.assistance.h"
#include "resz/resource.h"

CMainFrame::~CMainFrame()
{
}

CMainFrame::CMainFrame(CLegacyUIConfigurator& app)
    : m_App(app)
{
}

BOOL CMainFrame::OnInitDlg(HWND, LPARAM)
{
    CIcon icon(::LoadIconW(m_App.GetModuleInstance(), MAKEINTRESOURCEW(IDI_COMP)));
    if (nullptr != icon) {
        HICON raw = icon.Detach();
        SetIcon(raw, FALSE);
        SetIcon(raw, TRUE);
    }

    MoveToMonitor{}.Move(m_hWnd);

    //DlgResize_Init(false, false);
    return 0;
}

void CMainFrame::OnDestroy()
{
}
