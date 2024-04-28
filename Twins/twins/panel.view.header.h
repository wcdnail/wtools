#pragma once

#include "tab.bar.h"

namespace Twins
{
    struct TabBarItem;

    class PanelHeader: TabBar
    {
    private:
        typedef TabBar Super;

    public:
        typedef std::function<void(int, int)> ColumnResizeDoneCallback;

        enum
        {
            StateUnknown = 0,
            StateAscending = 1, 
            StateDescending = 2, 
        };

        PanelHeader();
        ~PanelHeader();

        using Super::Create;
        using Super::Clear;
        using Super::Add;
        using Super::SetHot;
        using Super::SetSelectedById;
        using Super::ClearSelected;
        using Super::SetIconById;
        using Super::OnClick;
        using Super::operator HWND;

        int ColumnCount() const;
        void SetColumnWidth(int n, int nw, bool invalidateParent);
        void SetColumnWidthById(int id, int nw, bool invalidateParent);
        int GetColumnWidth(int index) const;
        bool IsValidColumnId(int n) const;
        bool PtInRect(CPoint const& pn) const;
        ColumnResizeDoneCallback& OnColumnResizeDone();

    private:
        struct BySeparator;

        CCursor Cursor;
        CCursor ResizeCursor;
        TabBarItem* ResizedItem;
        bool IsDraging;
        int DragLeft;
        int DragItemWidth;
        int DragColumn;
        ColumnResizeDoneCallback ColumnResizeDoneFunctor;
        
        int OnCreate(LPCREATESTRUCT);
        void OnMouseMove(UINT flags, CPoint point);
        void OnLButtonDown(UINT flags, CPoint point);
        void OnLButtonUp(UINT flags, CPoint point);

        BEGIN_MSG_MAP_EX(PanelHeader)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            CHAIN_MSG_MAP(Super)
        END_MSG_MAP()
    };

    inline PanelHeader::ColumnResizeDoneCallback& PanelHeader::OnColumnResizeDone() { return ColumnResizeDoneFunctor; }
    inline bool PanelHeader::IsValidColumnId(int n) const { return (n >= 0) && (n <= (ColumnCount()-1)); }
}
