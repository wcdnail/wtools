#include "stdafx.h"
#include "CColorPickerDlg.h"
#include "CAppModuleRef.h"

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
    : m_ccColorPicker{}
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

INT_PTR CColorPickerDlg::DoModal(HWND hWndParent, LPARAM dwInitParam /*= 0*/)
{
    HRESULT const hCode{Initialize()};
    if (FAILED(hCode)) {
        // ##TODO: report Initialize
        return static_cast<INT_PTR>(hCode);
    }
    return WTL::CIndirectDialogImpl<CColorPickerDlg>::DoModal(hWndParent, dwInitParam);
}

void CColorPickerDlg::DoInitTemplate()
{
    bool constexpr       bExTemplate{true};
    short constexpr               nX{0};
    short constexpr               nY{0};
    short constexpr           nWidth{DIALOG_CX};
    short constexpr          nHeight{DIALOG_CY};
    LPCTSTR constexpr      szCaption{_T("Color Picker")};
    DWORD constexpr          dwStyle{WS_OVERLAPPEDWINDOW};
    DWORD constexpr        dwExStyle{WS_EX_APPWINDOW};
    LPCTSTR constexpr     szFontName{_T("MS Shell Dlg 2")};
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
    CONTROL_CONTROL(_T(""), IDC_COLORPICKER, _T("WCCF::CColorPicker"), 0, 14, 14, DIALOG_CX-28, DIALOG_CY-54, 0)
}

const WTL::_AtlDlgResizeMap* CColorPickerDlg::GetDlgResizeMap()
{
    static const WTL::_AtlDlgResizeMap theMap[] = {
        DLGRESIZE_CONTROL(IDC_COLORPICKER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        { -1, 0 },
    };
    return theMap;
}

BOOL CColorPickerDlg::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled = TRUE;
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(lResult);
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

BOOL CColorPickerDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);

    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
    }
    CenterWindow(GetParent());

    //WTL::CIcon const icon(LoadIconW(WTL::ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
    //SetIcon(icon, TRUE);
    //SetIcon(icon, FALSE);

    DlgResize_Init(true, true, 0);
    return TRUE;
}

void CColorPickerDlg::OnDestroy()
{
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
