#include "stdafx.h"
#include "windows.popup.hint.h"
#include "todo.fixme.etc.h"

namespace CF
{
    enum 
    {
        MinimalWidth = 150,
        MinimalHeight = 22,
        AppearTimerId = 101,
        WaitTimerId = 102,
        DisappearTimerId = 103,
        AppearTimerQuant = 1,
        WaitTimerQuant = 5000,
        HintXMargin = 4, 
        HintYMargin = 4,
        HintXSpacing = 2,
        HintYSpacing = 2,
    };

    static const double HintAppearTime = 0.5;

    class PopupHint::Hint: public ATL::CWindowImpl<PopupHint::Hint>
    {
    public:
        typedef ATL::CWindowImpl<Hint> Super;

        Hint(PopupHint& owner, PCTSTR text, unsigned flags, HICON icon);
        ~Hint();

        bool Show();

        BEGIN_MSG_MAP_EX(Hint)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_TIMER(OnTimer)
            MSG_WM_LBUTTONUP(OnAnyButtonUp)
            MSG_WM_RBUTTONUP(OnAnyButtonUp)
            MSG_WM_PAINT(OnPaint)
        END_MSG_MAP()

    private:
        PopupHint& owner_; 
        unsigned flags_;
        ATL::CString text_;
        CIcon icon_;
        long myHintHeight_;
        CFont font_;
        double startTime_;
        double appearTime_;
        UINT_PTR currendTimerId_;

        static long nextHintY_;

        CRect GetIconRect() const;
        CRect GetHintWindowRect() const;
        void StartTimer(UINT_PTR id, UINT quant);
        void AppearMove(double time);
        void AppearAlpha(double time);
        void DestroyHint();
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        void OnTimer(UINT_PTR);
        void OnAnyButtonUp(UINT, POINT);
        void OnPaint(HDC) const;
        virtual void OnFinalMessage(HWND);
    };

    long PopupHint::Hint::nextHintY_ = 0;

    PopupHint::Hint::Hint(PopupHint& owner, PCTSTR text, unsigned flags, HICON icon)
        : owner_(owner)
        , text_(text)
        , flags_(flags)
        , icon_(icon)
        , myHintHeight_(0)
        , font_((HFONT)GetStockObject(DEFAULT_GUI_FONT))
        , currendTimerId_(0)
    {}

    PopupHint::Hint::~Hint()
    {
        m_hWnd = NULL;
    }

    bool PopupHint::Hint::Show()
    {
        if (NULL == m_hWnd)
        {
            Super::Create(NULL, rcDefault, NULL
                , WS_POPUP
                , WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | 0x02000000L /* WS_EX_COMPOSITED */
                );

            return NULL != m_hWnd;
        }

        return false;
    }

    CRect PopupHint::Hint::GetIconRect() const
    {
        CRect rc(0, 0, 0, 0);

        if (icon_)
        {
// ##TODO: Get icon size."))
            rc.right = 48;
            rc.bottom = 48;
        }

        return rc;
    }

    CRect PopupHint::Hint::GetHintWindowRect() const
    {
        CRect rc(0, 0, MinimalWidth, MinimalHeight);

        CClientDC dc(m_hWnd);
        HGDIOBJ lobj = dc.SelectFont(font_);

        long textHeight = 0;
        int n = 0;
        while (-1 != n)
        {
            ATL::CString line = text_.Tokenize(_T("\r\n"), n);

            if (!line.IsEmpty())
            {
                SIZE sz = {0};
                dc.GetTextExtent(line, line.GetLength(), &sz);

// ##TODO: Horizontal text rendering"))
                rc.right = max(rc.right, sz.cx);
                textHeight += sz.cy;
            }
        }
        rc.bottom = max(textHeight, rc.bottom);
        ::SelectObject(dc, lobj);

        CRect rcIcon = GetIconRect();
        rc.right += rcIcon.Width();
        rc.bottom = max(rcIcon.Height(), rc.bottom);

        return rc;
    }

    static inline long GetScreenWidth() { return GetSystemMetrics(SM_CXMAXIMIZED) - (GetSystemMetrics(SM_CXSIZEFRAME) * 2); }
    static inline long GetScreenHeight() { return GetSystemMetrics(SM_CYMAXIMIZED) - (GetSystemMetrics(SM_CYSIZEFRAME) * 2); }

    static inline double GetSeconds() 
    {
        LARGE_INTEGER curTime = {0}, freq = {0};
        ::QueryPerformanceCounter(&curTime);
        ::QueryPerformanceFrequency(&freq);
        return (double)curTime.QuadPart / (double)(0 != freq.QuadPart ? freq.QuadPart : 1);
    }

    void PopupHint::Hint::StartTimer(UINT_PTR id, UINT quant)
    {
        startTime_ = GetSeconds();
        appearTime_ = HintAppearTime;
        SetTimer(id, quant);
    }

    int PopupHint::Hint::OnCreate(LPCREATESTRUCT)
    {
        ::SetLayeredWindowAttributes(m_hWnd, 0, 0, LWA_ALPHA);

        CRect rc = GetHintWindowRect();
        rc.InflateRect(HintXMargin, HintYMargin);

        long right = GetScreenWidth();
        long bottom = GetScreenHeight();

        bottom -= nextHintY_;
        myHintHeight_ = (rc.Height() + HintYSpacing);
        nextHintY_ += myHintHeight_;

// ##TODO: Popup from right bottom corner..."))
        rc.MoveToX(right - (rc.Width() + HintXSpacing));
        rc.bottom = rc.top = bottom - HintYSpacing;

        MoveWindow(rc, TRUE);
        ShowWindow(SW_SHOW);

        StartTimer(AppearTimerId, AppearTimerQuant);
        return 0;
    }

    void PopupHint::Hint::OnDestroy()
    {
        KillTimer(AppearTimerId);
        KillTimer(DisappearTimerId);
        KillTimer(WaitTimerId);
    }

    void PopupHint::Hint::AppearMove(double time)
    {
        CRect rc;
        GetWindowRect(rc);

        time = (time >= 1.0 ? 1.0 : time);
        double top = rc.bottom + HintYSpacing - ((double)myHintHeight_ * time);
        rc.top = (long)top;

        SetWindowPos(HWND_TOPMOST, rc, SWP_NOACTIVATE);
    }

    void PopupHint::Hint::AppearAlpha(double time)
    {
        double alpha = 255. * time;
        alpha = (alpha > 255. ? 255. : alpha);
        ::SetLayeredWindowAttributes(m_hWnd, 0, (BYTE)alpha, LWA_ALPHA);
    }

    static double GetTime(UINT_PTR id, double start, double time)
    {
        double cur = GetSeconds();
        double elapsed = cur - start;
        double percent = elapsed / (0.0 == time ? time : 1.0);
        return (DisappearTimerId == id ? 1.0 - percent : percent);
    }

    static bool TimeDone(double time)
    {
        return (time <= 0.0) || (time >= 1.0);
    }

    void PopupHint::Hint::OnAnyButtonUp(UINT, POINT)
    {
        if (DisappearTimerId != currendTimerId_)
        {
            KillTimer(AppearTimerId);
            KillTimer(WaitTimerId);

            StartTimer(DisappearTimerId, AppearTimerQuant);
        }
    }

    void PopupHint::Hint::OnTimer(UINT_PTR id)
    {
        currendTimerId_ = id;

        switch (id)
        {
        case DisappearTimerId:
        case AppearTimerId:
        {
            double time = GetTime(id, startTime_, appearTime_);
            appearTime_ -= 0.03;

            AppearMove(time);
            AppearAlpha(time);

            if (TimeDone(time))
            {
                KillTimer(id);
                if (DisappearTimerId == id)
                    DestroyHint();
                else
                    SetTimer(WaitTimerId, WaitTimerQuant);
            }
            break;
        }

        case WaitTimerId:
            KillTimer(id);
            StartTimer(DisappearTimerId, AppearTimerQuant);
            break;
        }
    }

    void PopupHint::Hint::OnFinalMessage(HWND)
    {
        nextHintY_ -= myHintHeight_;
        if (nextHintY_ < 0)
        {
            nextHintY_ = 0;
        }

        delete this;
    }

    void PopupHint::Hint::DestroyHint()
    {
        DestroyWindow();
        owner_.OnHintDestroy(this);
    }

    PopupHint::PopupHint()
    {
    }

    PopupHint::~PopupHint()
    {
        struct destroy_hint
        {
            void operator() (HintPtr& hint)
            {
                hint->DestroyWindow();
            }
        };

        std::for_each(hints_.begin(), hints_.end(), destroy_hint());
    }

    void PopupHint::AddHint(PCTSTR text, unsigned flags /* = 0 */, HICON icon /* = NULL */)
    {
        hints_.push_back(new Hint(*this, text, flags, icon));
    }

    void PopupHint::PopLast()
    {
        if (!hints_.empty())
            hints_[hints_.size()-1]->Show();
    }

    void PopupHint::OnHintDestroy(Hint* hintRawPtr)
    {
        Hints::iterator it = std::find(hints_.begin(), hints_.end(), hintRawPtr);
        if (it != hints_.end())
        {
            hints_.erase(it);
        }
    }

    void PopupHint::Hint::OnPaint(HDC) const
    {
        CPaintDC dc(m_hWnd);
        HGDIOBJ lobj = dc.SelectFont(font_);

        CRect rc;
        GetClientRect(rc);

        GRADIENT_RECT vrect[1] = { { 0, 1 } };
        TRIVERTEX vert[2] = 
        {
          { rc.left, rc.top, 0, 0, 0, 0 }
        , { rc.right, rc.bottom, 0, 0, 0x50ff, 0 }
        };

        dc.GradientFill(vert, _countof(vert), vrect, _countof(vrect), GRADIENT_FILL_RECT_V);

        rc.DeflateRect(HintXMargin, HintYMargin);

        if (icon_)
        {
            CRect rcIcon = GetIconRect();
            dc.DrawIcon(HintXMargin, HintYMargin, icon_);
            rc.left = rcIcon.right;
        }

        dc.SetTextColor(0xffffff);
        dc.SetBkMode(TRANSPARENT);
        dc.DrawText(text_, text_.GetLength(), rc, DT_LEFT);

        ::SelectObject(dc, lobj);
    }
}
