#include "stdafx.h"
#include "CColorPickerDlg.h"
#include "CAppModuleRef.h"
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>

#define _NEED_WMESSAGE_DUMP 1
#include <debug.dump.msg.h>

#include "IColor.h"

CColorPickerDlg::~CColorPickerDlg() = default;

CColorPickerDlg::CColorPickerDlg(COLORREF crColor)
    :   m_wndMaster{nullptr}
    , m_ColorPicker{crColor}
    ,     m_rcPlace{0, 0, 0, 0}
    ,  m_bModalLoop{false}
    ,   m_nPosFlags{Rc::Right}
{
}

BOOL CColorPickerDlg::PreTranslateMessage(MSG* pMsg)
{
    if (IsDialogMessageW(pMsg)) {
        return TRUE;
    }
    return FALSE;
}

bool CColorPickerDlg::Show(HWND hWndMaster, unsigned nPosFlags, bool bModal)
{
    std::wstring sFunc{L"NONE"};
    HRESULT      hCode{Initialize()};
    if (FAILED(hCode)) {
        sFunc = L"Initialize";
        goto reportError;
    }
    m_wndMaster = hWndMaster;
    m_bModalLoop = bModal;
    m_nPosFlags = nPosFlags;
    if (m_bModalLoop) {
        auto const nRes{DoModal(hWndMaster)};
        return nRes == IDOK;
    }
    if (!Create(hWndMaster)) {
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

namespace
{
    enum Sizes: short
    {
        DRGB_CX   = 14,
        DLG_CX    = CColorPicker::HOST_DLG_CX + DRGB_CX + 1,
        DLG_CY    = CColorPicker::HOST_DLG_CY + 1,
        BN_CX     = 48,
        BN_CY     = 12,
        DLG_CY_BN = BN_CY+6,
    };

    enum ControlIds: int
    {
        BEFORE_FIRST_CONTROL_ID = 71,
        IDC_COLORPICKER,
        IDC_DRAGBAR,
    };
}

void CColorPickerDlg::DoInitTemplate()
{
    bool constexpr       bExTemplate{true};
    short constexpr               nX{0};
    short constexpr               nY{0};
    short constexpr           nWidth{DLG_CX};
    short                    nHeight{DLG_CY};
    PCTSTR constexpr       szCaption{_T("Color Picker")};
    DWORD                    dwStyle{WS_POPUP | WS_BORDER};
    DWORD                  dwExStyle{WS_EX_TOOLWINDOW};
    LPCTSTR constexpr     szFontName{CColorPicker::szDlgFont};
    WORD constexpr         wFontSize{CColorPicker::wDlgFontSize};
    WORD constexpr           wWeight{0};
    BYTE constexpr           bItalic{0};
    BYTE constexpr          bCharset{0};
    DWORD constexpr         dwHelpID{0};
    ATL::_U_STRINGorID const    Menu{0U};
    ATL::_U_STRINGorID const szClass{0U};
    if (m_bModalLoop) {
        nHeight  += DLG_CY_BN;
        dwStyle   = WS_OVERLAPPEDWINDOW;
        dwExStyle = 0;
    }
    m_Template.Create(bExTemplate, szCaption,
                      nX, nY, nWidth, nHeight,
                      dwStyle, dwExStyle,
                      szFontName, wFontSize, wWeight, bItalic, bCharset,
                      dwHelpID, szClass, Menu);
}

void CColorPickerDlg::DoInitControls()
{
    short  nPickerX{2};
    short nPickerCX{DLG_CX-4};
    short nPickerCY{DLG_CY-4};
    if (m_bModalLoop) {
        nPickerCY -= (DLG_CY_BN-14);
        CONTROL_PUSHBUTTON(_T("Cancel"), IDCANCEL,                3, DLG_CY-2,   BN_CX, BN_CY, BS_PUSHBUTTON, 0)
        CONTROL_PUSHBUTTON(_T("OK"),         IDOK, DLG_CX-BN_CX*2-3, DLG_CY-2, BN_CX*2, BN_CY, BS_DEFPUSHBUTTON, 0)
    }
    else {
        nPickerX  += DRGB_CX + 2;
        nPickerCX -= DRGB_CX + 2;
        CONTROL_CTEXT(_T("Color Picker"), IDC_DRAGBAR, 3, 4, DRGB_CX, nPickerCY-6, SS_OWNERDRAW, 0)
    }
    CONTROL_CONTROL(_T(""), IDC_COLORPICKER, _T("WCCF::CColorPicker"), 0, nPickerX, 2, nPickerCX, nPickerCY, 0)
}

const WTL::_AtlDlgResizeMap* CColorPickerDlg::GetDlgResizeMap() const
{
    static constexpr WTL::_AtlDlgResizeMap gs_Map[] = {
        DLGRESIZE_CONTROL(IDC_COLORPICKER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_DRAGBAR, DLSZ_SIZE_Y)
        { -1, 0 },
    };
    static constexpr WTL::_AtlDlgResizeMap gs_ModalMap[] = {
        DLGRESIZE_CONTROL(IDC_COLORPICKER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        { -1, 0 },
    };
    return m_bModalLoop ? gs_ModalMap : gs_Map;
}

void CColorPickerDlg::OnColorUpdate(IColor const& clrSource)
{
    UNREFERENCED_PARAMETER(clrSource);
    GetDlgItem(IDC_DRAGBAR).InvalidateRect(nullptr, FALSE);
}

static UINT CCPD_OnGetDlgCode(LPMSG pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);
    if (pMsg) {
        DBG_DUMP_WMESSAGE(LTH_COLORPICKER, L"WNDPROC", pMsg);
    }
    return DLGC_WANTALLKEYS | DLGC_WANTTAB;
}

BOOL CColorPickerDlg::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    //DBG_DUMP_WMESSAGE_EXT(LTH_COLORPICKER, L"WNDPROC", hWnd, uMsg, wParam, lParam);
    BOOL bHandled = TRUE;
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) {
    case 0:
        MSG_WM_GETDLGCODE(CCPD_OnGetDlgCode)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        if (!m_bModalLoop) {
            MSG_WM_DRAWITEM(OnDrawItem)
            MSG_WM_NCHITTEST(OnNcHitTest)
        }
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(CDialogResize)
        break;
    default: 
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    if (!m_bModalLoop && m_ColorPicker.m_hWnd) {
        switch (uMsg) {
        case WM_KEYUP:
            break;
        //case WM_MOVE: // skip IsDialogMessageW recursives
        //case WM_GETICON:
        //case WM_SETICON:
        //case WM_ACTIVATEAPP:
        //case WM_NCACTIVATE:
        //case WM_ACTIVATE:
        //case WM_USER:
        //case WM_CHANGEUISTATE:
        //case WM_SHOWWINDOW:
        //case OCM_CTLCOLORDLG: // 0x2136
        //case WM_PAINT:
        //case WM_NCHITTEST:
        //case WM_SETCURSOR:
        //case WM_MOUSEACTIVATE:
        //case WM_NCMOUSEMOVE:
        //case WM_MOUSEMOVE:
        //case WM_NCMOUSELEAVE:
        //case WM_DESTROY:
        //case WM_UPDATEUISTATE:
        default:
            return FALSE;
        }
        //auto const pCurrMsg{GetCurrentMessage()};
        //MSG      theMessage{hWnd, uMsg, wParam, lParam, pCurrMsg->time, pCurrMsg->pt};
        MSG msg{*GetCurrentMessage()};
        DBG_DUMP_WMESSAGE_EXT(LTH_CONTROL, L"WNDPROC", hWnd, uMsg, wParam, lParam);
        DBG_DUMP_WMESSAGE(LTH_CONTROL, L"CURRMSG", &msg);
        if (::IsDialogMessageW(m_ColorPicker.m_hWnd, &msg)) {
            //DBG_DUMP_WMESSAGE_EXT(LTH_COLORPICKER, L"CPDLG", hWnd, uMsg, wParam, lParam);
            return TRUE;
        }
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
    CPoint       ptLT{0, 0};
    if (m_nPosFlags & Rc::Left) {
        ptLT.x = rcParent.left - 2 - nCX;
    }
    else if (m_nPosFlags & Rc::Right) {
        ptLT.x = rcParent.right + 2;
    }
    else if (m_nPosFlags & Rc::XCenter) {
        ptLT.x = rcParent.left + (rcParent.Width() - nCX) / 2;
    }
    if (m_nPosFlags & Rc::Top) {
        ptLT.y = rcParent.top;
    }
    else if (m_nPosFlags & Rc::Bottom) {
        ptLT.y = rcParent.bottom;
    }
    else if (m_nPosFlags & Rc::YCenter) {
        ptLT.y = rcParent.top + (rcParent.Height() - nCY) / 2;
    }
    CPoint const ptRB{ptLT.x + nCX, ptLT.y + nCY};
    m_rcPlace.SetRect(ptLT, ptRB);
}

BOOL CColorPickerDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_PARAMETER(wndFocus);
    UNREFERENCED_PARAMETER(lInitParam);
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != nullptr);
        pLoop->AddMessageFilter(this);
    }
    PrepareRect(m_wndMaster);
    if (m_rcPlace.IsRectEmpty()) {
        CenterWindow(m_wndMaster);
    }
    else {
        MoveWindow(m_rcPlace, FALSE);
    }
    if (!m_bModalLoop) {
        WTL::CFontHandle const hFont{GetFont()};
        WTL::CLogFont       hLogFont{};
        hFont.GetLogFont(hLogFont);
        hLogFont.lfEscapement = 900;
        hLogFont.lfWeight = FW_BLACK;
        hLogFont.lfHeight -= 3;
        m_vFont.Attach(hLogFont.CreateFontIndirectW());
        m_ColorPicker.GetMasterColor().AddObserver(*this);
    }
    DlgResize_Init(m_bModalLoop, true, 0);
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_INITDIALOG OK for hWnd:%p\n"), m_hWnd);
    return TRUE;
}

void CColorPickerDlg::OnDestroy()
{
    m_ColorPicker.GetMasterColor().RemoveObserver(*this);
    m_wndMaster = nullptr;
    SetMsgHandled(FALSE);
    if (auto const* pAppModule = AppModulePtr()) {
        WTL::CMessageLoop* pLoop = pAppModule->GetMessageLoop();
        ATLASSERT(pLoop != nullptr);
        pLoop->RemoveMessageFilter(this);
    }
}

void CColorPickerDlg::OnCommand(UINT uNotifyCode, int nID, HWND)
{
    UNREFERENCED_PARAMETER(uNotifyCode);
    switch (nID) {
    case IDOK:
    case IDCANCEL:
        if (m_bModalLoop) { EndDialog(nID); }
        else              { ShowWindow(SW_HIDE); }
        break;
    default:
        break;
    }
}

void CColorPickerDlg::OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI) const
{
    switch (nID) {
    case IDC_DRAGBAR: {
        WTL::CDCHandle   dc{pDI->hDC};
        CRect        rcItem{pDI->rcItem};
        ATL::CStringW sItem{};
        dc.GradientFillRect(rcItem, 0x000000, m_ColorPicker.GetColorRef(), false);
        if (GetDlgItem(IDC_DRAGBAR).GetWindowTextW(sItem) < 1) {
            break;
        }
        int const iSave{dc.SaveDC()};
        //CPoint pt{rcItem.BottomRight()};
        dc.SetTextColor(0xffffff);
        dc.SetBkMode(TRANSPARENT);
        dc.SelectFont(m_vFont);
        CRect rcText{rcItem};
        //std::swap(rcText.left, rcText.right);
        //std::swap(rcText.top, rcText.bottom);
        //dc.DrawTextW(sItem.GetString(), sItem.GetLength(), rcText, DT_CALCRECT);
        //dc.TextOutW(pt.x, pt.y, sItem.GetString(), sItem.GetLength());
        dc.DrawTextW(sItem.GetString(), sItem.GetLength(), rcText, DT_BOTTOM | DT_SINGLELINE);
        dc.RestoreDC(iSave);
        break;
    }
    }
}

UINT CColorPickerDlg::OnNcHitTest(CPoint pt) const
{
    CRect rc{};
    GetDlgItem(IDC_DRAGBAR).GetWindowRect(rc);
    return rc.PtInRect(pt) ? HTCAPTION : HTCLIENT;
}
