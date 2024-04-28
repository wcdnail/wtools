#include "stdafx.h"
#include "dialogz.basic.h"
#include "windows.icons.h"

namespace Cf
{
    BasicDialog::BasicDialog(unsigned flags) 
        : Rv()
        , Flags(flags)
        , Title()
        , TextBody()
        , Icon()
        , UiFont((HFONT)::GetStockObject(DEFAULT_GUI_FONT))
        , MsgFont(::CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Consolas")))
    {}

    BasicDialog::BasicDialog(WidecharString const& message, WidecharString const& title, unsigned flags) 
        : Rv()
        , Flags(flags)
        , Title(title)
        , TextBody(message)
        , Icon()
        , UiFont((HFONT)::GetStockObject(DEFAULT_GUI_FONT))
        , MsgFont(::CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Consolas")))
    {}

    BasicDialog::~BasicDialog()
    {}

    unsigned BasicDialog::GetCompatFlags(unsigned flags)
    {
        unsigned rv = 0;

        switch (flags & 0xf)
        {
        case MB_OK: rv |= DialogFlags::Ok; break;
        case MB_OKCANCEL: rv |= DialogFlags::OkCancel; break;
        case MB_ABORTRETRYIGNORE: rv |= DialogFlags::AbortRetryIgnore; break;
        case MB_YESNOCANCEL: rv |= DialogFlags::YesNoCancel; break;
        case MB_YESNO: rv |= DialogFlags::YesNo; break;
        case MB_RETRYCANCEL: rv |= DialogFlags::RetryCancel; break;
#if (WINVER >= 0x0500)
        case MB_CANCELTRYCONTINUE: rv |= DialogFlags::CancelRetryContinue; break;
#endif
        }

        switch (flags & 0xf0)
        {
        case MB_ICONHAND: rv |= DialogFlags::IconStop; break;
        case MB_ICONQUESTION: rv |= DialogFlags::IconQuestion; break;
        case MB_ICONEXCLAMATION: rv |= DialogFlags::IconExclamation; break;
        case MB_ICONASTERISK: rv |= DialogFlags::IconAsterix; break;
        }

        return rv;
    }

    bool BasicDialog::Show(HWND parent, Rect const& rc)
    {
        bool ok = true;
        if (nullptr == m_hWnd) {
            CRect rc2 = ToRect(rc);
            ok = (nullptr != Super::Create(parent, rc2));
        }
        if (ok) {
            ShowWindow(SW_SHOW);
        }
        return ok;
    }

    void BasicDialog::ShowModal(HWND parent)
    {
        int tr = (int)Super::DoModal(parent);
        Rv.Set(tr);
    }

    void BasicDialog::OnDestroy()
    {
        SetMsgHandled(FALSE);
    }

    void BasicDialog::OnKeyDown(UINT key, UINT rep, UINT flags)
    {
        if (VK_ESCAPE == key)
            DestroyWindow();
    }

    void BasicDialog::OnCommand(UINT code, int id, HWND control)
    {
        if (IDCANCEL == id)
        {
            Rv.Set(DialogResult::Cancel);
            EndDialog(id);
        }
        else if ((id >= IdOk) && (id <=IdHelp))
        {
            if ((BN_CLICKED == code) || (BN_PUSHED == code))
            {
                Rv.Set(DialogResult::Ok + (id - IdOk));
                EndDialog(id);
            }
        }
        else
            SetMsgHandled(FALSE);
    }

    void BasicDialog::CreateButtons(Cf::RectzAllocator<LONG>& btnAlloc)
    {
        if (0 == (Flags & DialogFlags::HasButtons)) {
            return ;
        }
        // Get count of the buttons...
        unsigned bit = 1, count = 0;
        for (UINT i=IdFirst; i<IdLast; i++) {
            if (0 != (Flags & bit)) {
                ++count;
            }
            bit <<= 1;
        }

        RectzAllocator<double> buttonRectAlloc(btnAlloc.Next(0, 0));
        double buttonWidth = (buttonRectAlloc.Width() + (2*count)) / (count ? count : 1);

        // ##TODO: Localization required!
        static PCTSTR buttonText[] = 
        { 
          _T("Ok")
        , _T("Yes")
        , _T("No")
        , _T("Cancel")
        , _T("Abort")
        , _T("Retry")
        , _T("Ignore")
        , _T("Close")
        , _T("Help")
        , _T("Continue")
        };

        // Create buttons...
        bit = 1;
        int j = 0;
        for (UINT i=IdFirst; i<IdLast; i++)
        {
            if (0 != (Flags & bit))
            {
                CButton button;
                CRect rcBtn = ToRect(buttonRectAlloc.Next(buttonWidth));

                if (button.Create(m_hWnd, rcBtn, buttonText[i - IdFirst], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_NOTIFY, 0, i))
                {
                    button.SetFont(UiFont);
                    button.Detach();
                    ++j;
                }
            }

            bit <<= 1;
        }
    }

    CRect BasicDialog::SetupIcon(Cf::RectzAllocator<LONG>& rcAlloc)
    {
        if (Flags & DialogFlags::HasIcons)
        {
            PCTSTR iconId = nullptr;

            switch (Flags & 0x0f000000)
            {
            case DialogFlags::IconStop: iconId = IDI_HAND; break;
            case DialogFlags::IconQuestion: iconId = IDI_QUESTION; break;
            case DialogFlags::IconExclamation: iconId = IDI_EXCLAMATION; break;
            case DialogFlags::IconAsterix: iconId = IDI_ASTERISK; break;
            }

            if (iconId)
            {
                HICON tempIcon = ::LoadIcon(nullptr, iconId);
                if (tempIcon)
                {
                    Icon.Attach(tempIcon);

                    CSize sz = Cf::GetIconSize(Icon);
                    return ToRect<LONG>(rcAlloc.Next(sz.cx + 8, sz.cy));
                }
            }
        }

        return CRect();
    }

    BOOL BasicDialog::OnInitDialog(HWND focusedWindow, LPARAM param)
    {
        ::SetWindowTextW(m_hWnd, Title.c_str());

        CRect rc;
        GetClientRect(rc);
        rc.DeflateRect(8, 10);

        Cf::RectzAllocator<LONG> rcAlloc(FromRect<LONG>(rc));
        CRect rcIcon = SetupIcon(rcAlloc);

        if ((Flags & DialogFlags::HasIcons) && Icon.m_hIcon)
        {
            WTL::CStatic icon;
            icon.Create(m_hWnd, rcIcon, nullptr
                , WS_CHILD | WS_VISIBLE 
                | SS_ICON | SS_CENTERIMAGE | SS_CENTER | SS_REALSIZEIMAGE
                , WS_EX_TRANSPARENT);

            icon.SetIcon(Icon);
            icon.Detach();
        }

        CRect rcBody, rcA;
        {
            CClientDC dc(*this);

            // FIXME: Add extra space after text
            WidecharString temp = TextBody;
            temp += L"\r\n\r\n";

            HFONT lf = dc.SelectFont(MsgFont);
            ::DrawTextW(dc, temp.c_str(), (int)temp.length(), rcBody, DT_CALCRECT);

            dc.SelectFont(UiFont);
            ::DrawTextW(dc, L"A", 1, rcA, DT_CALCRECT);

            dc.SelectFont(lf);
        }

        int buttonsCy = rcA.Height() + 16;

        CRect rcText = ToRect(rcAlloc.Next(rc.Width(), rc.Height() - buttonsCy));
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

        rcAlloc.Reset(FromRect<LONG>(rc));
        rcAlloc.Next(rcIcon.Width(), 0, 4);
        rcText = ToRect(rcAlloc.Next(rc.Width(), rc.Height() - buttonsCy));

        {
            WTL::CStatic text;
            text.Create(m_hWnd, rcText, TextBody.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT);
            text.SetFont(MsgFont, FALSE);
            text.Detach();
        }

        Cf::RectzAllocator<LONG> btnAlloc(rcAlloc.Next(rc.Width()));
        CreateButtons(btnAlloc);

        CenterWindow(GetParent());
        SetMsgHandled(FALSE);
        return TRUE;
    }

    int BasicDialog::OnCreate(LPCREATESTRUCT cs)
    {
        OnInitDialog(nullptr, 0);
        return 0;
    }
}
