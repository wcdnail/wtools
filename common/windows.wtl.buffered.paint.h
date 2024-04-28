#ifndef _CF_windows_wtl_buffered_paint_h__
#define _CF_windows_wtl_buffered_paint_h__

#include "wcdafx.api.h"
#include <wtypes.h>
#include <atlwin.h>
#include <atlgdi.h>
#include <atlcrack.h>

namespace CF  // Common framework
{
    class BufferedPaint
    {
    public:
        WCDAFX_API BufferedPaint(WTL::CDC& destDc, CDC& secondDc, bool doubleBuffering, HWND ownerWindow);
        WCDAFX_API ~BufferedPaint();

        WCDAFX_API WTL::CDC& GetCurrentDc();
        WCDAFX_API CRect const& GetRect() const;

    private:
        WTL::CDC& destDc_;
        WTL::CDC& secondDc_;
        bool doubleBuffering_;
        CRect rc_;

        static void CreateIfNull(WTL::CDC& destDc, WTL::CDC& secondDc, CRect const& rc);

    private:
        BufferedPaint(BufferedPaint const&);
        BufferedPaint& operator = (BufferedPaint const&);
    };

    class DoubleBuffered: public ATL::CMessageMap
    {
    public:
        WCDAFX_API void SetBackgroundColor(COLORREF color);
        
    protected:
        WCDAFX_API DoubleBuffered(COLORREF backColor, bool useSecondBuffer);
        WCDAFX_API virtual ~DoubleBuffered();

        WCDAFX_API CDC& GetSecondDc() const;
        WCDAFX_API bool IsDoubleBuffered() const;
        WCDAFX_API COLORREF GetBackgroundColor() const;

        BEGIN_MSG_MAP_EX(DoubleBuffered)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_SIZE(OnSize)
        END_MSG_MAP()

    private:
        COLORREF backColor_;
        bool doubleBuffered_;
        mutable WTL::CDC secondDc_;

        WCDAFX_API BOOL OnEraseBkgnd(CDCHandle dc) const;
        WCDAFX_API void OnSize(UINT type, CSize size);

    private:
        DoubleBuffered(DoubleBuffered const&);
        DoubleBuffered& operator = (DoubleBuffered const&);
    };
}

#endif // _CF_windows_wtl_buffered_paint_h__
