#include "pch.hxx"
#include "dh.tracing.h"
#include "rez/resource.h"
#include "err.printer.h"
#include <gtest/gtest.h>
#include <memory>
#include <map>

#include "dev.assistance/dev.assist.h"

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

    void DrawItem(HWND hWnd, HDC hdc, PAINTSTRUCT const* ps);
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

struct CItem: ATL::CWindowImpl<CItem, CWindow>
{
    ~CItem() override
    {}

    CItem(CTestDlg& master)
        : m_master(master)
        , m_class(-1)
    {}

    void Init()
    {
        m_class = DetermineWndClass(m_hWnd);
    }

    BEGIN_MSG_MAP(CItem)
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

    LRESULT OnNcPaint(UINT, WPARAM, LPARAM, BOOL&)
    {
        return 0;
    }

    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&)
    {
        PAINTSTRUCT ps;
        CPaintDC paintDc(m_hWnd);
        //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
        m_master.DrawItem(m_hWnd, paintDc.m_hDC, &paintDc.m_ps);
        EndPaint(&ps);
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

void CTestDlg::DrawItem(HWND hWnd, HDC hdc, PAINTSTRUCT const* ps)
{
}

TEST_F(TestDialogs, Customizabe)
{
    CTestDlg dlg;
    auto rv = dlg.DoModal();
    if (rv <= 0) {
        PrintLastError();
    }
}
