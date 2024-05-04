#pragma once

#include "wcdafx.api.h"
#include <atlwin.h>
#include <atlgdi.h>
#include <atlcrack.h>

namespace CF  // Common framework // Да что ты говоришь? )) (Этот (второй) коментарий я оставил тут десяток лет супстя...)
{
    class BufferedPaint
    {
    public:
        WCDAFX_API BufferedPaint(WTL::CDC& destDc, WTL::CDC& secondDc, bool doubleBuffering, HWND ownerWindow);
        WCDAFX_API ~BufferedPaint();

        WTL::CDCHandle GetCurrentDc() const;
        CRect const& GetRect() const;

    private:
        WTL::CDC&        destDc_;
        WTL::CDC&      secondDc_;
        bool    doubleBuffering_;
        CRect                rc_;

        static void CreateIfNull(WTL::CDC& destDc, WTL::CDC& secondDc, CRect const& rc);

    private:
        BufferedPaint(BufferedPaint const&);
        BufferedPaint& operator = (BufferedPaint const&);
    };

    class DoubleBuffered: public ATL::CMessageMap
    {
    protected:
        WCDAFX_API DoubleBuffered(bool useSecondBuffer);
        WCDAFX_API virtual ~DoubleBuffered();

        WTL::CDC& GetSecondDc() const;
        bool IsDoubleBuffered() const;

        BEGIN_MSG_MAP_EX(DoubleBuffered)
            MSG_WM_SIZE(OnSize)
        END_MSG_MAP()

    private:
        bool       doubleBuffered_;
        mutable WTL::CDC secondDc_;

        WCDAFX_API void OnSize(UINT type, CSize size);

    private:
        DoubleBuffered(DoubleBuffered const&);
        DoubleBuffered& operator = (DoubleBuffered const&);
    };

    inline WTL::CDCHandle BufferedPaint::GetCurrentDc() const
    {
        return (doubleBuffering_ ? secondDc_.m_hDC : destDc_.m_hDC);
    }

    inline CRect const& BufferedPaint::GetRect() const
    {
        return rc_;
    }

    inline WTL::CDC& DoubleBuffered::GetSecondDc() const
    {
        return secondDc_;
    }

    inline bool DoubleBuffered::IsDoubleBuffered() const
    {
        return doubleBuffered_;
    }
}
