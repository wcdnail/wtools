#include "stdafx.h"
#include "CColorPickerDlg.h"
#include "CAppModuleRef.h"
#include "IColor.h"
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>

#define _NEED_WMESSAGE_DUMP 1
#include <debug.dump.msg.h>

CColorPickerDlg::~CColorPickerDlg() = default;

CColorPickerDlg::CColorPickerDlg(COLORREF crColor)
    : m_ColorPicker{crColor}
    ,     m_rcPlace{0, 0, 0, 0}
    ,  m_bModalLoop{false}
    ,   m_nPosFlags{Rc::Right}
    ,   m_wndMaster{nullptr}
    ,    m_rcMaster{}
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
    m_bModalLoop = bModal;
    m_nPosFlags = nPosFlags;
    m_wndMaster = hWndMaster;   // in DoModal hWndMaster passed to DialogBoxIndirectParam
                                // BUT My::GetParrent will return NULL!
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
    DH::TPrintf(0, L"%s failed: 0x%08x %s\n", sFunc.c_str(), hCode, msg.GetString());
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
    DWORD                    dwStyle{WS_POPUP | WS_THICKFRAME | DS_CONTROL};
    DWORD                  dwExStyle{WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE};
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
        CONTROL_CTEXT(_T("Color Picker"), IDC_DRAGBAR, 1, 4, DRGB_CX+2, nPickerCY-6, SS_OWNERDRAW, 0)
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
    m_ColorPicker.SetColor(clrSource.GetColorRef(), clrSource.GetAlpha(), false, false);
    GetDlgItem(IDC_DRAGBAR).InvalidateRect(nullptr, FALSE);
}

static UINT CCPD_OnGetDlgCode(LPMSG pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);
    if (pMsg) {
        DBG_DUMP_WMESSAGE(0, L"WNDPROC", pMsg);
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
        MSG_WM_WINDOWPOSCHANGING(OnWindowPosx)
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
        DBG_DUMP_WMESSAGE_EXT(0, L"WNDPROC", hWnd, uMsg, wParam, lParam);
        DBG_DUMP_WMESSAGE(0, L"CURRMSG", &msg);
        if (::IsDialogMessageW(m_ColorPicker.m_hWnd, &msg)) {
            //DBG_DUMP_WMESSAGE_EXT(LTH_COLORPICKER, L"CPDLG", hWnd, uMsg, wParam, lParam);
            return TRUE;
        }
    }
    return FALSE;
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
    if (m_wndMaster) {
        MoveWindow(m_nPosFlags);
    }
    else {
        CenterWindow();
    }
    if (!m_bModalLoop) {
        HRESULT const hCode{ThemedInit()};
        if (FAILED(hCode)) {
            DH::TPrintf(TL_Error, L"ThemedInit failed: 0x%08x %s\n",
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
        }
        // nHeight = -MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        m_vFont.CreateFontW(-(DRGB_CX), 0, 900, 0, FW_BLACK,
            0, 0, 0, RUSSIAN_CHARSET, 0, 0,
            CLEARTYPE_NATURAL_QUALITY, 0, 
            L"Arial Black");
        m_ColorPicker.GetMasterColor().AddObserver(*this);
    }
    DlgResize_Init(m_bModalLoop, true, 0);
    DH::TPrintf(0, _T("%s OK for %p\n"), __FUNCTIONW__, this);
    return TRUE;
}

void CColorPickerDlg::OnDestroy()
{
    m_ColorPicker.GetMasterColor().RemoveObserver(*this);
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

HRESULT CColorPickerDlg::ThemedInit()
{
    // All class names are in $(PROJECT_DIR)\resz\ClassesNames.txt
    //
    // Short list:
    // BUTTON, CLOCK, COMBOBOX, COMMUNICATIONS, CONTROLPANEL, DATEPICKER, DRAGDROP, 
    // EDIT, EXPLORERBAR, FLYOUT, GLOBALS, HEADER, LISTBOX, LISTVIEW, MENU, MENUBAND, 
    // NAVIGATION, PAGE, PROGRESS, REBAR, SCROLLBAR, SEARCHEDITBOX, SPIN, STARTPANEL, 
    // STATUS, TAB, TASKBAND, TASKBAR, TASKDIALOG, TEXTSTYLE, TOOLBAR, TOOLTIP, 
    // TRACKBAR, TRAYNOTIFY, TREEVIEW, WINDOW
    //
    // https://learn.microsoft.com/en-us/windows/win32/controls/parts-and-states?redirectedfrom=MSDN
    //
    if (!m_hWnd) {
        return S_FALSE;
    }
    if (!IsThemingSupported()) {
        return S_FALSE;
    }
    ATLASSERT(::IsWindow(m_hWnd));
    static const PCWSTR pszClasses{L"WINDOW"};
    const HTHEME hTheme{::OpenThemeDataEx(m_hWnd, pszClasses, OTD_FORCE_RECT_SIZING | OTD_NONCLIENT)};
    if (!hTheme) {
        auto const code{static_cast<HRESULT>(GetLastError())};
        return code;
    }
    m_hTheme = hTheme;
    return S_OK;
}

void CColorPickerDlg::DrawDragBar(int nID, WTL::CDCHandle dc, CRect& rcItem)
{
    ATL::CStringW sItem{};
    dc.GradientFillRect(rcItem, 0x000000, m_ColorPicker.GetColorRef(), false);
    if (GetDlgItem(nID).GetWindowTextW(sItem) < 1) {
        return ;
    }
    int const iSave{dc.SaveDC()};
    dc.SetTextColor(0xffffff);
    dc.SetBkMode(TRANSPARENT);
    dc.SelectFont(m_vFont);
    CRect rcText{rcItem};
    DTTOPTS constexpr dtOptions{
        /* dwSize;              */ sizeof(dtOptions),
        /* dwFlags;             */ DTT_TEXTCOLOR | DTT_SHADOWCOLOR | DTT_SHADOWTYPE | DTT_SHADOWOFFSET | DTT_GLOWSIZE,
        /* crText;              */ 0xffffff,
        /* crBorder;            */ 0,
        /* crShadow;            */ 0x000000,
        /* iTextShadowType;     */ TST_CONTINUOUS,
        /* ptShadowOffset;      */ { 3, 2 },
        /* iBorderSize;         */ 0,
        /* iFontPropId;         */ 0,
        /* iColorPropId;        */ 0,
        /* iStateId;            */ 0,
        /* fApplyOverlay;       */ FALSE,
        /* iGlowSize;           */ 32,
        /* pfnDrawTextCallback; */ nullptr,
        /* lParam;              */ 0
    };
    DrawThemeTextEx(dc, 0, 0, sItem.GetString(), sItem.GetLength(), DT_BOTTOM | DT_SINGLELINE, rcText, &dtOptions);
    dc.RestoreDC(iSave);
}

void CColorPickerDlg::OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI)
{
    switch (nID) {
    case IDC_DRAGBAR: {
        WTL::CDCHandle const dc{pDI->hDC};
        CRect            rcItem{pDI->rcItem};
        HDC          dcBuffered{nullptr};
        if(IsBufferedPaintSupported()) {
            m_BufferedPaint.Begin(dc, rcItem, m_dwFormat, &m_PaintParams, &dcBuffered);
            if (dcBuffered) {
                DrawDragBar(nID, dcBuffered, rcItem);
            }
            m_BufferedPaint.End();
        }
        if (!dcBuffered) {
            DrawDragBar(nID, dc, rcItem);
        }
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

bool CColorPickerDlg::PrepareRect(ATL::CWindow wndParent)
{
    if (!wndParent.m_hWnd) {
        return false;
    }
    CRect      rcMy{};
    HWND   hWndLoop{nullptr};
    HWND hWndMaster{wndParent.m_hWnd};
    while (nullptr != (hWndLoop = ::GetParent(hWndMaster))) {
        hWndMaster = hWndLoop;
    }
    ::GetWindowRect(hWndMaster, m_rcMaster);
    GetWindowRect(rcMy);

    LONG const    nCX{rcMy.Width()};
    LONG const    nCY{rcMy.Height()};
    CPoint       ptLT{0, 0};

    if (m_nPosFlags & Rc::Top)          {
        if (m_nPosFlags & Rc::XCenter)  { ptLT.y = m_rcMaster.top - nCY; }
        else                            { ptLT.y = m_rcMaster.top; } }
    else if (m_nPosFlags & Rc::Bottom)  {
        if (m_nPosFlags & Rc::XCenter)  { ptLT.y = m_rcMaster.bottom; }
        else                            { ptLT.y = m_rcMaster.bottom - nCY; } }
    else if (m_nPosFlags & Rc::YCenter) { ptLT.y = m_rcMaster.top + (m_rcMaster.Height() - nCY) / 2; }

    if (m_nPosFlags & Rc::Left)         { ptLT.x = m_rcMaster.left - 2 - nCX; }
    else if (m_nPosFlags & Rc::Right)   { ptLT.x = m_rcMaster.right + 2; }
    else if (m_nPosFlags & Rc::XCenter) { ptLT.x = m_rcMaster.left + (m_rcMaster.Width() - nCX) / 2; }

    CPoint const ptRB{ptLT.x + nCX, ptLT.y + nCY};
    m_rcPlace.SetRect(ptLT, ptRB);
    return true;
}

void CColorPickerDlg::MoveWindow(unsigned nPosFlags)
{
    if (!m_hWnd) {
        return ;
    }
    m_nPosFlags = nPosFlags;
    if (PrepareRect(m_wndMaster)) {
        WndSuper::MoveWindow(m_rcPlace, FALSE);
    }
}

void CColorPickerDlg::FollowMaster(LPWINDOWPOS pWndPos)
{
    if (!m_hWnd || !pWndPos) {
        return ;
    }

    CPoint const  ptNew{pWndPos->x, pWndPos->y};
    CPoint const ptDiff{ptNew - m_rcMaster.TopLeft()};

    GetWindowRect(m_rcPlace);
    m_rcPlace.OffsetRect(ptDiff);
    WndSuper::MoveWindow(m_rcPlace, FALSE);

    m_rcMaster.SetRect(pWndPos->x, pWndPos->y, pWndPos->x + pWndPos->cx, pWndPos->y + pWndPos->cy);
}

void CColorPickerDlg::OnWindowPosx(LPWINDOWPOS pWPos)
{
    SetMsgHandled(FALSE);
    if (!pWPos) {
        return ;
    }
    // Prevent double-buffered painting artefacts during window movin
    pWPos->flags |= SWP_NOREDRAW;
}
