#pragma once

#include "file.viewer.view.h"
#include <vector>
#include <atlapp.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <tabsafx/TabbedFrame.h>

namespace Fl
{
    class Entry;
    class List;
}

namespace Twins
{
    class FileViewer: CTabbedChildWindow<CDotNetTabCtrl<CTabViewTabItem> >
                    , CMessageFilter
    {
    private:
        typedef CTabbedChildWindow<CDotNetTabCtrl<CTabViewTabItem> > Super;
        typedef CDotNetTabCtrl<CTabViewTabItem> TabCtrl;

    public:
        typedef std::vector<Fv::Viewer*> ViewerList;
        typedef Fv::ErrorCode ErrorCode;

        struct StandAlone
        {
            static const bool Yes = true;
            static const bool No = false;
        };


        FileViewer(CRect& rc, bool standAlone = StandAlone::No);
        ~FileViewer();

        void Show(Fl::List const& files, CMessageLoop& loop, HWND parent = NULL);

        bool IsTabIndexOk(int index) const;
        void SelectTab(int index);
        void CloseTab(int index);
        void CloseCurrentTabAndExitIfEmpty();
        bool ExitIfEmpty();
        ErrorCode Add(Fl::Entry const& entry);
        CRect const& GetMyRect() const;

    private:
        friend Super;
                
        ViewerList Views;
        Fv::SizeType BlockSize;
        CRect& MyRect;
        CMessageLoop* LoopPtr;
        bool IsStandAlone;
         
        enum 
        { 
            ID_TABS = 1000,
            WM_DESTROYIFEMPTY = WM_USER + 2, 
        };

        virtual BOOL PreTranslateMessage(MSG* message);
        bool InitTabs(Fl::List const* files);
        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        LRESULT OnCompositionChanged(UINT = 0, WPARAM = 0, LPARAM = 0);
        BOOL OnEraseBkgnd(CDCHandle dc);
        void OnSize(UINT, CSize);
        LRESULT OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        void UpdateInfos(Fv::Viewer const* view);
        LRESULT OnSelChange(LPNMHDR pnmh);
        void OnKeyDown(UINT code, UINT, UINT flags);
        void OnSetFocus(HWND last);
        bool DeleteView(int index);
        LRESULT OnClosePage(LPNMHDR pnmh);
        BOOL OnForwardMsg(LPMSG message, DWORD data);
        LRESULT OnDestroyIfEmpty(UINT = 0, WPARAM = 0, LPARAM = 0);

        BEGIN_MSG_MAP_EX(FileViewer)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_FORWARDMSG(OnForwardMsg)
            MESSAGE_HANDLER_EX(0x031e /*WM_DWMCOMPOSITIONCHANGED*/, OnCompositionChanged)
            MSG_WM_SIZE(OnSize)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_SETFOCUS(OnSetFocus)
            MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
            NOTIFY_CODE_HANDLER_EX(CTCN_SELCHANGE, OnSelChange)
            NOTIFY_CODE_HANDLER_EX(CTCN_CLOSE, OnClosePage)
            MESSAGE_HANDLER_EX(WM_DESTROYIFEMPTY, OnDestroyIfEmpty)
            CHAIN_MSG_MAP(Super)
        END_MSG_MAP()
    };

    inline CRect const& FileViewer::GetMyRect() const { return MyRect; }
}
