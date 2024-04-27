#pragma once

#include "file.list.h"
#include <boost/noncopyable.hpp>
#include <atlcrack.h>
#include <atlcomcli.h>
#include <objidl.h>

namespace Dnd { class DropTarget; }

namespace Twins
{
    class PanelView;

    class DragnDropHelper: boost::noncopyable
    {
    public:
        DragnDropHelper(PanelView& owner);
        ~DragnDropHelper();

        HWND GetHwnd() const;
        void OnStop(CPoint const& pt, STGMEDIUM& medium, FORMATETC* format, DWORD* effect);
        bool IsMyPoint(POINTL const& pt);

    private:
        friend class PanelView;

        static const unsigned InvalidTick = (unsigned)-2;

        struct CursorSet
        {
            CCursor DropMultiple;
            CCursor DropSingle;
            CCursor DropNo;
            HCURSOR Current;
            CursorSet();
        };

        PanelView& Owner;
        unsigned ClickTick;
        bool DragDetected;
        CursorSet Cursor;
        Fl::List Selection;
        CComPtr<Dnd::DropTarget> DTarget;
        //static UINT WM_DI_GETDRAGIMAGE;

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        void OnLButtonDown(UINT flags, CPoint point);
        void OnLButtonUp(UINT flags, CPoint point);
        void OnMouseMove(UINT flags, CPoint point);
        void OnStart(CPoint const& pt);
        void OnDrag(CPoint const& pt);
        bool IsMultipleSelection() const;
        static HGLOBAL PrepareHDROP(Fl::List const& files, CPoint const& pt);
        HRESULT BeginDrag(Fl::List const& files, CPoint const& pt) const;
        LRESULT OnGetDragImage(UINT, WPARAM wParam, LPARAM lParam);

        BEGIN_MSG_MAP_EX(DragnDropHelper)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            //MESSAGE_HANDLER_EX(WM_DI_GETDRAGIMAGE, OnGetDragImage);
        END_MSG_MAP()
    };

    inline bool DragnDropHelper::IsMultipleSelection() const { return Selection.Count() > 1; }
}
