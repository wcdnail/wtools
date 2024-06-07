#pragma once

#include <wcdafx.api.h>
#include <deque>
#include <string>
#include <atlwin.h>
#include <atlcrack.h>
#include <atlgdi.h>
#include <mutex>

namespace Ui
{
    class StatusBar: ATL::CWindowImpl<StatusBar, CWindow>
    {
    private:
        DECLARE_WND_CLASS_EX(_T("[WCD]StatusBar"), CS_VREDRAW | CS_HREDRAW, COLOR_WINDOW-1)

    public:
        DELETE_COPY_MOVE_OF(StatusBar);

        typedef ATL::CWindowImpl<StatusBar, CWindow> Super;

        ~StatusBar() override;
        StatusBar();

        using Super::ShowWindow;
        using Super::SubclassWindow;
        using Super::GetWindowRect;
        using Super::operator HWND;

        bool Create(HWND parent, UINT id);
        void Reset();
        void Clear();
        void Blink();
        void AddFormatV(HICON icon, int progress, PCWSTR text, va_list ap);
        void AddFormatV(HICON icon, int progress, PCSTR text, va_list ap);
        void AddFormat(HICON icon, int progress, PCWSTR text, ...);
        void AddFormat(HICON icon, int progress, PCSTR text, ...);
        void AddFormat(HICON icon, PCWSTR text, ...);
        void AddFormat(HICON icon, PCSTR text, ...);
        void AddFormat(int progress, PCWSTR text, ...);
        void AddFormat(int progress, PCSTR text, ...);
        void AddFormat(PCWSTR text, ...);
        void AddFormat(PCSTR text, ...);
        void Add(std::wstring const& text, HICON icon = NULL, int progress = -1);

    private:
        friend Super;

        struct It
        {
            std::wstring Text;
            CIconHandle Icon;
            int Progress;

            It(std::wstring const& text = L"", HICON icon = NULL, int progress = -1);
            ~It();

            It(It const& rhs);
            It& operator = (It const& rhs);
            void Swap(It& rhs);

            void Draw(CMemoryDC& dc, CRect const& rcClient) const;
        };

        typedef std::deque<It> ItDeque;

        It Current;
        It Next;
        int NextY;
        bool IsNextAnimation;
        ItDeque Deque;
        std::mutex DequeMx;

        // Appearance
        CFont Font;
        COLORREF MyBackColor;
        COLORREF MyTextColor;
        
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        void OnPaint(CDCHandle) const;
        void OnTimer(UINT_PTR id);
        void StartAnimation();
        void StopAnimation();

        static void Draw(It const& item, CMemoryDC& dc, CRect const& rcClient, COLORREF bkColor);

        BEGIN_MSG_MAP_EX(StatusBar)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_TIMER(OnTimer)
        END_MSG_MAP()
    };

    inline void StatusBar::AddFormat(HICON icon, int progress, PCWSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(icon, progress, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(HICON icon, int progress, PCSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(icon, progress, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(HICON icon, PCWSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(icon, -1, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(HICON icon, PCSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(icon, -1, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(int progress, PCWSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(NULL, progress, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(int progress, PCSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(NULL, progress, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(PCWSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(NULL, -1, text, ap);
        va_end(ap);
    }

    inline void StatusBar::AddFormat(PCSTR text, ...)
    {
        va_list ap;
        va_start(ap, text);
        AddFormatV(NULL, -1, text, ap);
        va_end(ap);
    }

    inline void StatusBar::Clear()
    {
        Add(L"", NULL, -1);
    }
}
