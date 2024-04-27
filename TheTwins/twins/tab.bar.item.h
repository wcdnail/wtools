#pragma once

#include <atlstr.h>
#include <atltypes.h>
#include <atlgdi.h>

namespace Twins
{
    struct TabBarItem
    {
        int id;
        HICON icon;
        CString name;
        mutable CPoint pos;
        UINT dflags;

        TabBarItem();
        ~TabBarItem();

        void Draw(CDC& dc, CRect const& rc, CSize const& iconSize) const;

        int Left() const;
        int Right() const;
        int Width() const;

        bool IsIn(CPoint const& point) const;
        bool IsInSeparator(CPoint const& point) const;
        bool IsIdEq(int n) const;
    };
}
