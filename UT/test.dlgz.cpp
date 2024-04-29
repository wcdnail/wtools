#include "pch.hxx"
#include "dh.tracing.h"
#include "err.printer.h"
#include "wtl.control.h"
#include "rez/resource.h"
#include "dev.assistance/dev.assist.h"
#include <gtest/gtest.h>
#include <memory>
#include <map>

struct TestDialogs: ::testing::Test
{
};

//
// https://github.com/dracula/dracula-theme
//
static constexpr COLORREF CLR_Background  = RGB( 40,  42,  54); // #282a36
static constexpr COLORREF CLR_CurrentLine = RGB( 68,  71,  90); // #44475a
static constexpr COLORREF CLR_Selection   = RGB( 68,  71,  90); // #44475a
static constexpr COLORREF CLR_Foreground  = RGB(248, 248, 242); // #f8f8f2
static constexpr COLORREF CLR_Comment     = RGB( 98, 114, 164); // #6272a4
static constexpr COLORREF CLR_Cyan        = RGB(139, 233, 253); // #8be9fd
static constexpr COLORREF CLR_Green       = RGB( 80, 250, 123); // #50fa7b
static constexpr COLORREF CLR_Orange      = RGB(255, 184, 108); // #ffb86c
static constexpr COLORREF CLR_Pink        = RGB(255, 121, 198); // #ff79c6
static constexpr COLORREF CLR_Purple      = RGB(189, 147, 249); // #bd93f9
static constexpr COLORREF CLR_Red         = RGB(255,  85,  85); // #ff5555
static constexpr COLORREF CLR_Yellow      = RGB(241, 250, 140); // #f1fa8c

static constexpr COLORREF WND_BK_COLOR    = CLR_Background;
static constexpr COLORREF WND_TXT_COLOR   = CLR_Foreground;
static const       HBRUSH WND_BK_BRUSH    = CreateSolidBrush(WND_BK_COLOR);
static const         HPEN WND_EDGE_PEN    = CreatePen(0, 1, CLR_Orange);

struct WClass
{
    enum : int
    {
        Unknown = -1,
        First = 0,
        Static = First,
        Button,
        ScrollBar,
        ComboBox,
        Edit,
        ListBox,
        HeaderCtrl,
        LinkCtrl,
        ListViewCtrl,
        TreeViewCtrl,
        ComboBoxEx,
        TabCtrl,
        IPAddressCtrl,
        PagerCtrl,
        ProgressBarCtrl,
        TrackBarCtrl,
        UpDownCtrl,
        DateTimePickerCtrl,
        MonthCalendarCtrl,
        RichEditCtrl,
        Last
    };
};

int DetermineWndClass(HWND hWnd)
{
    struct WClassDef
    {
        PCWSTR name;
        int      id;
    };
    static const WClassDef wtlClasses[] = {
        {             WTL::CStatic::GetWndClassName(), WClass::Static },
        {             WTL::CButton::GetWndClassName(), WClass::Button },
        {          WTL::CScrollBar::GetWndClassName(), WClass::ScrollBar },
        {           WTL::CComboBox::GetWndClassName(), WClass::ComboBox },
        {               WTL::CEdit::GetWndClassName(), WClass::Edit },
        {            WTL::CListBox::GetWndClassName(), WClass::ListBox },
        {         WTL::CHeaderCtrl::GetWndClassName(), WClass::HeaderCtrl },
        {           WTL::CLinkCtrl::GetWndClassName(), WClass::LinkCtrl },
        {       WTL::CListViewCtrl::GetWndClassName(), WClass::ListViewCtrl },
        {       WTL::CTreeViewCtrl::GetWndClassName(), WClass::TreeViewCtrl },
        {         WTL::CComboBoxEx::GetWndClassName(), WClass::ComboBoxEx },
        {            WTL::CTabCtrl::GetWndClassName(), WClass::TabCtrl },
        {      WTL::CIPAddressCtrl::GetWndClassName(), WClass::IPAddressCtrl },
        {          WTL::CPagerCtrl::GetWndClassName(), WClass::PagerCtrl },
        {    WTL::CProgressBarCtrl::GetWndClassName(), WClass::ProgressBarCtrl },
        {       WTL::CTrackBarCtrl::GetWndClassName(), WClass::TrackBarCtrl },
        {         WTL::CUpDownCtrl::GetWndClassName(), WClass::UpDownCtrl },
        { WTL::CDateTimePickerCtrl::GetWndClassName(), WClass::DateTimePickerCtrl },
        {  WTL::CMonthCalendarCtrl::GetWndClassName(), WClass::MonthCalendarCtrl },
        {       WTL::CRichEditCtrl::GetWndClassName(), WClass::RichEditCtrl },
    };
    wchar_t  _class[512] = { 0 };
    UINT    classNameLen = ::RealGetWindowClassW(hWnd, _class, _countof(_class) - 1);
    for (const auto it: wtlClasses) {
        if (0 == _wcsnicmp(_class, it.name, classNameLen)) {
            return it.id;
        }
    }
    return WClass::Unknown;
}

void WinDebugDump(HWND hWnd, PCWSTR prefix)
{
#ifdef _DEBUG
    CWindow temp(hWnd);
    wchar_t  _class[512] = { 0 };
    CStringW   text;
    CStringW     id;
    temp.GetWindowTextW(text);
    id.Format(L"%04d", temp.GetDlgCtrlID());
    temp.Detach();
    ::RealGetWindowClassW(hWnd, _class, _countof(_class) - 1);
    DH::ThreadPrintfc(DH::Category::WTL(), L"%s %p::'%s'(%d) [%s:%s]\n", prefix, hWnd, 
        _class, DetermineWndClass(hWnd),
        text.GetString(), id.GetString()
    );
#endif
}

struct CItem;

using CItemPtr = std::unique_ptr<CItem>;
using CItemMap = std::map<HWND, CItemPtr>;

struct CTestDlg : public CDialogImpl<CTestDlg, CWindow>
              // public WTL::CDialogResize<CMainFrame>
{
    using  Thiz = CTestDlg;
    using Super = CDialogImpl<CTestDlg, CWindow>;

    enum { IDD = IDD_TEST_GENERIC };

    CItemMap m_Items;

    BEGIN_MSG_MAP(CAboutDlg)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_CTLCOLOR, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORLISTBOX, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORSCROLLBAR, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
    END_MSG_MAP()

    LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(wID);
        return 0;
    }

    static BOOL CALLBACK EnumItems(HWND hWnd, Thiz* thiz);

    BOOL AddItem(HWND hWnd);

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);

    static void EraseBkgnd(HWND hWnd, CDCHandle& dc, CRect& rc)
    {
        ::GetClientRect(hWnd, rc);
        //dc.FillSolidRect(rc, WND_BK_COLOR);
        ::FillRect(dc, &rc, WND_BK_BRUSH);
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDCHandle dc(GET_WM_CTLCOLOR_HDC(wParam, lParam, uMsg));
        CRect rcClient;
        EraseBkgnd(m_hWnd, dc, rcClient);
        bHandled = TRUE;
        return 0;
    }

    LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDCHandle dc(GET_WM_CTLCOLOR_HDC(wParam, lParam, uMsg));
      //HWND hWnd = GET_WM_CTLCOLOR_HWND(wParam, lParam, uMsg);
      //WORD type = GET_WM_CTLCOLOR_TYPE(wParam, lParam, uMsg);

        dc.SetBkColor(WND_BK_COLOR);
        dc.SetTextColor(WND_TXT_COLOR);

        bHandled = TRUE;
        return reinterpret_cast<LRESULT>(WND_BK_BRUSH);
    }

    bool NcDrawItem(CItem const& item, HDC hdc);
    bool DrawItem(CItem const& item, HDC hdc, PAINTSTRUCT* ps);
};

//
//    CWindowImplRoot<TBase>: -> TBase, -> CMessageMap
//      |
//      -- CWindowImplRoot<TBase>
//             |
//             -- CWindowImplBaseT<TBase, TWinTraits>
//                    |
//                    -- CWindowImpl<T, TBase, TWinTraits>
//                                   |
//                                   -- This class
//

struct CItem: ATL::CWindowImpl<CItem, CF::ControlBase>
{
    ~CItem() override
    {}

    CItem(CTestDlg& master)
        : m_master(master)
        , m_class(-1)
    {}

    void Init() { m_class = DetermineWndClass(m_hWnd); }
    int ClassId() const { return m_class; }

    BEGIN_MSG_MAP(CItem)
        if (CF::ControlBase::OnWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)) {
            return TRUE;
        }
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        CHAIN_MSG_MAP_MEMBER(m_master)
    END_MSG_MAP()

    LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        WinDebugDump(m_hWnd, L"CC++");
        return 0;
    }

    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        WinDebugDump(m_hWnd, L"  --");
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnNcPaint(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
    {
        HRGN hRgn = (HRGN)wParam;
        CDC dc(GetDCEx(hRgn, DCX_WINDOW | DCX_INTERSECTRGN));
        if (!m_master.NcDrawItem(*this, dc.m_hDC)) {
            bHandled = false;
        }
        return 0;
    }

    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);
        dc.SelectFont(GetFont());
      //FillRect(dc.m_hDC, &dc.m_ps.rcPaint, WND_BK_BRUSH);
        if (!m_master.DrawItem(*this, dc.m_hDC, &dc.m_ps)) {
            bHandled = false;
        }
        return 0;
    }

private:
    CTestDlg& m_master;
    int        m_class;
};

BOOL CTestDlg::EnumItems(HWND hWnd, Thiz* thiz)
{
    return thiz->AddItem(hWnd);
}

BOOL CTestDlg::AddItem(HWND hWnd)
{
    auto it = std::make_unique<CItem>(*this);
    if (!it->SubclassWindow(hWnd)) {
        return FALSE;
    }
    it->Init();
    WinDebugDump(it->m_hWnd, L"  ++");
    m_Items[hWnd] = std::move(it);
    return TRUE;
}

LRESULT CTestDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    WinDebugDump(m_hWnd, L"()");
    EnumChildWindows(m_hWnd, reinterpret_cast<WNDENUMPROC>(&Thiz::EnumItems), reinterpret_cast<LPARAM>(this));
    return TRUE;
}

bool CTestDlg::NcDrawItem(CItem const& item, HDC hdc)
{
    return true;
}

bool CTestDlg::DrawItem(CItem const& item, HDC hdc, PAINTSTRUCT* ps)
{
    if (WClass::Unknown == item.ClassId()) {
        return false;
    }
    UINT  tflags = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
    ULONG  style = static_cast<ULONG>(::GetWindowLongW(item.m_hWnd, GWL_STYLE));
    ULONG estyle = static_cast<ULONG>(::GetWindowLongW(item.m_hWnd, GWL_EXSTYLE));
    ULONG   type = 0;
    switch (item.ClassId()) {
    case WClass::Static: {
        // style == SS_SUNKEN
        type  = style & SS_TYPEMASK; // SS_ICON...SS_USERITEM
        // SS_OWNERDRAW...SS_ETCHEDFRAME
        tflags = style & 3 | DT_EXPANDTABS;
        tflags |= (style & SS_CENTERIMAGE ? DT_VCENTER | DT_SINGLELINE : 0)
                | (style & SS_ENDELLIPSIS ? DT_END_ELLIPSIS : 0)
                | (style & SS_PATHELLIPSIS ? DT_PATH_ELLIPSIS : 0)
                | (style & SS_WORDELLIPSIS ? DT_WORD_ELLIPSIS : 0)
                ;
        break;
    }
    case WClass::Button: {
        type = style & BS_TYPEMASK;
        UINT ha = BS_CENTER & style;
        UINT va = BS_VCENTER & style;
        UINT defaultHa = static_cast<UINT>((type < BS_CHECKBOX) || (BS_FLAT == type) ? DT_CENTER : DT_LEFT);
        tflags = (style & BS_MULTILINE ? 0 : DT_SINGLELINE)
                | (!ha ? defaultHa : (BS_CENTER == ha ? DT_CENTER : (BS_RIGHT == ha ? DT_RIGHT : DT_LEFT)))
                | (!va ? DT_VCENTER : (BS_VCENTER == va ? DT_VCENTER : (BS_BOTTOM == va ? DT_BOTTOM : DT_TOP)))
                ;
        break;
    }
    case WClass::ScrollBar:          break;
    case WClass::ComboBox:           break;
    case WClass::Edit:               break;
    case WClass::ListBox:            break;
    case WClass::HeaderCtrl:         break;
    case WClass::LinkCtrl:           break;
    case WClass::ListViewCtrl:       break;
    case WClass::TreeViewCtrl:       break;
    case WClass::ComboBoxEx:         break;
    case WClass::TabCtrl:            break;
    case WClass::IPAddressCtrl:      break;
    case WClass::PagerCtrl:          break;
    case WClass::ProgressBarCtrl:    break;
    case WClass::TrackBarCtrl:       break;
    case WClass::UpDownCtrl:         break;
    case WClass::DateTimePickerCtrl: break;
    case WClass::MonthCalendarCtrl:  break;
    case WClass::RichEditCtrl:       break;
    default:
        return false;
    }

    CStringW text;
    CRect  rcItem;
    PRECT   rcTrg = &rcItem;

    item.GetWindowTextW(text);
    if (nullptr != ps) {
        rcTrg = &ps->rcPaint;
    }
    else {
        item.GetClientRect(rcTrg);
    }

    ::SetBkColor(hdc, WND_BK_COLOR);
    ::SetTextColor(hdc, WND_TXT_COLOR);

    if (text.GetLength() > 0) {
        ::SetTextColor(hdc, WND_TXT_COLOR);
        ::DrawTextW(hdc, text, text.GetLength(), rcTrg, tflags);
    }
    return true;
}

TEST_F(TestDialogs, Customizabe)
{
    CTestDlg dlg;
    auto rv = dlg.DoModal();
    if (rv <= 0) {
        PrintLastError();
    }
}
