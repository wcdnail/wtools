#include "stdafx.h"
#include "drive.bar.h"
#include "tab.bar.item.h"
#include "drive.info.h"
#include "panel.h"
#include <twins.langs/twins.lang.strings.h>
#include <windows.gdi.rects.h>
#include <dh.tracing.h>
#include <atlstr.h>

namespace Twins
{
    enum
    {
        InitialComboWidth = 54,
        MaximalComboHeight = 4096,
        InitialButtonWidth = 18,
        DefaultTabButtonWidth = 38,
    };

    DriveBar::DriveBar(Panel& owner)
        : Owner(owner)
        , Disks()
        , Combo()
        , Hint()
        , Go2RootBtn()
        , Go2UpBtn()
        , ComboRect()
        , ActiveDrive(-1)
        , PrevDrive(-1)
        , ChangeDrive()
        , MyFont(CreateFont(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Tahoma")))
        , MyComboFont(CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Consolas")))
        , MyTextColor(0xdfdfdf)
        , MyBackColor(0x202020)
        , MyBackBrush(::CreateSolidBrush(MyBackColor))
        , MyPen(::CreatePen(PS_SOLID, 1, 0x7f7f7f))
    {
        MyButtonBackColor[0] = 0x7f7f7f;
        MyButtonBackColor[1] = 0x000000;
    }

    DriveBar::~DriveBar()
    {}

    BOOL DriveBar::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, MyBackColor);
        return 1;
    }

    int DriveBar::OnCreate(LPCREATESTRUCT)
    {
        CRect rc;
        ::GetClientRect(m_hWnd, rc);

        CRect rcTab = rc;
        rcTab.top += 1;
        rcTab.bottom = rc.top + rc.Height() / 2 - 1;

        CRect rcHint = rc;
        rcHint.top = rcTab.bottom + 1;
        rcHint.bottom -= 1;

        ComboRect = rcHint;
        ComboRect.right = ComboRect.left + InitialComboWidth;
        rcHint.left = ComboRect.right + 1;

        CRect rc2Up = rcHint;
        rc2Up.left = rcHint.right - InitialButtonWidth;
        rc2Up.bottom -= 3;

        CRect rc2Root = rc2Up;
        rc2Root.right = rc2Up.left - 1;
        rc2Root.left = rc2Root.right - InitialButtonWidth; 

        rcHint.right = rc2Root.left - 1;
        rcHint.left += 3;

        ComboRect.bottom += MaximalComboHeight;
        
        Disks.Create(m_hWnd, rcTab, NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN, 0, ID_TABULATOR);
        Combo.Create(m_hWnd, ComboRect, NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL, 0, ID_COMBO);
        Hint.Create(m_hWnd, rcHint, NULL, WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, 0, ID_HINT);
        Go2RootBtn.Create(m_hWnd, rc2Root, _T("/"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, ID_GO2ROOT);
        Go2UpBtn.Create(m_hWnd, rc2Up, _T(".."), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, ID_GO2UP);

        Combo.SetFont(MyComboFont);
        Hint.SetFont(MyFont);
        Go2RootBtn.SetFont(MyFont);
        Go2UpBtn.SetFont(MyFont);

        Combo.Invalidate(TRUE);

        Disks.OnClick() = std::bind(&DriveBar::OnSelectTab, this, std::placeholders::_1, std::placeholders::_2);

        DlgResize_Init(false, false);
        return 0;
    }

    void DriveBar::Focus2View()
    {
        GetParent().SetFocus();
    }

    void DriveBar::Add(Drive::Enumerator::Item const& item)
    {
        static const CString spacer(_T(" "));

        CString driveLeter;
        driveLeter.Format(_T("%c"), item.path[0]);
        
        CString dispName;
        dispName.Format(_T("%s [%s] %s"),
            (DRIVE_NO_ROOT_DIR == item.type ? spacer : driveLeter), 
            Drive::Type::ToString(item.type),
            item.fs.c_str());

        Disks.Add(item.num, driveLeter, DefaultTabButtonWidth, item.icon);

        COMBOBOXEXITEM cbi;
        cbi.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        cbi.iItem          = item.num;
        cbi.pszText        = dispName.GetBuffer();
        cbi.cchTextMax     = dispName.GetLength();
        cbi.iImage         = item.iconIndex;
        cbi.iSelectedImage = cbi.iImage;
        cbi.iOverlay       = 0;
        cbi.iIndent        = 0;
        cbi.lParam         = (LPARAM)&item;

        int ni = Combo.InsertItem(&cbi);
        if (LB_ERR != ni)
        {
            CSize sz(0, 0);
            CClientDC dc(Combo);
            dc.GetTextExtent(dispName, dispName.GetLength(), &sz);

            if (ComboRect.Width() < sz.cx)
                ComboRect.right = ComboRect.left + sz.cx;

            ComboRect.bottom += sz.cy;
            Combo.SetItemHeight(ni, sz.cy);
        }
    }

    void DriveBar::OnUpdateDrivesList()
    {
        ::InvalidateRect(m_hWnd, NULL, FALSE);
        Owner.Reload();
    }

    void DriveBar::SetCurrentDrive(int drive, bool fromTab)
    {
        PrevDrive = ActiveDrive;
        ActiveDrive = drive;

        Drive::Enum().CheckIndex(ActiveDrive);

        if (fromTab)
            Combo.SetCurSel(ActiveDrive);
        else
            Disks.SetHot(ActiveDrive);

        if (ChangeDrive)
            ChangeDrive(ActiveDrive, PrevDrive);

        Focus2View();
    }

    int DriveBar::OnSelectTab(int drive, PCTSTR)
    {
        SetCurrentDrive(drive, true);
        return ActiveDrive;
    }

    LRESULT DriveBar::OnComboNotify(LPNMHDR nmh)
    {
        unsigned code = LOWORD(nmh->code);
        if (WM_KEYUP == code)
        {
            int data = HIWORD(nmh->code);

            //if ((data >= 'A') && (data <= 'Z'))
                //OnComboCommand(CBN_SELENDOK, 0, NULL);

            DH::ThreadPrintf(L"DRIVENFY: Notify %p == %x `%c`\n", nmh->code, code, data);
        }

        
        return 0;
    }

    void DriveBar::OnComboCommand(UINT code, int, HWND win)
    {
        if (CBN_SELENDOK == code)
        {
            int drive = Combo.GetCurSel();
            SetCurrentDrive(drive, false);
        }
        else if (CBN_SELENDCANCEL == code)
            SetCurrentDrive(ActiveDrive, true);
    }

    void DriveBar::CancelDropdownCombo()
    {
        Combo.ShowDropDown(FALSE);
    }

    void DriveBar::DropdownCombo()
    {
        Combo.SetFocus();
        Combo.ShowDropDown(TRUE);
    }

    void DriveBar::UpdateInfo(Drive::Enumerator::Item const& item)
    {
        Hint.SetWindowText(item.ready ? Drive::GetHint(item).GetString() : _LS(StrId_Unmounted2));
    }

    void DriveBar::InitializeDrives(DriveBar& left, DriveBar& right, Drive::Enumerator::ItemVector const& items)
    {
        left.Reset(items);
        right.Reset(items);
    }

    void DriveBar::Clear()
    {
        Disks.Clear();
        Combo.ResetContent();
        Hint.SetWindowTextW(_T(""));
        
        ComboRect.right = ComboRect.left;
        ComboRect.bottom = ComboRect.top + Combo.GetItemHeight(-1);
    }

    void DriveBar::OnLoadContentBegin()
    {
        Clear();
    }

    void DriveBar::OnLoadContentDone()
    {
        Combo.SetImageList(Drive::Enum().GetImageList());

        CRect rc = ComboRect;
        rc.right = rc.left + InitialComboWidth;
        Combo.SetWindowPos(NULL, rc, SWP_NOMOVE | SWP_NOZORDER);
        Combo.SetDroppedWidth(64 + ComboRect.Width() + GetSystemMetrics(SM_CXFRAME) + /* icon width */ 16 + /* margin */ 8);

        Drive::Enum().CheckIndex(ActiveDrive);

        Combo.SetCurSel(ActiveDrive);
        Disks.SetHot(ActiveDrive);

        OnUpdateDrivesList();
    }

    void DriveBar::Reset(Drive::Enumerator::ItemVector const& item)
    {
        OnLoadContentBegin();

        for (Drive::Enumerator::ItemVector::const_iterator it = item.begin(); it != item.end(); ++it)
            Add(*it);

        OnLoadContentDone();
    }

    void DriveBar::OnGo2Root(UINT, int, HWND) 
    {
        Owner.Go2RootDir(); 
        Owner.SetFocus();
    }

    void DriveBar::OnGo2Up(UINT, int, HWND) 
    {
        Owner.Go2UpDir(); 
        Owner.SetFocus();
    }

    void DriveBar::ActualizeDriveIndex(std::wstring const& dstr)
    {
        int n = Drive::Enum().FindByName(dstr);
        if ((-1 != n) && (n != ActiveDrive))
        {
            PrevDrive = ActiveDrive;
            ActiveDrive = n;

            Drive::Enum().CheckIndex(ActiveDrive);
            Combo.SetCurSel(ActiveDrive);
            Disks.SetHot(ActiveDrive);
        }
    }

    void DriveBar::UpdateDriveInfo()
    {
        if (Drive::Enum().IsIndexValid(ActiveDrive))
        {
            Drive::Enumerator::Item const& di = Drive::Enum().Get(ActiveDrive);
            //bool isReady = Drive::IsReady(di);
            //UpdateInfo(di, isReady);
            UpdateInfo(di);
        }
    }

    int DriveBar::GetDrive() const
    {
        return ActiveDrive;
    }

    CString DriveBar::GetDriveName(CPoint const& point) const
    {
        TabBarItem const& di = Disks.GetItem(point);
        return TabBar::IsValid(di) ? di.name.GetString() : _T("");
    }

    void DriveBar::Invalidate() const
    {
        ::InvalidateRect(Disks, NULL, FALSE);
        ::InvalidateRect(Combo, NULL, FALSE);
        ::InvalidateRect(Hint, NULL, FALSE);
        ::InvalidateRect(Go2RootBtn, NULL, FALSE);
        ::InvalidateRect(Go2UpBtn, NULL, FALSE);
    }

    HBRUSH DriveBar::OnCtlColor(CDCHandle dc, HWND)
    {
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(MyTextColor);
        return MyBackBrush;
    }

    void DriveBar::OnDrawItem(int id, PDRAWITEMSTRUCT di)
    {
        CDCHandle dc(di->hDC);
        CRect rc(di->rcItem);

        HPEN lpen = dc.SelectPen(MyPen);

        OnCtlColor(dc, di->hwndItem);

        CF::GradRect(dc, rc, MyButtonBackColor);
        CF::FrameRect(dc, rc);

        CWindow temp(di->hwndItem);
        int textLen = temp.GetWindowTextLength();
        if (textLen > 0)
        {
            CRect rcText(rc);
            rcText.DeflateRect(2, 2);

            CString text(_T('\0'), textLen);
            temp.GetWindowText(text);

            UINT tf = DT_LEFT;

            if (ODT_BUTTON == di->CtlType)
                tf = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

            dc.DrawText(text, textLen, rcText, tf);
        }

        dc.SelectPen(lpen);
    }
}
