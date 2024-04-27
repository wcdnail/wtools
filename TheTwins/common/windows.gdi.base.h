#ifndef _CF_windows_gdi_base_h__
#define _CF_windows_gdi_base_h__

#include "trect.h"
#include <atlgdi.h>

namespace Cf  // Common framework
{
    class BitmapDC
    {
    public:
        typedef cf::rect<LONG> CommonRect;

        BitmapDC();
        ~BitmapDC();

        bool Load(UINT resourceId);

        void Put(WTL::CDCHandle dc, DWORD bltFlags = SRCCOPY) const;
        void Put(WTL::CDCHandle dc, CommonRect const& rc, DWORD bltFlags = SRCCOPY) const;
        void Put(WTL::CDCHandle dc, CommonRect const& rc, unsigned align, DWORD bltFlags = SRCCOPY) const;
        void Put(WTL::CDCHandle dc, RECT const& rc, unsigned align, DWORD bltFlags = SRCCOPY) const;

    private:
        WTL::CDC dc_;
        CommonRect rcSource_;

        bool LoadFrom(WTL::CBitmap& tempBitmap);

    private:
        BitmapDC(BitmapDC const&);
        BitmapDC& operator = (BitmapDC const&);
    };
}

#endif // _CF_windows_gdi_base_h__
