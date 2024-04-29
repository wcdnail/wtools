#include "pch.hxx"
#include "dialogz.basic.h"
#include "string.utils.error.code.h"
#include "rez/resource.h"
#include <gtest/gtest.h>
#include <atlconv.h>

struct TestBasics: ::testing::Test
{
    static void PrintLastError()
    {
        HRESULT   hr = static_cast<HRESULT>(GetLastError());
        CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(hr);
        CW2AEX<512> conv(msg.GetString(), CP_UTF8);
        fprintf_s(stderr, "DLG: Show failure %08x '%s'\n", hr, conv.m_psz);
    }
};

struct TestBasicDlg: CF::BasicDialog
{
    using     Super = CF::BasicDialog;
    using Colorizer = Super::Colorizer;

    ~TestBasicDlg() override;
    TestBasicDlg();

private:
    DECL_OVERRIDE_MSG_MAP_EX(TestBasicDlg);

    BOOL OnInitDialog(HWND, LPARAM);
};

IMPL_OVERRIDE_MSG_MAP_EX(TestBasicDlg)
    HANDLE_SUPER_MSG_MAP_EX(Super)
    MSG_WM_INITDIALOG(OnInitDialog)
END_MSG_MAP()

BOOL TestBasicDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    return TRUE;
}

TestBasicDlg::~TestBasicDlg()
{
}

TestBasicDlg::TestBasicDlg()
    : BasicDialog(IDD_TEST_BASICS_DLG0, 0)
{
}

TEST_F(TestBasics, Dialogs)
{
    HINSTANCE hUT = GetModuleHandleW(nullptr);

    //Rect rc(10, 10, 800, 600);
    TestBasicDlg dlg;
    if (!dlg.ShowModal(hUT)) {
        PrintLastError();
        ASSERT_TRUE(false);
    }
}
