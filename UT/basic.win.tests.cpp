#include "pch.hxx"
#include "dh.tracing.h"
#include "err.printer.h"
#include "dialogz.basic.h"
#include "dialogz.messagebox.h"
#include "rez/resource.h"
#include <gtest/gtest.h>
#include <atlconv.h>

struct TestBasics: ::testing::Test
{
};

struct TestBasicDlg: CF::BasicDialog
{
    using     Super = CF::BasicDialog;
    using Colorizer = Super::Colorizer;

    ~TestBasicDlg() override;
    TestBasicDlg();

private:
    CIcon            m_icon;
    WTL::CComboBox  m_Combo;
    WTL::CListBox m_ListBox;

    DECL_OVERRIDE_MSG_MAP_EX(TestBasicDlg);

    HICON LoadCustomIcon() override;
    void OnButtonClick(int id, UINT code) override;
    BOOL OnInitDialog(HWND, LPARAM);
};

TestBasicDlg::~TestBasicDlg()
{
}

TestBasicDlg::TestBasicDlg()
    : BasicDialog(IDD_TEST_BASICS_DLG0, L"Some text", L"Some title", CF::DialogAttrs::IconCustom | CF::DialogAttrs::YesNoCancel, nullptr)
{
}

IMPL_OVERRIDE_MSG_MAP_EX(TestBasicDlg)
    //HANDLE_SUPER_MSG_MAP_EX(Super)
    //if (Super::OnWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) {
    //    return TRUE;
    //}
    MSG_WM_INITDIALOG(OnInitDialog)
END_MSG_MAP()

HICON TestBasicDlg::LoadCustomIcon()
{
    CIconHandle icon;
    icon.LoadIconW(MAKEINTRESOURCEW(IDI_ALL_OK), 128, 128, LR_LOADTRANSPARENT | LR_VGACOLOR);
    return icon.m_hIcon;
}

BOOL TestBasicDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    m_icon.LoadIconW(MAKEINTRESOURCEW(IDI_WTL));
    SetIcon(m_icon, TRUE);
    SetIcon(m_icon, FALSE);
    if (0) {
        static const PCWSTR wtlControlClasses[] = {
            L"CStatic",
            L"CButton",
            L"CScrollBar",
            L"CComboBox",
            L"CEdit",
            L"CListBox",
            L"CHeaderCtrl",
            L"CLinkCtrl",
            L"CListViewCtrl",
            L"CTreeViewCtrl",
            L"CComboBoxEx",
            L"CTabCtrl",
            L"CIPAddressCtrl",
            L"CPagerCtrl",
            L"CProgressBarCtrl",
            L"CTrackBarCtrl",
            L"CUpDownCtrl",
            L"CDateTimePickerCtrl",
            L"CMonthCalendarCtrl",
            L"CRichEditCtrl",

        };

        m_Combo.Attach(GetDlgItem(IDC_COMBO1));
        for (const auto clsName: wtlControlClasses) {
            m_Combo.AddString(clsName);
        }
    }
    return TRUE;
}

void TestBasicDlg::OnButtonClick(int id, UINT code)
{
    //BasicDialog::OnButtonClick(id, code);
}

TEST_F(TestBasics, Dialogs)
{
    HINSTANCE hUT = GetModuleHandleW(nullptr);

    //CF::UserDialog::Ask(nullptr, L"WHAT???", L"Что это за слово???", MB_YESNO | MB_ICONWARNING);

    /**/
    //Rect rc(10, 10, 800, 600);
    TestBasicDlg dlg;
    if (!dlg.ShowModal(hUT)) {
        PrintLastError("Error in 'TestBasicDlg::ShowModal'");
        ASSERT_TRUE(false);
    }
    DH::ThreadPrintf(L"DLG result: %d\n", dlg.Result().Code);
    /**/
}
