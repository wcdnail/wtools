#pragma once

#include "resource.h"
#include <WTL/CColorButton.h>
#include <WTL/CColorCell.h>
#include <WTL/CColorPickerDlg.h>
#include <color.stuff.h>
#include <win32.fonts.h>
#include <dh.tracing.h>
#include <scoped.bool.guard.h>
#include <windows.gdi.rects.h>
#include <string.utils.error.code.h>
#include <dev.assistance/dev.assist.h>
#include <dev.assistance/debug.console/debug.console.h>
#include <atlwin.h>
#include <atlddx.h>
#include <atldlgs.h>
#include <atltheme.h>
#include <atlcrack.h>
#include <ranges>

struct CDefaultWin32Dlg: ATL::CDialogImpl<CDefaultWin32Dlg>,
                         WTL::CDialogResize<CDefaultWin32Dlg>,
                         WTL::CWinDataExchange<CDefaultWin32Dlg>,
                         WTL::CThemeImpl<CColorPickerDlg>,
                         WTL::CBufferedPaintImpl<CColorPickerDlg>,
                         WTL::CMessageFilter,
                         IColorObserver
{
    enum { IDD = IDD_DIALOG1 };

    bool          m_bLoadValues{false};
    CColorButton  m_btnMyColor1{};
    CColorButton  m_btnMyColor2{};
    CColorButton  m_btnMyColor3{};
    CColorCellEx      m_crCell1{0x000000, RGB_MAX_INT};
    CColorPickerDlg     m_cpDlg{0x1f2f3f};
    BOOL               m_brLeft{TRUE};
    BOOL            m_brXCenter{FALSE};
    BOOL              m_brRight{FALSE};
    BOOL                m_brTop{TRUE};
    BOOL            m_brYCenter{FALSE};
    BOOL             m_brBottom{FALSE};
    ATL::CString     m_sMvFlags{};
    UINT          m_cpMoveFlags{0};
    WTL::CFont          m_vFont{};
    int           m_nFontFamily{0};
    ATL::CString m_szFontFamily{L"Arial Black"};
    FontMap           m_FontMap{};
    WTL::CComboBox  m_FontCombo{};
    WTL::CPen      m_pnTextRect{nullptr};
    ATL::CString      m_sdrText{L"\
Мой дядя самых честных правил,\r\n\
Когда не в шутку занемог,\r\n\
Он уважать себя заставил\r\n\
И лучше выдумать не мог.\r\n\
..."};

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

    void OnColorUpdate(IColor const& clrSource) override
    {
        UNREFERENCED_ARG(clrSource);
        GetDlgItem(IDC_DRAWER).InvalidateRect(nullptr, FALSE);
    }

    HRESULT Initialize()
    {
        HRESULT hCode{S_OK};
        hCode = m_cpDlg.Initialize();
        if (FAILED(hCode)) {
            return hCode;
        }
        //hCode = m_FontCombo.PreCreateWindow();
        //if (FAILED(hCode)) {
        //    return hCode;
        //}
        return S_OK;
    }

    BEGIN_DDX_MAP(CDefaultWin32Dlg)
        DDX_CHECK(IDC_RADIO_LEFT, m_brLeft)
        DDX_CHECK(IDC_RADIO_XCENTER, m_brXCenter)
        DDX_CHECK(IDC_RADIO_RIGHT, m_brRight)
        DDX_CHECK(IDC_RADIO_TOP, m_brTop)
        DDX_CHECK(IDC_RADIO_YCENTER, m_brYCenter)
        DDX_CHECK(IDC_RADIO_BOTTOM, m_brBottom)
        DDX_COMBO_INDEX(IDC_DRAWER_FONT_COMBO, m_nFontFamily)
        if (!bSaveAndValidate) {
            DDX_TEXT(IDC_PICKER_MV_FLAGS, m_sMvFlags)
        }
        if (bSaveAndValidate && (nCtlID == static_cast<UINT>(-1))) {
            return TRUE;
        }
        DDX_TEXT(IDC_DRAWER_TEXT, m_sdrText)
    END_DDX_MAP()

    BEGIN_DLGRESIZE_MAP(CDefaultWin32Dlg)
        DLGRESIZE_CONTROL(IDC_BUTTON1, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_BUTTON2, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_BUTTON3, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_COLOR1, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_DRAWER, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_STA_DRAWER_PARAMS, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_DRAWER_FONT_COMBO, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_DRAWER_FONT_SEL, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_DEBUG_CONSOLE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP(CDefaultWin32Dlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_NOTIFY(OnNotify)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
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
            DBGTPrint(0, L"WM_NOTIFY id:%-4d nc:%-5d %s\n", nID, pnmh->code, DH::NotifyCodeToStrW(pnmh->code));
            break;
        }
        SetMsgHandled(FALSE);
        return 0;
    }

    void OnCommand(UINT uNotifyCode, int nID, HWND)
    {
        if (m_bLoadValues) {
            return ;
        }
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
        case IDC_DRAWER_TEXT:
            if (EN_CHANGE != uNotifyCode) {
                return ;
            }
            DoDataExchange(DDX_SAVE, nID);
            GetDlgItem(IDC_DRAWER).InvalidateRect(nullptr, FALSE);
            return ;

        case IDC_DRAWER_FONT_COMBO:
            if (CBN_SELENDOK != uNotifyCode) {
                return ;
            }
            DoDataExchange(DDX_SAVE, nID);
            FontUpdate();
            return ;
        case IDC_DRAWER_FONT_SEL: {
            WTL::CLogFont lf{};
            m_vFont.GetLogFont(lf);
            WTL::CFontDialog dlg{&lf};
            if (IDOK != dlg.DoModal()) {
                return ;
            }

            return ;
        }
        default:
            DBGTPrint(0, L"WM_COMMAND id:%-4d nc:%-5d %s\n", nID, uNotifyCode, DH::NotifyCodeToStrW(uNotifyCode));
            break;
        }
        SetMsgHandled(FALSE);
    }

    LRESULT OnEndDialog(int wID)
    {
        EndDialog(wID);
        return 0;
    }

    void OnWindowPosChanged(LPWINDOWPOS pWndPos)
    {
        m_cpDlg.FollowMaster(pWndPos);
        SetMsgHandled(FALSE);
    }

    HRESULT ThemedInit(PCWSTR pszClasses)
    {
        if (!m_hWnd) {
            return S_FALSE;
        }
        if (!IsThemingSupported()) {
            return S_FALSE;
        }
        ATLASSERT(::IsWindow(m_hWnd));
        const HTHEME hTheme{::OpenThemeDataEx(m_hWnd, pszClasses, OTD_FORCE_RECT_SIZING | OTD_NONCLIENT)};
        if (!hTheme) {
            auto const code{static_cast<HRESULT>(GetLastError())};
            return code;
        }
        m_hTheme = hTheme;
        return S_OK;
    }

    void FontSetFamily(ATL::CString const& szFamily)
    {
        CScopedBoolGuard guard{m_bLoadValues};
        const int nFont = m_FontCombo.FindStringExact(-1, szFamily.GetString());
        if (nFont < 0) {
            return ;
        }
        m_FontCombo.SetCurSel(nFont);
        m_nFontFamily = m_FontCombo.GetCurSel();
    }

    void FontInitialize()
    {
        m_FontCombo.ResetContent();
        int nIndex = 0;
        for (auto const& key : m_FontMap | std::views::keys) {
            if (L'@' == key[0]) {
                // ##TODO: skip @'at'ed font?
                continue;
            }
            int const nItem{m_FontCombo.AddString(key.c_str())};
            if (nItem < 0) {
                auto const hCode{static_cast<HRESULT>(GetLastError())};
                DH::TPrintf(TL_Warning, L"AddString['%s'] failed: 0x%08x %s\n", key.c_str(),
                    hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
                continue;
            }
            ++nIndex;
        }
    }

    void FontUpdate()
    {
        ATL::CString szTemp;
        if (m_FontCombo.GetLBText(m_nFontFamily, szTemp) < 1) {
            auto const hCode{static_cast<HRESULT>(GetLastError())};
            DH::TPrintf(TL_Warning, L"GetLBText[%d] failed: 0x%08x %s\n", m_nFontFamily,
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            return ;
        }
        m_szFontFamily = szTemp;
        
        // nHeight = -MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);

        m_vFont.Attach(nullptr);
        m_vFont.CreateFontW(-(12), 0, /*900*/ 0, 0, /*FW_BLACK*/ FW_NORMAL,
            0, 0, 0, RUSSIAN_CHARSET, 0, 0,
            CLEARTYPE_NATURAL_QUALITY, 0, 
            m_szFontFamily.GetString());

        GetDlgItem(IDC_DRAWER_TEXT).SetFont(m_vFont);
        GetDlgItem(IDC_DRAWER).InvalidateRect(nullptr, FALSE);
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        ModifyStyle(0xffffffff, WS_OVERLAPPEDWINDOW);
        ModifyStyleEx(0xffffffff, WS_EX_APPWINDOW);

        static PCWSTR const pszClasses{
            VSCLASS_BUTTON L";"
            VSCLASS_EDIT L";"
            VSCLASS_MENU L";"
            VSCLASS_SCROLLBAR L";"
            VSCLASS_WINDOW
        };
        HRESULT const hCode{ThemedInit(pszClasses)};
        if (FAILED(hCode)) {
            DH::TPrintf(TL_Warning, L"ThemedInit['%s'] failed: 0x%08x %s\n", pszClasses,
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
        }

        CFontDef::LoadAll(m_FontMap);
        m_FontCombo = GetDlgItem(IDC_DRAWER_FONT_COMBO);
        FontInitialize();
        FontSetFamily(m_szFontFamily);
        FontUpdate();

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

        m_crCell1.AddObserver(*this);
        m_crCell1.AddObserver(m_cpDlg);
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

        // Setup drawing tests
        m_pnTextRect.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        GetDlgItem(IDC_DRAWER).ModifyStyle(0, SS_OWNERDRAW);

        DlgResize_Init(true, true, 0);
        return TRUE;
    }

    void OnDrawItem(int nID, LPDRAWITEMSTRUCT pDI)
    {
        switch (nID) {
        case IDC_COLOR1:
        case IDC_DRAWER:
            break;
        default:
            SetMsgHandled(FALSE);
            return ;
        }
        WTL::CDCHandle const dc{pDI->hDC};
        CRect            rcItem{pDI->rcItem};
        HDC          dcBuffered{nullptr};
        if(IsBufferedPaintSupported()) {
            m_BufferedPaint.Begin(dc, rcItem, m_dwFormat, &m_PaintParams, &dcBuffered);
            if (dcBuffered) {
                DrawItem(nID, dcBuffered, rcItem);
            }
            m_BufferedPaint.End();
        }
        if (!dcBuffered) {
            DrawItem(nID, dc, rcItem);
        }
    }

    void DrawItem(int nID, WTL::CDCHandle dc, CRect& rc)
    {
        switch (nID) {
        case IDC_DRAWER:
            break;
        case IDC_COLOR1: 
            m_crCell1.Draw(dc, rc, nullptr);
            return ;
        default:
            return ;
        }
        int const         iSave{dc.SaveDC()};
        UINT constexpr nDTFlags{DT_CENTER | DT_VCENTER};
        CRect            rcText{};

        dc.GradientFillRect(rc, 0x000000, m_crCell1.GetColorRef(), false);

        HFONT const prevFont{dc.SelectFont(m_vFont)};
        if (dc.DrawTextW(m_sdrText.GetString(), m_sdrText.GetLength(), rcText, nDTFlags | DT_CALCRECT) <= 0) {
            HRESULT const hCode{static_cast<HRESULT>(GetLastError())};
            DH::TPrintf(TL_Warning, L"GetTextExtent failed: 0x%08x %s\n",
                hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
            rcText = rc;
        }
        else {
            Rc::PutInto(rc, rcText, Rc::Center);
        }

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
        DrawThemeTextEx(dc, 0, 0, m_sdrText.GetString(), m_sdrText.GetLength(), nDTFlags, rcText, &dtOptions);

        HBRUSH const prevBrush{dc.SelectStockBrush(NULL_BRUSH)};
        HPEN const     prevPen{dc.SelectPen(m_pnTextRect)};
        CRect          rcFrame{rcText};
        rcFrame.InflateRect(2, 2);
        CF::FrameRect(dc, rcFrame);

        dc.SelectBrush(prevBrush);
        dc.SelectPen(prevPen);
        dc.SelectFont(prevFont);
        dc.RestoreDC(iSave);
    }
};
