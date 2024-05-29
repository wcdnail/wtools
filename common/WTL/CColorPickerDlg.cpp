#include "stdafx.h"
#include "CColorPickerDlg.h"
#include "CAppModuleRef.h"
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>

namespace
{
    enum Sizes: short
    {
        DIALOG_CX = CColorPicker::DLG_CX + 90,
        DIALOG_CY = CColorPicker::DLG_CY + 80,
    };

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 71,
        IDC_COLORPICKER,
    };
}

CColorPickerDlg::~CColorPickerDlg() = default;

CColorPickerDlg::CColorPickerDlg()
    :     m_wndMaster{nullptr}
    , m_ccColorPicker{}
    ,       m_rcPlace{0, 0, 0, 0}
{
}

BOOL CColorPickerDlg::PreTranslateMessage(MSG* pMsg)
{
    if (IsDialogMessageW(pMsg)) {
        return TRUE;
    }
    return FALSE;
}

HRESULT CColorPickerDlg::Initialize()
{
    return m_ccColorPicker.PreCreateWindow();
}

bool CColorPickerDlg::Show(HWND hWndMaster, COLORREF& crTarget, bool bModal)
{
    std::wstring sFunc{L"NONE"};
    HRESULT      hCode{Initialize()};
    if (FAILED(hCode)) {
        sFunc = L"Initialize";
        goto reportError;
    }
    m_wndMaster = hWndMaster;
    m_ccColorPicker.SetTracked(crTarget);
    if (bModal) {
        auto const nRes{WTL::CIndirectDialogImpl<CColorPickerDlg>::DoModal(hWndMaster)};
        return nRes == IDOK;
    }
    if (!WTL::CIndirectDialogImpl<CColorPickerDlg>::Create(hWndMaster)) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"Create";
        goto reportError;
    }
    ShowWindow(SW_SHOW);
    return true;
reportError:
    auto const msg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(LTH_CONTROL L" %s failed: 0x%08x %s\n", sFunc.c_str(), hCode, msg.GetString());
    return false;
}

void CColorPickerDlg::DoInitTemplate()
{
    bool constexpr       bExTemplate{true};
    short constexpr               nX{0};
    short constexpr               nY{0};
    short constexpr           nWidth{DIALOG_CX};
    short constexpr          nHeight{DIALOG_CY};
    PCTSTR constexpr       szCaption{_T("Color Picker")};
    DWORD constexpr          dwStyle{WS_OVERLAPPEDWINDOW};
    DWORD constexpr        dwExStyle{WS_EX_APPWINDOW};
    PCTSTR constexpr      szFontName{_T("MS Shell Dlg 2")};
    WORD constexpr         wFontSize{8};
    WORD constexpr           wWeight{0};
    BYTE constexpr           bItalic{0};
    BYTE constexpr          bCharset{0};
    DWORD constexpr         dwHelpID{0};
    ATL::_U_STRINGorID const    Menu{0U};
    ATL::_U_STRINGorID const szClass{0U};
    m_Template.Create(bExTemplate, szCaption,
                      nX, nY, nWidth, nHeight,
                      dwStyle, dwExStyle,
                      szFontName, wFontSize, wWeight, bItalic, bCharset,
                      dwHelpID, szClass, Menu);
}

void CColorPickerDlg::DoInitControls()
{
    CONTROL_CONTROL(_T(""), IDC_COLORPICKER, _T("WCCF::CColorPicker"), 0, 14,           14, DIALOG_CX-28, DIALOG_CY-54, 0)
    CONTROL_PUSHBUTTON(_T("Cancel"),             IDCANCEL,                14, DIALOG_CY-12,           48,  DIALOG_CY-2, BS_PUSHBUTTON, 0)
    CONTROL_PUSHBUTTON(_T("OK"),                     IDOK, DIALOG_CX-(28+48), DIALOG_CY-12,           48,  DIALOG_CY-2, BS_DEFPUSHBUTTON, 0)
}

const WTL::_AtlDlgResizeMap* CColorPickerDlg::GetDlgResizeMap()
{
    static constexpr WTL::_AtlDlgResizeMap gs_Map[] = {
        DLGRESIZE_CONTROL(IDC_COLORPICKER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        { -1, 0 },
    };
    return gs_Map;
}

BOOL CColorPickerDlg::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled = TRUE;
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(bHandled);
    switch(dwMsgMapID) {
    case 0:
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(CDialogResize<CColorPickerDlg>)
        break;
    default: 
        ATLTRACE(static_cast<int>(ATL::atlTraceWindowing), 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

void CColorPickerDlg::PrepareRect(ATL::CWindow wndParent)
{
    if (!wndParent.m_hWnd) {
        return ;
    }
    CRect     rcMy{};
    CRect rcParent{};
    wndParent.GetWindowRect(rcParent);
    GetWindowRect(rcMy);
    LONG const    nCX{rcMy.Width()};
    LONG const    nCY{rcMy.Height()};
    CPoint const ptLT{rcParent.right + 2, rcParent.top};
    CPoint const ptRB{ptLT.x + nCX, ptLT.y + nCY};
    m_rcPlace.SetRect(ptLT, ptRB);
}

BOOL CColorPickerDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);

    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
    }

    if (m_wndMaster.m_hWnd) {
        ModifyStyleEx(0, WS_EX_TOOLWINDOW);
    }

    PrepareRect(m_wndMaster);
    if (m_rcPlace.IsRectEmpty()) {
        CenterWindow(m_wndMaster);
    }
    else {
        MoveWindow(m_rcPlace, FALSE);
    }

  //WTL::CIcon const icon(LoadIconW(WTL::ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
  //SetIcon(icon, TRUE);
  //SetIcon(icon, FALSE);

    DlgResize_Init(true, true, 0);
    return TRUE;
}

void CColorPickerDlg::OnDestroy()
{
    m_wndMaster = nullptr;
    SetMsgHandled(FALSE);
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
    }
}

LRESULT CColorPickerDlg::OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
{
#ifdef _DEBUG
    if (m_bModal) {
        EndDialog(wID);
    }
#else
    EndDialog(wID);
#endif
    return 0;
}
