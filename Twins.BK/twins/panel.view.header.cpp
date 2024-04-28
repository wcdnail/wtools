#include "stdafx.h"
#include "panel.view.header.h"
#include "tab.bar.item.h"

namespace Twins
{
    PanelHeader::PanelHeader()
        : Super()
        , Cursor(::LoadCursor(NULL, IDC_ARROW))
        , ResizeCursor(::LoadCursor(NULL, IDC_SIZEWE))
        , ResizedItem(NULL)
        , IsDraging(false)
        , DragLeft(0)
        , DragItemWidth(0)
        , DragColumn(-1)
        , ColumnResizeDoneFunctor()
    {}

    PanelHeader::~PanelHeader()
    {}

    int PanelHeader::OnCreate(LPCREATESTRUCT)
    {
        return 0;
    }

    void PanelHeader::OnMouseMove(UINT flags, CPoint point)
    {
        if (IsDraging)
        {
            SetCursor(ResizeCursor);
            int amount = point.x - DragLeft;
            if (NULL != ResizedItem)
            {
                int nw = DragItemWidth + amount;
                nw = (nw < 4 ? 4 : nw);

                if (ResizedItem->pos.y != nw)
                {
                    ResizedItem->pos.y = nw;
                    ::InvalidateRect(m_hWnd, NULL, FALSE);
                }
            }
        }
        else
        {
            ItemVec::iterator it = std::find_if(Items.begin(), Items.end(), std::bind(&TabBarItem::IsInSeparator, std::placeholders::_1, point));
            ResizedItem = it != Items.end() ? &(*it) : NULL;
            if (NULL != ResizedItem)
            {
                DragColumn = ResizedItem->id;
                SetCursor(ResizeCursor);
            }

            SetMsgHandled(FALSE);
        }
    }

    void PanelHeader::OnLButtonDown(UINT flags, CPoint point)
    {
        IsDraging = NULL != ResizedItem;

        if (IsDraging)
        {
            ::SetCapture(m_hWnd);
            DragLeft = point.x;
            DragItemWidth = ResizedItem->pos.y;
            SetCursor(ResizeCursor);
        }
        else
            SetMsgHandled(FALSE);
    }

    void PanelHeader::OnLButtonUp(UINT flags, CPoint point)
    {
        if (NULL != ResizedItem)
        {
            int column = DragColumn;
            int nw = ResizedItem->pos.y;
            IsDraging = false;
            ResizedItem = NULL;
            DragColumn = -1;
            ::ReleaseCapture();

            if (ColumnResizeDoneFunctor)
                ColumnResizeDoneFunctor(column, nw);
        }
        else
            SetMsgHandled(FALSE);
    }

    int PanelHeader::ColumnCount() const 
    {
        return Count(); 
    }

    void PanelHeader::SetColumnWidth(int n, int nw, bool invalidateParent)
    {
        if (SetWidth(n, nw) && invalidateParent)
            ::InvalidateRect(GetParent(), NULL, FALSE);
    }

    void PanelHeader::SetColumnWidthById(int id, int nw, bool invalidateParent)
    {
        SetColumnWidth(GetIndexById(id), nw, invalidateParent);
    }

    int PanelHeader::GetColumnWidth(int index) const 
    {
        return IsValidColumnId(index) ? GetItem(index).Width() : 0; 
    }

    bool PanelHeader::PtInRect(CPoint const& pn) const
    {
        CRect rc;
        GetClientRect(rc);
        return TRUE == rc.PtInRect(pn);
    }
}
