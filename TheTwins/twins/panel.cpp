#include "stdafx.h"
#include "twins.funcs.h"
#include "panel.h"
#include "shell.helpers.h"
#include "drive.enum.h"
#include "drive.info.h"
#include "status.ui.h"
#include "dialogz.name.h"
#include "shell.menus.h"
#include "file.operation.generic.h"
#include "file.copy.h"
#include <dh.timer.h>
#include <dh.tracing.h>
#include <twins.lang.strings.h>
#include <string.utils.error.code.h>
#include <windows.wtl.message.h>
#include <twins.lang.strings.h>
#include <atlconv.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

namespace Twins
{
    static int _AutoPanelId = 0;

    Panel::Panel()
        : Id(_AutoPanelId++)
        , View(Id)
        , Drives(*this)
        , Paths(*this)
    {}

    Panel::~Panel()
    {}

    BOOL Panel::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        ::GetClientRect(m_hWnd, rc);
        dc.FillSolidRect(rc, 0x202020);
        return TRUE;
    }

    int Panel::OnCreate(LPCREATESTRUCT)
    {
        CRect rc;
        ::GetClientRect(m_hWnd, rc);

        static const int InitialDriveBarHeight = 48;
        static const int InitialPathBarHeight = 18;

        CRect rcDrives = rc;
        rcDrives.bottom = rcDrives.top + InitialDriveBarHeight;

        CRect rcPath = rc;
        rcPath.top = rcDrives.bottom + 2;
        rcPath.bottom = rcPath.top + InitialPathBarHeight;

        CRect rcView = rc;
        rcView.top = rcPath.bottom + 1;
        rcView.bottom -= 2;

        Drives.Create(m_hWnd, rcDrives, NULL, WS_CHILD  | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, ID_DRIVE_SELECTOR);
        Paths.Create(m_hWnd, rcPath, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, ID_PATH_BAR);
        View.Create(m_hWnd, rcView, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, ID_PANEL_VIEW);

        Drives.OnChangeDrive() = boost::bind(&Panel::OnDriveSelect, this, _1, _2);
        View.Renamer = boost::bind(&Panel::OnRename, this, _1, _2);
        View.Scanner.OnFetchDone = boost::bind(&Panel::FetchContentDone, this, _1);

        DlgResize_Init(false, false);
        return 0;
    }

    LRESULT Panel::OnPathBarHeightChanged(UINT, WPARAM wParam, LPARAM)
    {
        int height = (int)wParam;

        CRect rcView;
        View.GetWindowRect(rcView);
        ScreenToClient(rcView);

        rcView.top += height;
        View.SetWindowPos(NULL, rcView, SWP_NOZORDER);

        return 0;
    }

    void Panel::OnDestroy()
    {
        LoadContentCancel();
        SetMsgHandled(FALSE);
    }

    void Panel::OnSetFocus(HWND hwnd)
    {
        Paths.Invalidate();
        View.SetFocus();
        SetActivePanel(*this);
        //Dh::ThreadPrintf(L"   PANEL: фокус - %d hwnd=0x%x m_hWnd=0x%x\n", Id, hwnd, m_hWnd);
    }

    void Panel::OnKeyDown(UINT code, UINT rep, UINT flags)
    {
        Message::RedirectTo(GetParent().GetParent(), *this);
    }

    void Panel::OnThemeChanged()
    {
        Invalidate();
    }

    void Panel::Invalidate() const
    {
        Drives.Invalidate();
        Paths.Invalidate();
        View.Invalidate();
    }

    void Panel::OnDriveSelect(int drive, int prev)
    {
        if (prev == drive)
            return ;

        Paths.SetPathHint(_T(""));

        Drive::Enumerator::Item const& info = Drive::Enum().Get(drive);
        bool loadContent = Drive::IsReady(info);
        Drives.UpdateInfo(info);

        if (loadContent)
        {
            GetDirManager().OnChangeDrive(info.path);
            FetchContent(NULL);
        }
        else
            View.Clear();
    }

    void Panel::OnEnter(FItem const* it)
    {
#pragma message(_TODO("Implementation"))
//        if (it->IsDir())
//            ChangeDir(it);
//
//        else
//        {
//#pragma message(_TODO("Warning! Ask - how and who opens. Ask - before run binary!!!"))
//
//            //std::wstring ename = info.GetFilename().c_str();
//            //Extern::Item extrn(ename, info.GetPath());
//            //HRESULT hr = Extern::Run(extrn, GetDirManager().FullPath().native(), L"", 1, NULL);
//            //
//            //SetMainframeStatus(boost::system::error_code((int)(E_PENDING == hr ? 0 : hr), boost::system::system_category())
//            //    , info.LoadShellIcon(), _LS(StrId_Launchs), ename.c_str());
//        }
    }

    void Panel::EnableControls(BOOL enable)
    {
        ::EnableWindow(Paths, enable);
        ::EnableWindow(Drives, enable);
    }

    void Panel::FetchContent(wchar_t const* path)
    {
        LoadContentCancel();
        Paths.SetPathHint(_T("..."));
        View.OnLoadContentBegin();
        EnableControls(FALSE);
        Drives.ActualizeDriveIndex(View.Scanner.Path.GetDrive());
        Drives.UpdateDriveInfo();
        View.Scanner.Fetch(path);
    }

    void Panel::LoadContentCancel()
    {
        View.Scanner.Cancel();
    }

    void Panel::FetchContentDone(double elapsed)
    {
        EnableControls(TRUE);

        std::wstring const& fullpath = GetDirManager().FullPath().wstring();
        SetMainframeStatus(View.Scanner.ErrorCode, LoadShellIcon(fullpath.c_str()), _LS(StrId_Loadings8fsec), fullpath.c_str(), elapsed);

        Paths.SetPathHint(fullpath.c_str());
        //Drives.UpdateDriveInfo();
        View.OnLoadContentDone();
    }

    bool Panel::OnRename(FItem const* it, wchar_t const* newname)
    {
#pragma message(_TODO("Implementation"))
        boost::system::error_code ec;

        //boost::filesystem::path newpath = DirMan.FullPath();
        //newpath /= newname;

        //boost::filesystem::rename(info.GetPathObj(), newpath, ec);
        //SetMainframeStatus(ec, NULL, _LS(StrId_Renames), info.GetFilename().c_str());
        //
        //if (!ec)
        //    SetSelectionAfter(newpath.filename().wstring());

        return !ec;
    }

    void Panel::ChangeDir(FItem const* it)
    {
#pragma message(_TODO("Implementation"))
        //if (NULL != it)
        //{
        //    FetchPanelContent(it->GetFilename());
        //}

        //DirectoryManager::ErrorCode ec = DirMan.Cd(info, &SelectAfter);
        //if (!ec)
            //Reload();

        //else
        //{
        //    UiStatus().AddFormat(_LS(StrId_ErrordSunableentertodirectorys)
        //        , ec.value(), ec.message().c_str(), info.GetPath().c_str());
        //}

        ////DirMan.SetPath(std::wstring((PCWSTR)dirpath));
        //Reload();
    }

    void Panel::Go2RootDir()
    {
        if (GetDirManager().Go2RootDir(View.Scanner.SelectedName))
            Reload();
    }

    void Panel::Go2UpDir()
    {
        if (GetDirManager().Go2UpperDir(View.Scanner.SelectedName))
            Reload();
    }

    void Panel::UpdateDriveInfoIfSame(Panel& other)
    {
        if (Drives.GetDrive() == other.Drives.GetDrive())
            Drives.UpdateDriveInfo();
    }

    void Panel::ShowShellContextMenu(std::wstring const& pathname, CPoint point)
    {
#pragma message(_TODO("Implementation"))
        //Fl::List files;
        //files.PushBack(Fl::Entry::MakeStringEntry(pathname));
        //
        //ClientToScreen(&point);
        //TrackShellContextMenu((HWND)View, point, files);
    }

    void Panel::OnContextMenu(HWND sender, CPoint point)
    {
        std::wstring pathname;

        if (sender == (HWND)View)
        {
            if (View.Scanner.Path.IsRootPath())
                pathname = View.Scanner.Path.FullPath().native().substr(0, 2);
            else
                pathname = View.Scanner.Path.FullPath().native() + L"\\";
        }
        else if (sender == (HWND)Drives)
        {
            pathname = (PCWSTR)Drives.GetDriveName(point);
            if (!pathname.empty())
                pathname += L":";
        }

        if (!pathname.empty())
            ShowShellContextMenu(pathname, point);
    }

    void Panel::SetSelectionAfter(std::wstring const& name)
    {
        if (View.Scanner.SelectedName.empty())
            View.Scanner.SelectedName = name;
    }

    void Panel::CreateDir()
    {
        NameDialog dlg(View.GetHotFilename().c_str(), _LS(StrId_Enterdirectoryfoldername), _LS(StrId_Newdirectoryfolder));

        INT_PTR rv = dlg.DoModal(m_hWnd);
        if (IDOK == rv)
        {
            boost::filesystem::path newpath = GetDirManager().FullPath();
            boost::filesystem::path newname = dlg.GetText();
            newpath /= newname;

            boost::system::error_code ec;
            if (!boost::filesystem::create_directories(newpath, ec))
                ec.assign(ERROR_ALREADY_EXISTS, boost::system::system_category());

            SetMainframeStatus(ec.value(), NULL, _LS(StrId_Creatingdirectorys), newname.c_str());

            if (!ec)
                SetSelectionAfter(newname.filename().wstring());
        }
    }

    void Panel::OnDropFiles(HDROP dropInfo)
    {
        UINT dropedCount = ::DragQueryFile(dropInfo, 0xffffffff, NULL, 0);
        if (dropedCount < 1)
            return ;

        Fl::List files;
        files.Reserve(dropedCount);

        for (UINT i = 0; i < dropedCount; i++)
        {
            UINT len = ::DragQueryFile(dropInfo, i, NULL, 0);
            if (len > 0)
            {
                CString filename;

                ++len;
                if (::DragQueryFile(dropInfo, i, filename.GetBufferSetLength(len), len))
                {
                    filename.ReleaseBufferSetLength(len);
                    files.PushBack(boost::filesystem::path((PCWSTR)filename));
                }
            }
        }

        OperationResult rv = PerformOperation(files, CopyFiles(GetCurrentPath()), m_hWnd);
        SetMainframeStatus(rv.value(), NULL, _LS(StrId_Copyfilesinamountofd), files.Count());
    }

    void Panel::StartPathEdit() { Paths.EditPath(); }

    void Panel::AddCurrentPathToBookmarks() 
    {
#pragma message(_TODO("Implementation"))
        _NotImplementedYet(_LS(StrId_Addpathtobookmarks)); 
    }

    void Panel::RunViewer()
    {
#pragma message(_TODO("Implementation"))
        _NotImplementedYet(_LS(StrId_View)); 
    }

    std::wstring Panel::CreateEntry()
    {
        NameDialog dlg(View.GetHotFilename().c_str(), _LS(StrId_Enterfilename), _LS(StrId_Newfile));
        INT_PTR rv = dlg.DoModal(m_hWnd);
        if (IDOK == rv)
        {
            boost::filesystem::path newpath = GetDirManager().FullPath();
            boost::filesystem::path newname = dlg.GetText();
            newpath /= newname;
        
            HANDLE hnew = ::CreateFile(newpath.c_str(), 0, 0, NULL, CREATE_NEW, 0, NULL);
            HRESULT hr = ::GetLastError();
            if (INVALID_HANDLE_VALUE != hnew)
                ::CloseHandle(hnew);
        
            SetMainframeStatus(hr, NULL, _LS(StrId_Creatingfiles), newname.c_str());
        
            if (0 == hr)
            {
                SetSelectionAfter(newname.filename().wstring());
                return newpath.native();
            }
        }

        return L"";
    }
}
