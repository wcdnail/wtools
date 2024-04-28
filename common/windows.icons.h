#pragma once

#include <windef.h>
#include <winuser.h>
#include <wingdi.h>
#include <atltypes.h>

namespace CF
{
    inline CSize GetIconSize(HICON icon)
    {
        CSize rv;

        ICONINFO info = {0};
        if (::GetIconInfo(icon, &info))
        {
            BITMAP bm = {0};
            int cy = 0;

            if (info.hbmColor)
            {
                ::GetObject(info.hbmColor, sizeof(bm), &bm);
                cy = bm.bmHeight;
            }
            else if (info.hbmMask)
            {
                ::GetObject(info.hbmMask, sizeof(bm), &bm);
                cy = bm.bmHeight / 2;
            }

            rv.cx = bm.bmWidth;
            rv.cy = cy;

            if (info.hbmColor)
                ::DeleteObject(info.hbmColor);

            if (info.hbmMask)
                ::DeleteObject(info.hbmMask);
        }

        return rv;
    }
}
