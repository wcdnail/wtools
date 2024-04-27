#include "stdafx.h"
#include "command.line.h"
#include <climits>

namespace Ui
{
    CommandLine::CommandLine(EnterCallback const& onEnter)
        : Super()
        , OnCancel()
        , History()
        , Prompt()
        , PromptMx()
        , Combo()
        , OnEnter(onEnter)
        , MyFont(::CreateFont(-11, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, PROOF_QUALITY, 0, _T("Courier New")))
        , MyTextColor(0xdfdfdf)
        , MyBackColor(0x202020)
        , MyBackBrush(::CreateSolidBrush(MyBackColor))
    {}

    CommandLine::~CommandLine()
    {}

    bool CommandLine::Create(HWND parent, CRect const& rc, UINT id)
    {
        CRect rcLine = rc;
        Super::Create(parent, rcLine, NULL, WS_CHILD | WS_VISIBLE, 0, id);
        return NULL != this->m_hWnd;
    }

    int CommandLine::OnCreate(LPCREATESTRUCT cs)
    {
        Prompt.Create(this->m_hWnd, rcDefault, _T("")
            , WS_CHILD | WS_VISIBLE 
            | SS_CENTERIMAGE 
            | SS_RIGHT | SS_PATHELLIPSIS
            , 0, IdPrompt);

        CRect rcCmb(0, 0, 0, 600);
        Combo.Create(this->m_hWnd, rcCmb, NULL
            , WS_CHILD | WS_VISIBLE | WS_TABSTOP
            | CBS_DROPDOWN 
            , CBES_EX_NOSIZELIMIT
            , IdCombo);

        Prompt.SetFont(MyFont);
        Combo.SetFont(MyFont);

        for (HistoryStore::const_iterator it = History.cbegin(); it != History.cend(); ++it) {
            Combo.InsertItem(CBEIF_TEXT, 0, (LPCTSTR)it->c_str(), 0, 0, 0, 0, NULL);
        }

        Combo.UpdateWindow();
        Combo.Invalidate();

        return 0;
    }

    void CommandLine::OnDestroy()
    {
        LoadHistoryFromCombo();
        SetMsgHandled(FALSE);
    }

    void CommandLine::LoadHistoryFromCombo()
    {
        RefHistoryStore tempHistory;

        int count = Combo.GetCount();
        for (int i=0; i<count; i++)
        {
            CString text;
            if (Combo.GetLBText(i, text) > 0) {
                std::wstring strText(text.GetString(), text.GetLength());
                tempHistory.emplace_back(std::move(strText));
            }
        }

        History.swap(tempHistory);
    }

    void CommandLine::OnSize(UINT type, CSize sz)
    {
        if (Prompt.m_hWnd)
        {
            CRect rc(0, 0, sz.cx, sz.cy);
            rc.right = (int)(sz.cx * .2);
            Prompt.MoveWindow(rc, FALSE);

            if (Combo.m_hWnd)
            {
                rc.left = rc.right + 2;
                rc.right = sz.cx;
                
                Combo.MoveWindow(rc, FALSE);
            }
        }
    }

    void CommandLine::OnSetFocus(CWindow prev)
    {
        if (Combo.m_hWnd)
            Combo.SetFocus();
    }

    void CommandLine::SetPath(std::wstring const& text)
    {
        std::lock_guard lk(PromptMx);
        Prompt.SetWindowText((text + L" #>").c_str());
    }

    void CommandLine::ShowDropdown()
    {
        if (Combo.m_hWnd)
        {
            Combo.SetFocus();
            Combo.ShowDropDown(TRUE);
        }
    }

    LRESULT CommandLine::OnEndEditNotify(LPNMHDR nmh)
    {
        PNMCBEENDEDIT h = (PNMCBEENDEDIT)nmh;
        CString text;
        BOOL rv = TRUE;

        bool store = false;
        if (CBENF_RETURN == h->iWhy)
        {
            text = h->szText;
            store = (OnEnter ? OnEnter(text) : false);
            rv = FALSE;
        }

        if (h->fChanged && store)
            ToHistory(text);
        
        if (CBENF_ESCAPE == h->iWhy)
        {
            if (OnCancel)
                OnCancel(Combo);
        }

        Combo.SetWindowText(_T(""));
        return rv;
    }

    LRESULT CommandLine::OnSelEndCancel(LPNMHDR nmh)
    {
        if (CBN_SELENDCANCEL == nmh->code)
        {
            int i = 0;
        }

        return 0;
    }

    void CommandLine::ToHistory(CString const& text)
    {
        int n = Combo.FindStringExact(0, text);
        if (CB_ERR == n)
            Combo.InsertItem(CBEIF_TEXT, 0, text, 0, 0, 0, 0, NULL);
    }

    void CommandLine::AddText(std::wstring const& text, bool addDirSeparator)
    {
        CStringW temp;
        Combo.GetWindowText(temp);

        temp += text.c_str();
        if (addDirSeparator && (text.length() > 1) && (text[text.length()-1] != L'\\'))
            temp += L'\\';

        Combo.SetWindowText(temp);
    }

    BOOL CommandLine::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, MyBackColor);
        return 1;
    }

    HBRUSH CommandLine::OnCtlColorStatic(CDCHandle dc, HWND)
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);

        return MyBackBrush;
    }

    HBRUSH CommandLine::OnCtlColorEdit(CDCHandle dc, HWND hwnd)
    {
        return OnCtlColorStatic(dc, hwnd);
    }
}
