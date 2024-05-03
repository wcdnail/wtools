#include "stdafx.h"
#if 0
#include "native.window.h"
#include "debug.consts.h"
#include "strut.error.code.h"
#include "strut.human.size.h"
#include "strut.filename.manip.h"
#include "strut.env.vars.h"
#include "panel.view.0.recinfo.h"
#include "panel.scanner.fs.enum.h"
#include "theme.h"
#include <atltypes.h>
#include <windows.gdi.base.h>
#include <windows.gdi.rects.h>
#include "res/resource.h"

namespace Native
{
    namespace Window
    {
#if _UT_GDIBASE
        CF::BitmapDC gBitmapDc0;
#else
        volatile LONG gLoadCancel = 0;
        bool gShowHidenFiles = true;

#if _UT_LISTVIEWCTL
        Twins::PanelView gPanelView(gPanelConf);
#endif

#if _UT_PANELVIEW
        Twins::PanelView0 gPanelView0(gPanelConf);
#endif
#endif

        enum
        {
            ID_PANELVIEW = 1,
        };

        struct Resizer: public CWindow
                      , public CDialogResize<Resizer>
        {
            BEGIN_DLGRESIZE_MAP(Resizer)
#if _UT_PANELVIEW
                DLGRESIZE_CONTROL(ID_PANELVIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
#endif
            END_DLGRESIZE_MAP()
        };

        Resizer gResizer;

        void OnSize(HWND window, UINT type, SIZE const& sz)
        {
        }

        void OnSize(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
        {
            BOOL dummy = FALSE;
            gResizer.OnSize(message, wParam, lParam, dummy);
        }

        void OnCommand(HWND window, int id, int code, LPARAM param)
        {
        }

        void OnActivate(HWND window, UINT state, BOOL bMinimized, HWND wndOther)
        {
#if _UT_PANELVIEW
            gPanelView0.SetFocus();
#endif
        }

        void OnSetFocus(HWND window, HWND wndOther)
        {
#if _UT_PANELVIEW
            gPanelView0.SetFocus();
#endif
        }

        void OnPaint(HWND window)
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(window, &ps);
            HGDIOBJ hLastObj = ::SelectObject(hdc, ::GetStockObject(DEFAULT_GUI_FONT));

            RECT rc;
            CRect rc2 = rc;
            ::GetClientRect(window, &rc);
            ::InflateRect(&rc, -8, -6);

            CF::GradRect(hdc, rc2, 0xff00f0, 0xf000ff, GRADIENT_FILL_RECT_V);

#if _UT_GDIBASE
            gBitmapDc0.Put(hdc, rc, cf::put_at::right | cf::put_at::bottom);
#else
#if _UT_SYSCOLORS
            long x = rc.left;
            long y = rc.top;
            long w = 220;
            long h = 16;

            for (int i=0; i<32; i++)
            {
                HBRUSH hbr = ::CreateSolidBrush(::GetSysColor(i));
                ::SelectObject(hdc, hbr);
                ::Rectangle(hdc, x, y, x+w, y+h);

                PCWSTR pszSysColorName = Debug::Consts::GetSysColorName(i);
                if (pszSysColorName)
                {
                    RECT rcText = { x+w+8, y, x+w+8+w, y+h };
                    ::DrawTextW(hdc, pszSysColorName, (int)::wcslen(pszSysColorName), &rcText, DT_LEFT);
                }

                ::DeleteObject(hbr);

                y += h + 2;
            }
#endif
#endif

            ::SelectObject(hdc, hLastObj);
            ::EndPaint(window, &ps);
        }

        void OnDestroy(HWND window)
        {
#if _UT_LISTVIEWCTL
            gPanelView.DestroyWindow();
#endif
            ::PostQuitMessage(0);
        }

        void OnThemeChanged(HWND window)
        {
            Twins::Theme().OnChanged();
        }

        int OnCreate(HWND window, LPCREATESTRUCT csPtr)
        {
            HINSTANCE hinst = ::GetModuleHandle(NULL);

#if _UT_GDIBASE
            gBitmapDc0.Load(IDB_BK0);
#endif

            CRect rc;
            ::GetClientRect(window, rc);

#if (!_UT_GDIBASE)
            rc.DeflateRect(12, 10);

#if _UT_SYSCOLORS
            rc.left += 460;
#endif

#if _UT_PANELVIEW
            if (!gPanelView0.Create(window, rc, NULL, 0, 0, ID_PANELVIEW))
            {
                HRESULT hr = GetLastError();

                CString message;
                message.Format(_T("—бой Twins::PanelView0::Create(%p, %p) - %s")
                    , window, &rc
                    , Twins::ErrorCode<>::SystemMessage(hr));

                MessageBox(window, message, _T("ќшибка при создании"), MB_ICONSTOP);
            }
            else
            {
                HRESULT hr = S_OK;

                //Twins::PanelScanner::FSEnumerator enumerator(gShowHidenFiles, gLoadCancel, gPanelView0);
                //hr = Twins::FS::Enumerate(Twins::SysEnv::Expand(_T("%HOME%")), _T(""), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(_T("c:\\"), _T(""), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(_T("c:\\"), _T("Program Files (x86)"), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(_T("c:\\"), _T("Users\\Migele"), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(_T("c:\\"), _T("Program Files\\SysinternalsSuite-2010.10.14"), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(_T("c:\\"), _T("Windows\\System32"), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(_T("\\\\Computer\\"), _T(""), _T(""), enumerator);
                //hr = Twins::FS::Enumerate(Twins::SysEnv::Expand(_T("%WINDIR%")), _T(""), _T("*"), enumerator);
                //hr = Twins::FS::Enumerate(Twins::SysEnv::Expand(_T("%SYSTEMROOT%")), _T(""), _T("*"), enumerator);

                //if (S_OK != hr)
                    //::SetWindowText(window, Twins::ErrorCode<>::SystemMessage(hr));
            }
#endif
#if _UT_LISTVIEWCTL
#if _UT_PANELVIEW_SUBCLASSING
            HWND lv = ::CreateWindowEx(LVS_EX_FULLROWSELECT 
                | LVS_EX_GRIDLINES 
                | LVS_EX_DOUBLEBUFFER
                , WC_LISTVIEW, NULL
                , WS_CHILD | WS_VISIBLE 
                | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                | LVS_REPORT 
                | LVS_SHOWSELALWAYS
                | LVS_OWNERDRAWFIXED
                | LVS_EDITLABELS
                | WS_BORDER
                , rc.left, rc.top, rc.Width(), rc.Height()
                , window, NULL, hinst, NULL);

            bool ok = FALSE != gPanelView.SubclassWindow(lv);
#else
            bool ok = NULL != gPanelView.Create(window, rc);
#endif

            if (!ok)
            {
                HRESULT hr = GetLastError();

                CString message;
                message.Format(_T("—бой Twins::PanelView::Create(%p, %p) - %s")
                    , window, &rc
                    , Twins::ErrorCode<>::SystemMessage(hr));

                MessageBox(window, message, _T("ќшибка при создании"), MB_ICONSTOP);

            }
            else
            {
                Twins::PanelView::RecordInfo tempRec;
                for (int i=0; i<10; i++)
                {
                    tempRec.name.Format(_T("Record #%d"), i);
                    gPanelView.Add(tempRec);
                }
            }
#endif
#endif
            gResizer.Attach(window);
            gResizer.DlgResize_Init(false, false);
            return 0;
        }

#ifdef _UT_PANELVIEW
    void OnPv0SelectItem(HWND window, WPARAM wParam, LPARAM lParam)
    {
#if _UT_PANELVIEW_ENABLE_DIR_ENTER
        Twins::PanelView0::RecordInfo const* info = (Twins::PanelView0::RecordInfo const*)lParam;
        if (NULL != info)
        {
            CString pathName = info->pathName;

            if (0 != (FILE_ATTRIBUTE_DIRECTORY & info->attr))
            {
                bool dirUp = info->name == _T("..");
                if (dirUp)
                {
                    CString temp = pathName.Left(pathName.GetLength()-3);
                    int n = temp.ReverseFind(_T('\\'));
                    n = (-1 == n ? 0 : n + 1);
                    pathName = temp.Left(n);
                }

                gPanelView0.Clear();
                
                //Twins::PanelScanner::FSEnumerator enumerator(gShowHidenFiles, gLoadCancel, gPanelView0);
                //HRESULT hr = Twins::FS::Enumerate(pathName, _T(""), _T("*"), enumerator);

                //if (S_OK != hr)
                    //::SetWindowText(window, Twins::ErrorCode<>::SystemMessage(hr));
            }
        }
#endif
    }
#endif 
    }
}
#endif
