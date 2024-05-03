#include "stdafx.h"
#include "win32.host.window.h"
#include "win32.host.app.h"
#include "status.bar.ex.h"
#include "command.line.h"
#include "extern.run.h"
#include "dialogz.find.h"
#include <locale.helper.h>
#include <string.utils.error.code.h>
#include <wtl.column.tree.h>
#include <shell.imagelist.h>
#include "res/resource.h"

namespace Simple
{
    namespace Window
    {
        enum
        {
            StatusBarId = 1000,
            CommandLineId,
            TreeView,
        };

        struct Resizer: public CWindow
                      , public CDialogResize<Resizer>
        {
            BEGIN_DLGRESIZE_MAP(Resizer)
                DLGRESIZE_CONTROL(StatusBarId, DLSZ_SIZE_X | DLSZ_MOVE_Y)
                DLGRESIZE_CONTROL(CommandLineId, DLSZ_SIZE_X)
                DLGRESIZE_CONTROL(TreeView, DLSZ_SIZE_X | DLSZ_SIZE_Y)
            END_DLGRESIZE_MAP()
        };

        bool OnCommandEntered(CString const& line);
        void PopulateTree(CColumnTreeCtrl& ctree);

        static Resizer gResizer;
        static Ui::StatusBar gStatusBar;
        static Ui::CommandLine gCommandLine(OnCommandEntered);
        static CIcon gIcon;
        static CColumnTreeCtrl gTreeView;
        static Sh::Imagelist gShellIcons(false);
        static Sh::Imagelist gShellSmallIcons(true);

        void OnCommand(HWND window, int id, int code, LPARAM param) {}
        void OnActivate(HWND window, UINT state, BOOL bMinimized, HWND wndOther) {}
        void OnSetFocus(HWND window, HWND wndOther) {}
        void OnThemeChanged(HWND window) {}
        void OnTimer(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {}
        void OnLButtonDown(HWND window, UINT message, WPARAM wParam, LPARAM lParam) { /*POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };*/ }
        void OnLButtonUp(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {}
        void OnMouseMove(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {}

        void OnSize(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
        {
            BOOL dummy = FALSE;
            gResizer.OnSize(message, wParam, lParam, dummy);
        }

        void OnPaint(HWND window)
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(window, &ps);

            CFontHandle font((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
            HGDIOBJ lastobj = ::SelectObject(hdc, font);

            ::SelectObject(hdc, lastobj);
            ::EndPaint(window, &ps);
        }

        void ShowFindDialog(PCWSTR dirpath)
        {
            Twins::FindDialog::Config config(gShellSmallIcons);
            Twins::FindDialog dlg(dirpath, config);

            INT_PTR fd = dlg.DoModal();
        }

        void OnKeyDown(HWND window, UINT code, UINT, UINT flags)
        {
            if (VK_ESCAPE == code)
                ::DestroyWindow(window);

            else if (VK_F7 == code)
                ShowFindDialog(L"c:\\Program Files (x86)");
        }

        int OnCreate(HWND window, LPCREATESTRUCT cs)
        {
            CStringW pwd;
            int len = (int)::GetCurrentDirectoryW(4096, pwd.GetBuffer(4096));
            pwd.ReleaseBufferSetLength(len);

            //StartFind(pwd);

            CRect rc;
            ::GetClientRect(window, rc);


            CRect rcStatus;
            if (gStatusBar.Create(window, StatusBarId))
            {
                gStatusBar.GetWindowRect(rcStatus);
                CWindow(window).ScreenToClient(rcStatus);

                CStringW module;
                int len = ::GetModuleFileNameW(NULL, module.GetBuffer(4096), 4096);
                module.ReleaseBufferSetLength(len);

                HICON tempIcon[1] = {0};
                UINT tc = ::ExtractIconEx(module, 0, NULL, tempIcon, 1);
                gIcon.Attach(!tempIcon[0] ? ::LoadIcon(NULL, IDI_APPLICATION) : tempIcon[0]);

                CStringW text;
                CWindow(window).GetWindowText(text);

                gStatusBar.Add((PCWSTR)text, gIcon, 45);
            }

            CRect rcLine = rc;
            rcLine.bottom = rcLine.top + 24;
            gCommandLine.Create(window, rcLine, CommandLineId);

            gCommandLine.SetPath((PCWSTR)pwd);

            CRect rcTree = rc;
            rcTree.DeflateRect(2, 2);
            rcTree.top = rcLine.bottom + 2;
            rcTree.bottom = rcStatus.top - 2;

            if (gTreeView.Create(window, rcTree, NULL
                , WS_CHILD | WS_VISIBLE | WS_BORDER
                | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT
                , 0, TreeView))
            {
                PopulateTree(gTreeView);
            }

            gResizer.Attach(window);
            gResizer.DlgResize_Init(false, false);
            return 0;
        }

        void OnDestroy(HWND window) {}

        bool OnCommandEntered(CString const& line)
        {
            Twins::String command = CT2W(line).m_psz;
            Twins::Extern::Item it(L"CommandLine", command);

            Twins::String pwd;
            DWORD len = ::GetCurrentDirectoryW(65535, pwd.GetBuffer(65536));
            pwd.ReleaseBufferSetLength(len);

            HRESULT hr = Twins::Extern::Run(it, pwd, L"");
            if (ERROR_FILE_NOT_FOUND == hr)
            {
                Twins::Extern::Item cmd(L"CMD", L"%COMSPEC%");

                Twins::String args = L"/C \"";
                args += command;
                args += L"\"";

                hr = Twins::Extern::Run(cmd, pwd, args);
            }

            if (S_OK != hr)
            {
                DropFailBox(_T("Ошибка запуска дочернего процесса"), _T("`%s`\n%d %s")
                    , (PCTSTR)line, hr, Str::ErrorCode<TCHAR>::SystemMessage(hr));

                return false;
            }

            return true;
        }

        static HTREEITEM AppendToTree(WTL::CTreeViewCtrl& tree, HTREEITEM rootItem, HTREEITEM insertAfter, CStringW const& name, int iconIndex)
        {
            TVINSERTSTRUCTW it = {0};

            it.hParent = rootItem;
            it.hInsertAfter = insertAfter;
            it.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
            it.item.pszText = (PWSTR)name.GetString();
            it.item.cchTextMax = name.GetLength();
            it.item.iImage = iconIndex;
            it.item.iSelectedImage = iconIndex;

            return tree.InsertItem(&it);
        }

        static int GetShellIconIndex(CStringW const& path, UINT flags)
        {
            SHFILEINFO info = {0};
            ::SHGetFileInfoW(path, -1, &info, sizeof(info), SHGFI_LINKOVERLAY | SHGFI_SYSICONINDEX | flags);
            return info.iIcon;
        }

        static CImageList gTreeImageList;

        static void PopulateTree(CColumnTreeCtrl& ctree)
        {
            gTreeImageList.Create (16, 16, ILC_COLOR32|ILC_MASK,5,1);

            int nIconFloppy = gTreeImageList.AddIcon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_STOCK_FLOPPY)));
            int nIconFixedDisk = gTreeImageList.AddIcon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_STOCK_FIXEDDISK)));
            int nIconHardDisk = gTreeImageList.AddIcon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_STOCK_HDD)));
            int nIconCDROM = gTreeImageList.AddIcon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_STOCK_CDROM)));
            int nIconNetworkPlace = gTreeImageList.AddIcon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_STOCK_SHAREDFOLDER)));
            int nIconOverlay = gTreeImageList.AddIcon(::LoadIcon(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDI_STOCK_OVL1)));
            gTreeImageList.SetOverlayImage(nIconOverlay, 1);

            gTreeView.GetTreeCtrl().SetImageList(gTreeImageList, LVSIL_NORMAL);

	        ctree.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 180);
	        ctree.InsertColumn(1, _T("Type"), LVCFMT_LEFT, 80);
	        ctree.InsertColumn(2, _T("File System"), LVCFMT_CENTER, 80);
	        ctree.InsertColumn(3, _T("Size"), LVCFMT_RIGHT, 70);
	        ctree.InsertColumn(4, _T("Free"), LVCFMT_RIGHT, 70);

	        HTREEITEM hRoot, hItem;
	
	        hRoot = ctree.GetTreeCtrl().InsertItem(_T("3.5\" Floppy (A:)"), nIconFloppy, nIconFloppy, TVI_ROOT, TVI_FIRST);
	        ctree.SetItemText(hRoot,1,_T("Removable"));
	        ctree.SetItemText(hRoot,2,_T("FAT12"));
	        ctree.SetItemText(hRoot,3,_T("1.44 Mb"));
	        ctree.SetItemText(hRoot,4,_T("100 Kb"));

	        hRoot = ctree.GetTreeCtrl().InsertItem(_T("S312026AS"), nIconHardDisk, nIconHardDisk, TVI_ROOT, hRoot);
	        ctree.SetItemText(hRoot,1,_T("Hard Disk"));
	
	        hItem = ctree.GetTreeCtrl().InsertItem(_T("Local Disk (C:)"), nIconFixedDisk, nIconFixedDisk, hRoot, TVI_FIRST);
        	ctree.SetItemText(hItem,1,_T("Fixed Disk"));
        	ctree.SetItemText(hItem,2,_T("NTFS"));
        	ctree.SetItemText(hItem,3,_T("200 Gb"));
        	ctree.SetItemText(hItem,4,_T("150 Gb"));
        
        	hItem = ctree.GetTreeCtrl().InsertItem(_T("Local Disk (E:)"), nIconFixedDisk, nIconFixedDisk, hRoot, hItem);
        	ctree.SetItemText(hItem,1,_T("Fixed"));
        	ctree.SetItemText(hItem,2,_T("NTFS"));
        	ctree.SetItemText(hItem,3,_T("128 Gb"));
        	ctree.SetItemText(hItem,4,_T("120 Gb"));
        
        	ctree.GetTreeCtrl().Expand(hRoot, TVE_EXPAND);
        
            /*
        	hRoot = ctree.GetTreeCtrl().InsertItem(_T("Maxtor DiamondMax 92048U8"), nIconHardDisk, nIconHardDisk);
        	ctree.SetItemText(hRoot,1,_T("Hard Disk"));
        	
        	hItem = ctree.GetTreeCtrl().InsertItem(_T("Work (D:)"), nIconFixedDisk, nIconFixedDisk, hRoot);
        	ctree.SetItemText(hItem,1,_T("Fixed Disk"));
        	ctree.SetItemText(hItem,2,_T("FAT32"));
        	ctree.SetItemText(hItem,3,_T("40 Gb"));
        	ctree.SetItemText(hItem,4,_T("12 Gb"));
        
        	ctree.GetTreeCtrl().Expand(hRoot, TVE_EXPAND);
        
        	hRoot = ctree.GetTreeCtrl().InsertItem(_T("_NEC CD-RW NR9400-A"), nIconHardDisk,nIconHardDisk);
        	ctree.SetItemText(hRoot,1,_T("CD-ROM"));
        
        	hItem = ctree.GetTreeCtrl().InsertItem(_T("My Music (F:)"), nIconCDROM,nIconCDROM,hRoot);
        	ctree.SetItemText(hItem,1,_T("CD-Rewritable"));
        	ctree.SetItemText(hItem,2,_T("CDFS"));
        	ctree.SetItemText(hItem,3,_T("700 Mb"));
        	ctree.SetItemText(hItem,4,_T("0 Kb"));
        
        	ctree.GetTreeCtrl().Expand(hRoot,TVE_EXPAND);
        	
        	hRoot = ctree.GetTreeCtrl().InsertItem(_T("_NEC DVD-RW ND3550-A"), nIconHardDisk,nIconHardDisk);
        	ctree.SetItemText(hRoot,1,_T("CD-ROM"));
        
        	hItem = ctree.GetTreeCtrl().InsertItem(_T("DVD-ROM Drive (G:)"), nIconCDROM, nIconCDROM, hRoot);
        	ctree.SetItemText(hItem,1,_T("DVD-RW"));
        	
        	ctree.GetTreeCtrl().Expand(hRoot,TVE_EXPAND); 

        	hItem = ctree.GetTreeCtrl().InsertItem(_T("Music on Multimedia"), nIconNetworkPlace, nIconNetworkPlace, TVI_ROOT);
        	ctree.SetItemText(hItem,1,_T("Network Place"));
        	
        	ctree.GetTreeCtrl().SetItemState(hItem, TVIS_OVERLAYMASK, INDEXTOOVERLAYMASK(1));
            */
        }
    }
}
