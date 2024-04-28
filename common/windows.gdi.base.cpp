#include "stdafx.h"
#include "windows.gdi.base.h"

namespace Cf  // Common framework
{
    BitmapDC::BitmapDC()
        : dc_()
        , rcSource_()
    {
    }

    BitmapDC::~BitmapDC()
    {
    }

    bool BitmapDC::LoadFrom(WTL::CBitmap& tempBitmap)
    {
        BITMAP bm = {0};
        if (tempBitmap.GetBitmap(bm))
        {
            WTL::CClientDC desktopDc(::GetDesktopWindow());
            if (desktopDc.m_hDC)
            {
                WTL::CDC tempDc;
                if (tempDc.CreateCompatibleDC(desktopDc))
                {
                    rcSource_.reset(0, 0, bm.bmWidth, bm.bmHeight);
                    tempDc.SelectBitmap(tempBitmap);
                    dc_.Attach(tempDc.Detach());
                    return true;
                }
            }
        }

        return false;
    }

    bool BitmapDC::Load(UINT resourceId)
    {
        WTL::CBitmap tempBitmap;
        if (tempBitmap.LoadBitmap(resourceId))
            return LoadFrom(tempBitmap);

        return false;
    }

    void BitmapDC::Put(WTL::CDCHandle dc, DWORD bltFlags /*= SRCCOPY*/) const
    {
        dc.BitBlt(0, 0, rcSource_.width(), rcSource_.height(), dc_, rcSource_.left(), rcSource_.top(), bltFlags);
    }

    void BitmapDC::Put(WTL::CDCHandle dc, RECT const& rc, unsigned align, DWORD bltFlags /*= SRCCOPY*/) const
    {
        Put(dc, CommonRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top), align, bltFlags);
    }

    void BitmapDC::Put(WTL::CDCHandle dc, CommonRect const& rc, DWORD bltFlags /*= SRCCOPY*/) const
    {
        dc.BitBlt(rc.left(), rc.top(), rc.width(), rc.height(), dc_, rcSource_.left(), rcSource_.top(), bltFlags);
    }

    void BitmapDC::Put(WTL::CDCHandle dc, CommonRect const& rc, unsigned align, DWORD bltFlags /*= SRCCOPY*/) const
    {
        CommonRect rcOut(rcSource_);
        rcOut.put_into(rc, align);
        Put(dc, rcOut, bltFlags);
    }
}
