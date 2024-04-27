#pragma once

#include "panel.view.edit.h"
#include <atlwin.h>
#include <atlctrls.h>

namespace Twins
{
    class Panel;

    typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0> PathBarTraits;

    class PathBar: ATL::CWindowImpl<PathBar, ATL::CWindow, PathBarTraits>
                 , CDialogResize<PathBar>
    {
    private:
        typedef ATL::CWindowImpl<PathBar, ATL::CWindow, PathBarTraits> Super;
        typedef CDialogResize<PathBar> SuperResizer;

    public:
        enum { WmHeightChanged = WM_USER + 0x7000 };

        PathBar(Panel& owner);
        ~PathBar();

        using Super::Create;
        using Super::GetWindowRect;
        using Super::operator HWND;
        using Super::EnableWindow;

        void Invalidate() const;
        void SetPathHint(PCTSTR displayPath);
        void TrackHistoryPopup(bool byKey = false);
        void EditPath();

    private:
        friend Super;
        friend SuperResizer;

        Panel& Owner;
        CTabCtrl BookmarkBar;
        CStatic Path;
        CButton Favorite;
        CButton History;
        LabelEdit PathEdit;

        // Appearance
        CFont MyFont;
        COLORREF MyTextColor;
        COLORREF MyBackColor;
        COLORREF MyActiveTextColor;
        COLORREF MyActiveBackColor;
        CBrush MyActiveBackBrush;
        CBrush MyInactiveBackBrush;
        CBrush MyBackBrush;
        CPen MyPen;
        COLORREF MyButtonBackColor[2];

        BOOL OnEraseBkgnd(CDCHandle dc);
        int OnCreate(LPCREATESTRUCT);
        void AdjustPositions();
        HBRUSH OnCtlColorStatic(CDCHandle dc, HWND);
        void ShowHistoryPopup(UINT = 0, int = 0, HWND = 0);
        void ShowFavoritesPopup(UINT = 0, int = 0, HWND = 0);
        void OnPathEditDone(int index, CString const& newname);
        void OnSetFocus(CWindow);
        HBRUSH OnCtlColor(CDCHandle dc, HWND);
        void OnDrawItem(int id, PDRAWITEMSTRUCT di);

    private:
        PathBar(PathBar const&);
        PathBar& operator = (PathBar const&);

    private:
        DECLARE_WND_CLASS(_T("[Twins]PathBar"))

        BEGIN_MSG_MAP_EX(PathBar)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
            MSG_WM_CTLCOLORBTN(OnCtlColor)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_DRAWITEM(OnDrawItem)
            COMMAND_HANDLER_EX(ID_HISTORY, BN_CLICKED, ShowHistoryPopup)
            COMMAND_HANDLER_EX(ID_FAVORITES, BN_CLICKED, ShowFavoritesPopup)
            CHAIN_MSG_MAP(SuperResizer)
        END_MSG_MAP()

        enum
        {
            ID_PAGER = 101,
            ID_PATH,
            ID_FAVORITES,
            ID_HISTORY,
            ID_PATHEDIT,
        };

        BEGIN_DLGRESIZE_MAP(PathBar)
            DLGRESIZE_CONTROL(ID_PAGER, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_PATH, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_PATHEDIT, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_FAVORITES, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(ID_HISTORY, DLSZ_MOVE_X)
        END_DLGRESIZE_MAP()
    };
}
