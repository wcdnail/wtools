#include "pch.hxx"
#include "dialogz.basic.h"
#include "string.utils.error.code.h"
#include "rez/resource.h"
#include <gtest/gtest.h>
#include <atlconv.h>

struct TestBasics: ::testing::Test
{
};

struct TestBasicDlg: CF::BasicDialog
{
    ~TestBasicDlg() override;
    TestBasicDlg();
};

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
        HRESULT   hr = static_cast<HRESULT>(GetLastError());
        CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(hr);
        CW2AEX<512> conv(msg.GetString(), CP_UTF8);
        fprintf_s(stderr, "DLG: Show failure %08x '%s'\n", hr, conv.m_psz);
        ASSERT_TRUE(false);
    }
}
