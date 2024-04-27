#pragma once

#include "file.list.h"
#include "drive.bar.h"
#include "path.bar.h"
#include "panel.view.h"
#include <settings.h>
#include <dir.manager.h>
#include <atlwin.h>
#include <atlcrack.h>
#include <string>
#include <boost/noncopyable.hpp>

namespace Twins
{
    typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_ACCEPTFILES> Panel0Traits;

    class Panel: boost::noncopyable,
                 ATL::CWindowImpl<Panel, ATL::CWindow, Panel0Traits>,
                 CDialogResize<Panel>
    {
        typedef ATL::CWindowImpl<Panel, ATL::CWindow, Panel0Traits> Super;
        typedef CDialogResize<Panel> SuperResizer;

    public:
        const int    Id;
        PanelView  View;
        DriveBar Drives;
        PathBar   Paths;

        Panel();
        ~Panel();

        using Super::Create;
        using Super::SetFocus;
        using Super::operator HWND;

        void Invalidate() const;
        Cf::DirManager const& GetDirManager() const;
        Cf::DirManager& GetDirManager();
        PCWSTR GetCurrentPath() const;
        void Reload();
        void ChangeDir(FItem const* it);
        void Go2RootDir();
        void Go2UpDir();
        void CreateDir();
        void OnThemeChanged();
        void UpdateDriveInfoIfSame(Panel& other);
        void AddCurrentPathToBookmarks();
        void StartPathEdit();
        std::wstring CreateEntry();
        void RunViewer();

    private:
        friend Super;
        friend SuperResizer;

        BOOL OnEraseBkgnd(CDCHandle dc);
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        void OnSetFocus(HWND);
        void OnKeyDown(UINT code, UINT rep, UINT flags);
        void OnDriveSelect(int drive, int prev);
        void FetchContent(PCWSTR path);
        void LoadContentCancel();
        void FetchContentDone(double elapsed);
        void StoreSelection(std::wstring const& selected, bool isDir);
        bool OnRename(FItem const* it, wchar_t const* newname);
        void EnableControls(BOOL enable);
        void OnContextMenu(HWND sender, CPoint point);
        void ShowShellContextMenu(std::wstring const& pathname, CPoint point);
        void OnDropFiles(HDROP dropInfo);
        void StoreSelected(Fl::Entry const& info);
        LRESULT OnPathBarHeightChanged(UINT, WPARAM wParam, LPARAM);
        void SetSelectionAfter(std::wstring const& name);
        void OnEnter(FItem const* it);

        enum
        {
            ID_DRIVE_SELECTOR = 1000,
            ID_PATH_BAR,
            ID_PANEL_VIEW,
        };

        DECLARE_WND_CLASS(_T("[Twins]Panel"))

        BEGIN_MSG_MAP_EX(Panel)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_CONTEXTMENU(OnContextMenu)
            MSG_WM_DROPFILES(OnDropFiles)
            CHAIN_MSG_MAP(SuperResizer)
        END_MSG_MAP()

        BEGIN_DLGRESIZE_MAP(Panel)
            DLGRESIZE_CONTROL(ID_DRIVE_SELECTOR, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_PATH_BAR, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_PANEL_VIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()
    };

    inline PCWSTR Panel::GetCurrentPath() const { return GetDirManager().FullPath().wstring().c_str(); }
    inline Cf::DirManager const& Panel::GetDirManager() const { return View.Scanner.Path; }
    inline Cf::DirManager& Panel::GetDirManager() { return View.Scanner.Path; }
    inline void Panel::Reload() { FetchContent(nullptr); }
}
