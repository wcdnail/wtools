#pragma once

#include "tab.bar.h"
#include "drive.enum.h"
#include <wtl.combobox.h>
#include <atlwin.h>
#include <atlstr.h>

namespace Twins
{
    class Panel;

    typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0> DriveBarTraits;

    class DriveBar: ATL::CWindowImpl<DriveBar, ATL::CWindow, DriveBarTraits>
                  , CDialogResize<DriveBar>
    {
    private:
        typedef ATL::CWindowImpl<DriveBar, ATL::CWindow, DriveBarTraits> Super;
        typedef CDialogResize<DriveBar> SuperResizer;
        typedef std::function<void(int, int)> ChangeDriveCallback;

    public:
        DriveBar(Panel& owner);
        ~DriveBar();

        using Super::Create;
        using Super::operator HWND;
        using Super::EnableWindow;

        void CancelDropdownCombo();
        void DropdownCombo();
        void UpdateInfo(Drive::Enumerator::Item const& item);
        void ActualizeDriveIndex(std::wstring const& dstr);
        void UpdateDriveInfo();
        ChangeDriveCallback& OnChangeDrive();
        int GetDrive() const;
        CString GetDriveName(CPoint const& point) const;
        void Invalidate() const;

        static void InitializeDrives(DriveBar& left, DriveBar& right, Drive::Enumerator::ItemVector const& item);
        
    private:
        friend Super;
        friend SuperResizer;

        Panel& Owner;
        TabBar Disks;
        CAdvComboBox Combo;
        CStatic Hint;
        CButton Go2RootBtn;
        CButton Go2UpBtn;
        CRect ComboRect;
        int ActiveDrive;
        int PrevDrive;
        ChangeDriveCallback ChangeDrive;

        // Appearance
        CFont MyFont;
        CFont MyComboFont;
        COLORREF MyTextColor;
        COLORREF MyBackColor;
        CBrush MyBackBrush;
        CPen MyPen;
        COLORREF MyButtonBackColor[2];

        enum
        {
            ID_TABULATOR = 1,
            ID_COMBO,
            ID_HINT,
            ID_GO2ROOT,
            ID_GO2UP,
        };

        DECLARE_WND_CLASS(_T("[Twins]DriveBar"))

        BEGIN_MSG_MAP_EX(DriveBar)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_CTLCOLORSTATIC(OnCtlColor)
            MSG_WM_CTLCOLORBTN(OnCtlColor)
            MSG_WM_DRAWITEM(OnDrawItem)
            COMMAND_ID_HANDLER_EX(ID_COMBO, OnComboCommand)
            NOTIFY_ID_HANDLER_EX(ID_COMBO, OnComboNotify)
            COMMAND_HANDLER_EX(ID_GO2ROOT, BN_CLICKED, OnGo2Root)
            COMMAND_HANDLER_EX(ID_GO2UP, BN_CLICKED, OnGo2Up)
            CHAIN_MSG_MAP(SuperResizer)
        END_MSG_MAP()

        BEGIN_DLGRESIZE_MAP(DriveBar)
            DLGRESIZE_CONTROL(ID_TABULATOR, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_HINT, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_GO2ROOT, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(ID_GO2UP, DLSZ_MOVE_X)
        END_DLGRESIZE_MAP()

        BOOL OnEraseBkgnd(CDCHandle dc);
        int OnCreate(LPCREATESTRUCT);
        void SetCurrentDrive(int drive, bool fromTab);
        int OnSelectTab(int drive, PCTSTR);
        void OnComboCommand(UINT code, int, HWND);
        LRESULT OnComboNotify(LPNMHDR pnmh);
        void Focus2View();
        void OnUpdateDrivesList();
        void Clear();
        void OnLoadContentBegin();
        void OnLoadContentDone();
        void Reset(Drive::Enumerator::ItemVector const& item);
        void Add(Drive::Enumerator::Item const& item);
        void OnGo2Root(UINT, int, HWND);
        void OnGo2Up(UINT, int, HWND);
        HBRUSH OnCtlColor(CDCHandle dc, HWND);
        void OnDrawItem(int id, PDRAWITEMSTRUCT di);

    private:
        DriveBar(DriveBar const&);
        DriveBar& operator = (DriveBar const&);
    };

    inline DriveBar::ChangeDriveCallback& DriveBar::OnChangeDrive() { return ChangeDrive; }
}
