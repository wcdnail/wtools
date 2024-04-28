#include "stdafx.h"
#include "status.bar.ex.h"
#include "string.utils.format.h"

namespace Ui
{
    namespace
    {
        enum Hardcoded
        {
            InitialCy = 16,
            SwapCurrentAndNext = 1000,
            WaitNext,
            SwapCurrentAndNextTimeout = 2,
            WaitNextTimeout = 500,
        };
    }

    StatusBar::StatusBar()
        : Super()
        , Current()
        , Next()
        , NextY(0)
        , IsNextAnimation(false)
        , Deque()
        , Font(::CreateFont(-(InitialCy - 4), 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Tahoma")))
        , MyBackColor(0x202020)
        , MyTextColor(0xdfdfdf)
    {}

    StatusBar::It::~It()
    {
        if (NULL != Icon.m_hIcon)
        {
            ::DestroyIcon(Icon);
            Icon.m_hIcon = NULL;
        }
    }

    StatusBar::It::It(std::wstring const& text /* = L"" */, HICON icon /* = NULL */, int progress /* = -1 */)
        : Text(text)
        , Icon(icon)
        , Progress(progress)
    {}

    StatusBar::It::It(It const& rhs)
        : Text(rhs.Text)
        , Icon(rhs.Icon)
        , Progress(rhs.Progress)
    {
        const_cast<It*>(&rhs)->Icon.m_hIcon = NULL;
    }

    StatusBar::It& StatusBar::It::operator = (It const& rhs)
    {
        It temp(rhs);
        temp.Swap(*this);
        return *this;
    }

    void StatusBar::It::Swap(It& rhs)
    {
        Text.swap(rhs.Text);
        std::swap(Icon, rhs.Icon);
        std::swap(Progress, rhs.Progress);
    }

    StatusBar::~StatusBar()
    {}

    bool StatusBar::Create(HWND parent, UINT id)
    {
        CRect rc;
        ::GetClientRect(parent, rc);

        rc.top = rc.bottom - (InitialCy + 2);
        Super::Create(parent, rc, NULL, WS_CHILD | WS_VISIBLE, 0, id);

        return NULL != m_hWnd;
    }

    int StatusBar::OnCreate(LPCREATESTRUCT cs)
    {
        return 0;
    }

    void StatusBar::OnDestroy()
    {
        KillTimer(WaitNext);
        KillTimer(SwapCurrentAndNext);
        SetMsgHandled(FALSE);
    }

    void StatusBar::Reset()
    {
        KillTimer(WaitNext);
        KillTimer(SwapCurrentAndNext);

        Current = Next = It();
        NextY = 0;
        IsNextAnimation = false;

        std::lock_guard lk(DequeMx);
        Deque.clear();
    }

    void StatusBar::Add(std::wstring const& text, HICON icon /*= NULL*/, int progress /*= -1*/)
    {
        std::lock_guard lk(DequeMx);
        Deque.push_back(It(text, icon, progress));

        if (!IsNextAnimation)
            StartAnimation();
    }

    void StatusBar::AddFormatV(HICON icon, int progress, PCWSTR text, va_list ap)
    {
        Add((PCWSTR)Str::Elipsis<wchar_t>::FormatV(text, ap), icon, progress);
    }

    void StatusBar::AddFormatV(HICON icon, int progress, PCSTR text, va_list ap)
    {
        Add(CA2W(Str::Elipsis<char>::FormatV(text, ap), CP_ACP).m_psz, icon, progress);
    }

    void StatusBar::StartAnimation()
    {
        Next = Deque.front();
        Deque.pop_front();

        NextY = InitialCy;
        IsNextAnimation = true;
        SetTimer(SwapCurrentAndNext, SwapCurrentAndNextTimeout);
    }

    void StatusBar::StopAnimation()
    {
        Current = Next;
        NextY = 0;
        IsNextAnimation = false;
        KillTimer(SwapCurrentAndNext);

        bool needWaitNext = false;
        {
            std::lock_guard lk(DequeMx);
            needWaitNext = !Deque.empty();
        }

        if (needWaitNext)
        {
            KillTimer(WaitNext);
            SetTimer(WaitNext, WaitNextTimeout);
        }
    }

    void StatusBar::OnTimer(UINT_PTR id)
    {
        if (SwapCurrentAndNext == id)
        {
            --NextY;
            if (NextY <= 0)
                StopAnimation();

            ::InvalidateRect(m_hWnd, NULL, FALSE);
        }

        if (WaitNext == id)
        {
            KillTimer(id);
            std::lock_guard lk(DequeMx);
            StartAnimation();
        }
    }

    void StatusBar::Draw(It const& item, CMemoryDC& dc, CRect const& rcClient, COLORREF bkColor)
    {
        dc.FillSolidRect(rcClient, bkColor);
        item.Draw(dc, rcClient);
    }

    void StatusBar::OnPaint(CDCHandle) const
    {
        CRect rc;
        GetClientRect(rc);

        CPaintDC paint(*this);
        CMemoryDC dc(paint, rc);

        dc.SetBkMode(TRANSPARENT);
        HFONT lobj = dc.SelectFont(Font);

        dc.SetTextColor(MyTextColor);

        if (IsNextAnimation)
        {
            int h = rc.Height();

            CRect rc2 = rc;
            rc2.top = NextY;
            rc2.bottom = rc2.top + h;

            CRect rc1 = rc;
            rc1.top = rc2.top - h;
            rc1.bottom = rc1.top + h;

            Draw(Current, dc, rc1, MyBackColor);
            Draw(Next, dc, rc2, MyBackColor);
        }
        else
            Draw(Current, dc, rc, MyBackColor);

        dc.SelectFont(lobj);
    }

    void StatusBar::It::Draw(CMemoryDC& dc, CRect const& rcClient) const
    {
        CRect rc = rcClient;
        rc.DeflateRect(1, 1);

        if (NULL != Icon)
        {
            dc.DrawIconEx(rc.left + 1, rc.top + (rc.Height() - InitialCy) / 2
                , Icon, InitialCy, InitialCy);
        }

        rc.left += InitialCy + 4;

        {
            CRect rcText = rc;
            if (-1 != Progress)
                rcText.right = rcText.left + (int)(rc.Width() * .8);

            //dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
            dc.DrawText(Text.c_str(), (int)Text.length(), rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

            rc.left = rcText.right + 2;
        }

// ##TODO: Handle progress..."))
#if 0
        if (-1 != Progress)
        {
            CRect rcProgress = rc;
            dc.Rectangle(rcProgress);
        }
#endif
    }

    void StatusBar::Blink()
    {
// ##TODO: Implementation"))
    }
}
