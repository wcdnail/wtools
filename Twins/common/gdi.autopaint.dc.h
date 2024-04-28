#pragma once

#include <boost/noncopyable.hpp>
#include <atlgdi.h>

namespace Gdi
{
    class AutoPaintDc: boost::noncopyable
    {
    public:
        AutoPaintDc(HWND window);
        ~AutoPaintDc();

        CDC& GetValidDc();
        CDC& GetMemoryDc();
        CRect const& GetRect();

        static CRect ClientRect(HWND window);

    private:
        CRect rc_;
        CPaintDC paintDc_;
        CDC memDc_;
        CBitmap memBm_;
        HBITMAP lastMemDcBm_;
    };

    inline CRect AutoPaintDc::ClientRect(HWND window)
    {
        CRect rc;
        ::GetClientRect(window, rc);
        return rc;
    }

    inline AutoPaintDc::AutoPaintDc(HWND window) 
        : rc_(ClientRect(window))
        , paintDc_(window)
        , memDc_(::CreateCompatibleDC(paintDc_))
        , memBm_(::CreateCompatibleBitmap(paintDc_, rc_.Width(), rc_.Height()))
        , lastMemDcBm_(memDc_.SelectBitmap(memBm_))
    {}

    CDC& AutoPaintDc::GetValidDc()
    {
        return NULL == memDc_.m_hDC ? paintDc_ : memDc_;
    }

    inline AutoPaintDc::~AutoPaintDc()
    {
        paintDc_.BitBlt(0, 0, rc_.Width(), rc_.Height(), memDc_, 0, 0, SRCCOPY);
        memDc_.SelectBitmap(lastMemDcBm_);
    }

    inline CDC& AutoPaintDc::GetMemoryDc() { return memDc_; }
    inline CRect const& AutoPaintDc::GetRect() { return rc_; }
}
