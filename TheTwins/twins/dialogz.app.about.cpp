#include "stdafx.h"
#include "dialogz.app.about.h"
#include <info/runtime.information.h>
#include <twins.langs/twins.lang.strings.h>
#include "res/resource.h"

namespace Twins
{
    std::wstring AboutDialog::AppUrl = L"http://vk.com/wcd_michael";

    AboutDialog::AboutDialog()
        : Super(IDD_TWINS_ABOUT, NULL)
        , Background(GetStockBrush(WHITE_BRUSH))
        , HeaderFont(::CreateFont(-14, 0, 0, 0, FW_HEAVY, 0, 1, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Arial Black")))
        , Header2Font(::CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Tahoma")))
        , VersionFont(::CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Tahoma")))
        , AuthorsFont(::CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Consolas")))
        , Header()
        , Header2()
        , Software()
        , License()
        , Authors()
    {}

    AboutDialog::~AboutDialog()
    {}

    BOOL AboutDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        DDX_CONTROL_HANDLE(IDC_AA_HEADER, Header);
        DDX_CONTROL_HANDLE(IDC_AA_HEADER2, Header2);
        DDX_CONTROL_HANDLE(IDC_AA_SOFTWARE, Software);
        DDX_CONTROL_HANDLE(IDC_AA_LICENSE, License);
        DDX_CONTROL_HANDLE(IDC_AA_AUTHORS, Authors);

        return TRUE;
    }

    template <typename Filler>
    static void InitText(CWindow& control, Filler filler)
    {
        CStringW text;
        control.GetWindowText(text);

        std::wstring wText(text.GetBuffer(), text.GetLength());
        filler(wText);

        control.SetWindowText(wText.c_str());
    }

    template <typename Filler>
    static void InitText(CEdit& control, Filler filler)
    {
        CStringW text;
        control.GetWindowText(text);

        std::wstring wText(text.GetBuffer(), text.GetLength());
        filler(wText);

        control.SetSel(0, -1);
        control.ReplaceSel(wText.c_str(), FALSE);
    }

    void InitHeader2(std::wstring& text);
    void InitSoftware(std::wstring& text);
    void InitLicense(std::wstring& text);
    void InitAuthors(std::wstring& text);

    BOOL AboutDialog::OnInitDialog(HWND, LPARAM param)
    {
        DoDataExchange(DDX_LOAD);

        LocalizeControl(m_hWnd, IDC_AA_HEADER);
        LocalizeControl(m_hWnd, IDC_AA_HEADER2);

        Header.SetCursor(::LoadCursor(NULL, IDC_HAND));

      //AddButton(_LS(StrId_Feedback), ResultReply);
        AddButton(_LS(StrId_Close), ResultOk);

        InitText(Header2, InitHeader2);
        InitText(Software, InitSoftware);
        InitText(License, InitLicense);
        InitText(Authors, InitAuthors);

        return TRUE;
    }

    void AboutDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        SetMsgHandled(FALSE);
    }

    BOOL AboutDialog::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillRect(rc, Background);
        return TRUE;
    }

    HBRUSH AboutDialog::OnCtlColor(CDCHandle dc, HWND control)
    {
        COLORREF color = 0x000000;

        if (control == Header.m_hWnd)
        {
            dc.SelectFont(HeaderFont);
            color = 0x6f0000;
        }
        else if (control == Header2.m_hWnd)
        {
            dc.SelectFont(Header2Font);
            color = 0x7f7f7f;
        }
        else if (control == Software.m_hWnd)
            dc.SelectFont(VersionFont);

        else if (control == Authors.m_hWnd)
        {
            dc.SelectFont(AuthorsFont);
            color = 0x7f0f0f;
        }

        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(color);

        return Background;
    }

    void AboutDialog::OnMouseDown(UINT flags, CPoint point)
    {
        if (!AppUrl.empty())
        {
            CRect rcHeader;
            Header.GetWindowRect(rcHeader);
            ScreenToClient(rcHeader);

            if (rcHeader.PtInRect(point))
            {
                std::wstring url = AppUrl;
                //url += L"";
                ::ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }
        
        //EndDialog(ResultCancel);
    }

    static void InitHeader2(std::wstring& text)
    {
        text += L"\r\n" + _L(StrId_IndependedDeveloper2012);
    }

    static void InitSoftware(std::wstring& text)
    {
        Runtime::InfoStore const& info = Runtime::Info();

        std::wostringstream temp;
        temp << "\r\n• " << info.Executable.Version.ProductName << " " << info.Executable.Version.ProductVersion << " " << info.TargetOs
             << "\r\n  ( " << info.Compiler
             << "\r\n  , " << info.CLib
             << "\r\n  , BOOST " << info.BoostVersion
             << "\r\n  )"
             ;

        text = temp.str();
    }

    static void InitLicense(std::wstring& text)
    {
        text += _L(StrId_Testingbuildnolicenserequired);
    }

    static void InitAuthors(std::wstring& text)
    {
        std::wostringstream temp;
        temp << L"\r\n" << _LS(StrId_Authors) << L":"
             << L"\r\n  • WCDNail, muxmoh@yandex.ru"
                L"\r\n"
                L"\r\n" << _LS(StrId_Thanksforassistance) << L":"
                L"\r\n  • " << _LS(StrId_TomywifeNadineallowedmetoworkevening)
             << L"\r\n  • boost.org"
                L"\r\n  • stackoverflow.com"
                L"\r\n  • codeproject.com"
                L"\r\n      • WTL Tab control stuff"
             // http://www.codeproject.com/Articles/2431/Custom-Tab-Controls-Tabbed-Frame-and-Tabbed-MDI
                L"\r\n          • Bjarke Viksoe, bjarke@viksoe.dk"
                L"\r\n          • Daniel Bowen, dbowen@es.com"
                L"\r\n          • Peter Carlson"
                L"\r\n      • WTL Cool menu bar"
             // http://www.codeproject.com/Articles/493/Cool-WTL-menu-with-Gradient-Sidebar
                L"\r\n          • Ramon Smits"
                L"\r\n      • Shell menus, drag'n'drop, WTL usefully stuff"
                L"\r\n          • Michael Dunn"
                L"\r\n"
                L"\r\n"
             ;

        text += temp.str();
    }
}
