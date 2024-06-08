#include "stdafx.h"
#include "tab.bar.item.h"
#include <rect.putinto.h>

namespace Twins
{
    TabBarItem::~TabBarItem() = default;

    TabBarItem::TabBarItem()
        :     id{-1}
        ,   icon{nullptr}
        ,   name{}
        ,    pos{}
        , dflags{DT_LEFT | DT_VCENTER | DT_SINGLELINE}
    {
    }

    void TabBarItem::Draw(WTL::CDCHandle dc, CRect const& rc, CSize const& iconSize) const
    {
        CSize textBounds;
        dc.GetTextExtent(name, name.GetLength(), &textBounds);

        CRect rcSmall = rc;
        rcSmall.DeflateRect(2, 0);

        CRect rcItem{0, 0, min(textBounds.cx + iconSize.cx + 2, rcSmall.Width()), min(max(textBounds.cy, iconSize.cy), rcSmall.Height())};
        unsigned const align = (0 != (dflags & DT_VCENTER) ? Rc::YCenter : 0)
                             | (0 != (dflags & DT_CENTER)  ? Rc::XCenter : 0)
                             | (0 != (dflags & DT_RIGHT)   ? Rc::Right   : 0);
        Rc::PutInto(rcSmall, rcItem, align);

        if (nullptr != icon) {
            CRect rcIcon(0, 0, iconSize.cx, iconSize.cy);
            Rc::PutInto(rcItem, rcIcon, Rc::Left | Rc::YCenter);

            dc.DrawIconEx(rcIcon.left, rcIcon.top, icon, rcIcon.Width(), rcIcon.Height());
            rcItem.left = rcIcon.right + 2;
        }

        dc.DrawText(name, name.GetLength(), rcItem, dflags);
    }

    int TabBarItem::Left() const
    {
        return pos.x;
    }

    int TabBarItem::Width() const
    {
        return pos.y;
    }

    int TabBarItem::Right() const
    {
        return Left() + Width();
    }

    bool TabBarItem::IsIn(CPoint const& point) const
    {
        return (point.x >= Left()) && (point.x <= Right());
    }

    bool TabBarItem::IsInSeparator(CPoint const& point) const
    {
        return (point.x >= (Right()-1)) && (point.x <= (Right()+1));
    }

    bool TabBarItem::IsIdEq(int n) const
    {
        return id == n;
    }
}
