#include "stdafx.h"
#include "dialogz.basic.h"
#include "windows.icons.h"

#ifdef _MSC_VER
#  pragma warning(disable: 5045) // 5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#endif

namespace CF
{
    BasicDialog::~BasicDialog()
    {
    }

    BasicDialog::BasicDialog(UINT idd, WStrView message, WStrView title, unsigned flags, HICON icon) 
        :        IDD(idd)
        ,   m_result()
        ,    m_attrs(flags)
        ,    m_title(title)
        ,     m_text(message)
        ,     m_icon(icon)
        ,     m_font(static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT)))
        , m_textFont(::CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Consolas")))
    {
    }

    BasicDialog::BasicDialog(WStrView message, WStrView title, unsigned flags)
        : BasicDialog(IDD_BASIC, message, title, flags, nullptr)
    {
    }

    BasicDialog::BasicDialog(UINT idd, unsigned flags)
        : BasicDialog(idd, {}, {}, flags, nullptr)
    {
    }

    IMPL_MSG_MAP_EX(BasicDialog)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_COMMAND(OnCommand)
        CHAIN_MSG_MAP_CUST(Colorizer::ProcessColorizerMessage);
    END_MSG_MAP()

    unsigned BasicDialog::GetCompatFlags(unsigned flags)
    {
        unsigned rv = 0;
        switch (flags & 0xf) {
        case MB_OK: rv |= DialogAttrs::Ok; break;
        case MB_OKCANCEL: rv |= DialogAttrs::OkCancel; break;
        case MB_ABORTRETRYIGNORE: rv |= DialogAttrs::AbortRetryIgnore; break;
        case MB_YESNOCANCEL: rv |= DialogAttrs::YesNoCancel; break;
        case MB_YESNO: rv |= DialogAttrs::YesNo; break;
        case MB_RETRYCANCEL: rv |= DialogAttrs::RetryCancel; break;
#if (WINVER >= 0x0500)
        case MB_CANCELTRYCONTINUE: rv |= DialogAttrs::CancelRetryContinue; break;
#endif
        }

        switch (flags & 0xf0)
        {
        case MB_ICONHAND: rv |= DialogAttrs::IconStop; break;
        case MB_ICONQUESTION: rv |= DialogAttrs::IconQuestion; break;
        case MB_ICONEXCLAMATION: rv |= DialogAttrs::IconExclamation; break;
        case MB_ICONASTERISK: rv |= DialogAttrs::IconAsterix; break;
        }

        return rv;
    }

    bool BasicDialog::Create(HINSTANCE hResInst, HWND hWndParent, LPARAM dwInitParam)
    {
        ATLASSUME(this->m_hWnd == NULL);
        BOOL rv = this->m_thunk.Init(nullptr, nullptr);
        if (!rv) {
            SetLastError(ERROR_OUTOFMEMORY);
            return false;
        }
        _AtlWinModule.AddCreateWndData(&this->m_thunk.cd, (void*)this);
#ifdef _DEBUG
        m_bModal = false;
#endif
        HWND hWnd = CreateDialogParamW(
            (hResInst ? hResInst : _AtlBaseModule.GetResourceInstance()),
            MAKEINTRESOURCE(IDD),
            hWndParent,
            BasicDialog::StartDialogProc,
            dwInitParam
        );
        ATLASSUME(this->m_hWnd == hWnd);
        return true;
    }

    bool BasicDialog::Create(Rect const& rc, HWND parent, HINSTANCE hResInst, LPARAM param)
    {
        bool ok = true;
        if (nullptr == m_hWnd) {
            ok = Create(hResInst, parent, param);
        }
        if (ok) {
            CRect rc2 = ToCRect(rc);
            MoveWindow(rc2);
            ShowWindow(SW_SHOW);
        }
        return ok;
    }

    bool BasicDialog::ShowModal(HINSTANCE hResInst, HWND hWndParent /*= ::GetActiveWindow()*/, LPARAM dwInitParam /*= 0*/)
    {
        ATLASSUME(this->m_hWnd == nullptr);
        BOOL rv = this->m_thunk.Init(nullptr, nullptr);
        if (!rv) {
            SetLastError(ERROR_OUTOFMEMORY);
            return false;
        }
        _AtlWinModule.AddCreateWndData(&this->m_thunk.cd, (void*)this);
#ifdef _DEBUG
        m_bModal = true;
#endif
        INT_PTR irv = DialogBoxParamW(
            (hResInst ? hResInst : _AtlBaseModule.GetResourceInstance()),
            MAKEINTRESOURCE(IDD),
            hWndParent,
            BasicDialog::StartDialogProc,
            dwInitParam
        );
        return irv > 0;
    }

    bool BasicDialog::ShowModal(HWND hWndParent, LPARAM dwInitParam)
    {
        return ShowModal(nullptr, hWndParent, dwInitParam);
    }

    DialogResult BasicDialog::Result() const
    {
        return m_result;
    }

    void BasicDialog::OnDestroy()
    {
        SetMsgHandled(false);
    }

    void BasicDialog::OnKeyDown(UINT key, UINT rep, UINT flags)
    {
        UNREFERENCED_ARG(rep);
        UNREFERENCED_ARG(flags);
        if (VK_ESCAPE == key) {
            DestroyWindow();
        }
    }

    void BasicDialog::OnButtonClick(int id, UINT code)
    {
        if ((id >= IdOk) && (id <=IdHelp)) {
            m_result.Code = DialogResult::Ok + (id - IdOk);
            EndDialog(id);
        }
    }

    void BasicDialog::OnCommand(UINT code, int id, HWND control)
    {
        UNREFERENCED_ARG(control);
        if (IDCANCEL == id) {
            m_result.Code = DialogResult::Cancel;
            EndDialog(id);
        }
        else if ((BN_CLICKED == code) || (BN_PUSHED == code)) {
            OnButtonClick(id, code);
        }
        else {
            SetMsgHandled(false);
        }
    }

    void BasicDialog::CreateButtons(CF::RectzAllocator<LONG>& btnAlloc) const
    {
        if (0 == (m_attrs & DialogAttrs::HasButtons)) {
            return ;
        }
        // Get count of the buttons...
        unsigned bit = 1, count = 0;
        for (UINT i=IdFirst; i<IdLast; i++) {
            if (0 != (m_attrs & bit)) {
                ++count;
            }
            bit <<= 1;
        }

        DRect rc = LRect2DRect(btnAlloc.Next(0, 0));
        RectzAllocator<double> buttonRectAlloc(rc);
        double buttonWidth = (buttonRectAlloc.Width() + (2*count)) / (count ? count : 1);

        // ##TODO: localization
        static PCTSTR buttonText[] =
        {
            _T("Ok"),
            _T("Yes"),
            _T("No"),
            _T("Cancel"),
            _T("Abort"),
            _T("Retry"),
            _T("Ignore"),
            _T("Close"),
            _T("Help"),
            _T("Continue")
        };

        // Create buttons...
        bit = 1;
        int j = 0;
        for (UINT i=IdFirst; i<IdLast; i++)
        {
            if (0 != (m_attrs & bit))
            {
                CButton button;
                CRect rcBtn = ToCRect(buttonRectAlloc.Next(buttonWidth));

                if (button.Create(m_hWnd, rcBtn, buttonText[i - IdFirst], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_NOTIFY, 0, i))
                {
                    button.SetFont(m_font);
                    button.Detach();
                    ++j;
                }
            }

            bit <<= 1;
        }
    }

    HICON BasicDialog::LoadCustomIcon()
    {
        return nullptr;
    }

    CRect BasicDialog::SetupIcon(CF::RectzAllocator<LONG>& rcAlloc)
    {
        if (!(m_attrs & DialogAttrs::HasIcons)) {
            return {};
        }
        PCTSTR  iconId = nullptr;
        HICON tempIcon = nullptr;
        switch (m_attrs & DialogAttrs::HasIcons) {
        case        DialogAttrs::IconStop: iconId = IDI_HAND; break;
        case    DialogAttrs::IconQuestion: iconId = IDI_QUESTION; break;
        case DialogAttrs::IconExclamation: iconId = IDI_EXCLAMATION; break;
        case     DialogAttrs::IconAsterix: iconId = IDI_ASTERISK; break;
        default:
            tempIcon = LoadCustomIcon();
            break;
        }
        if (!tempIcon && iconId) {
            tempIcon = ::LoadIcon(nullptr, iconId);
        }
        if (!tempIcon) {
            return {};
        }
        m_icon.Attach(tempIcon);
        CSize sz = CF::GetIconSize(m_icon);
        return ToCRect<LONG>(rcAlloc.Next(sz.cx + 8, sz.cy));
    }

    BOOL BasicDialog::OnInitDialog(HWND focusedWindow, LPARAM param)
    {
        UNREFERENCED_ARG(focusedWindow);
        UNREFERENCED_ARG(param);
        SetWindowTextW(m_title.c_str());

        CRect rc;
        GetClientRect(rc);
        rc.DeflateRect(10, 10);

        CF::RectzAllocator<LONG> rcAlloc(FromCRect<LONG>(rc));
        CRect rcIcon = SetupIcon(rcAlloc);

        if ((m_attrs & DialogAttrs::HasIcons) && m_icon.m_hIcon) {
            WTL::CStatic icon;
            icon.Create(m_hWnd, rcIcon, nullptr,
                WS_CHILD | WS_VISIBLE |
                SS_ICON | SS_CENTERIMAGE | 
                SS_CENTER | SS_REALSIZEIMAGE,
                WS_EX_TRANSPARENT
            );
            icon.SetIcon(m_icon);
            icon.Detach();
        }

        CRect rcBody, rcA;
        {
            CClientDC dc(*this);

            // FIXME: Add extra space after text
            WString temp = m_text;
            temp += L"\r\n\r\n";

            HFONT lf = dc.SelectFont(m_textFont);
            ::DrawTextW(dc, temp.c_str(), (int)temp.length(), rcBody, DT_CALCRECT);

            dc.SelectFont(m_font);
            ::DrawTextW(dc, L"A", 1, rcA, DT_CALCRECT);

            dc.SelectFont(lf);
        }

        int buttonsCy = rcA.Height() + 16;

        CRect rcText = ToCRect(rcAlloc.Next(rc.Width(), rc.Height() - buttonsCy));
        int bw = rcBody.Width(), tw = rcText.Width();
        int bh = rcBody.Height(), th = rcText.Height();

        CRect rcWin;
        GetWindowRect(rcWin);

        if (bw > tw)
            rcWin.right += (bw - tw);

        if (bh > th)
            rcWin.bottom += (bh - th);

        MoveWindow(rcWin, FALSE);

        GetClientRect(rc);
        rc.DeflateRect(8, 10);

        rcAlloc.Reset(FromCRect<LONG>(rc));
        rcAlloc.Next(rcIcon.Width(), 0, 4);
        rcText = ToCRect(rcAlloc.Next(rc.Width(), rc.Height() - buttonsCy));

        {
            WTL::CStatic text;
            text.Create(m_hWnd, rcText, m_text.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT);
            text.SetFont(m_textFont, FALSE);
            text.Detach();
        }

        CF::RectzAllocator<LONG> btnAlloc(rcAlloc.Next(rc.Width()));
        CreateButtons(btnAlloc);

        CenterWindow(GetParent());
        SetMsgHandled(FALSE);
        return TRUE;
    }

    int BasicDialog::OnCreate(LPCREATESTRUCT cs)
    {
        UNREFERENCED_ARG(cs);
        OnInitDialog(nullptr, 0);
        return 0;
    }
}
