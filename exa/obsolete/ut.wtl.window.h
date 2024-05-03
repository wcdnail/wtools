#pragma once 

#include "ut.wtl.control.h"
#include <assistance/sensors/sensor.facility.h>
#include <atlapp.h>
#include <atlframe.h>
#include <atlctrls.h>

namespace Ut
{
    void DropFailBox(PCTSTR caption, PCTSTR format, ...);

    typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
                     , WS_EX_APPWINDOW
                   //| WS_EX_COMPOSITED
                      > HLWindowTraits;

    class HLWindow: public CFrameWindowImpl<HLWindow, CWindow, HLWindowTraits>
                  , public CUpdateUI<HLWindow>
                  , public CMessageFilter
                  , public CIdleHandler
                  , public CDialogResize<HLWindow>
    {
    public:
        typedef CFrameWindowImpl<HLWindow, CWindow, HLWindowTraits> Super;
        typedef CUpdateUI<HLWindow> SuperUpdateUI;
        typedef CDialogResize<HLWindow> SuperResizer;

        HLWindow(CMessageLoop& loop);
        ~HLWindow();

        static WTL::CFrameWndClassInfo& GetWndClassInfo();
        static _AtlDlgResizeMap const* GetDlgResizeMap();
        static _AtlUpdateUIMap const* GetUpdateUIMap();

        BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);

        CMessageLoop& GetMessageLoop();
        CIcon const& GetMainIcon() const;

        HLControl& GetHlControl() { return control_; }

    private:
        CMessageLoop& messageLoop_;
        BOOL messageHandled_;
        CIcon icon_;
        //Assist::Sensors::Facility sensorsFacility_;
        //CStatic sensorsContainer_;
        HLControl control_;

        BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
        BOOL IsMsgHandled() const;
        void SetMsgHandled(BOOL bHandled);

        void OnException(char const* = NULL);

        virtual BOOL PreTranslateMessage(MSG*);
        virtual BOOL OnIdle();

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle dc);

    private:
        HLWindow(HLWindow const&);
        HLWindow& operator = (HLWindow const&);
    };

}
