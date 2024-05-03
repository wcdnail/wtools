#include "stdafx.h"
#include "ut.wtl.window.h"
#include "status.ui.h"
#include <assistance/sensors/memory.sensor.h>
#include <crash.report/crash.report.h>
#include <debug.console.2/debug.console.2.h>
#include "res/resource.h"

namespace Ut
{
    enum MainFrame_ControlIds
    {
        ID_TOOLBAR = ATL_IDW_TOOLBAR,
        ID_STATUSBAR = ATL_IDW_STATUS_BAR,
        ID_VSPLITTER = ID_STATUSBAR + 2000,
        ID_HSPLITTER,
        ID_RIGHTTOPCTL,
        ID_SENSORSCONTAINER,
        ID_HLCONTROL,

        SensorsHeight = 90,
    };

    WTL::CFrameWndClassInfo& HLWindow::GetWndClassInfo()
    { 
        static WTL::CFrameWndClassInfo wc =
        {
            { sizeof(WNDCLASSEX)
            , CS_OWNDC
            , StartWindowProc
            , 0, 0
            , NULL
            , NULL
            , NULL
            , (HBRUSH)(COLOR_WINDOW)
            , NULL
            , _T("UT:HLWindow")
            , NULL 
            }
            , NULL, NULL, IDC_ARROW, TRUE, 0, _T(""), IDR_MAINFRAME
        };

        return wc;
    }

    CMessageLoop& HLWindow::GetMessageLoop() { return messageLoop_; }
    CIcon const& HLWindow::GetMainIcon() const { return icon_; }
    BOOL HLWindow::IsMsgHandled() const { return messageHandled_; }
    void HLWindow::SetMsgHandled(BOOL bHandled) { messageHandled_ = bHandled; }

    HLWindow::HLWindow(CMessageLoop& loop)
        : messageLoop_(loop)
        , messageHandled_(FALSE)
    {
    }

    HLWindow::~HLWindow()
    {
    }

    void HLWindow::OnException(char const* exMessage /* = NULL */)
    {
        HRESULT code = GetLastError();
        ::CRASH_REPORT(exMessage, code);
        ::PostQuitMessage(code);
    }

    BOOL HLWindow::PreTranslateMessage(MSG* messageLoop)
    {
        return Super::PreTranslateMessage(messageLoop);
    }

    BOOL HLWindow::OnIdle()
    {
        UIUpdateToolBar();
        UIUpdateStatusBar();
        return FALSE;
    }

    CUpdateUIBase::_AtlUpdateUIMap const* HLWindow::GetUpdateUIMap()
    { 
        static const _AtlUpdateUIMap upateUIMap[] =
        {
            UPDATE_ELEMENT(ID_TOOLBAR, UPDUI_TOOLBAR)
            UPDATE_ELEMENT(ID_STATUSBAR, UPDUI_STATUSBAR)
            { (WORD)-1, 0 }
        };

        return upateUIMap;
    }

    HLWindow::SuperResizer::_AtlDlgResizeMap const* HLWindow::GetDlgResizeMap()
    { 
        static const _AtlDlgResizeMap theMap[] = 
        {
            DLGRESIZE_CONTROL(ID_TOOLBAR, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_STATUSBAR, DLSZ_SIZE_X | DLSZ_MOVE_Y)
            //DLGRESIZE_CONTROL(ID_SENSORSCONTAINER, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_HLCONTROL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
            { -1, 0 }
        };
        return theMap;
    }

    BOOL HLWindow::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        BOOL bOldMsgHandled = messageHandled_;
        BOOL bRet = _ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
        messageHandled_ = bOldMsgHandled;
        return bRet;
    }

    BOOL HLWindow::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    { 
        try
        {
            BOOL bHandled = TRUE;
            switch(dwMsgMapID)
            {
            case 0:
                MSG_WM_CREATE(OnCreate)
                    MSG_WM_DESTROY(OnDestroy)
                    MSG_WM_ERASEBKGND(OnEraseBkgnd)
                    CHAIN_MSG_MAP(SuperResizer)
                    CHAIN_MSG_MAP(SuperUpdateUI)
                    CHAIN_MSG_MAP(Super)
                    break;
            default:
                ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
                ATLASSERT(FALSE); 
                break; 
            } 
        }
        catch (std::exception const& ex)
        {
            OnException(ex.what());
        }
        catch(...)
        {
            OnException();
        }

        return FALSE;
    }

    BOOL HLWindow::OnEraseBkgnd(CDCHandle dc)
    {
        SetMsgHandled(FALSE);
        return TRUE;
    }

    void HLWindow::OnDestroy()
    {
        ::PostQuitMessage(0); // FIXME: exiting...

        messageLoop_.RemoveMessageFilter(this);
        messageLoop_.RemoveIdleHandler(this);

        SetMsgHandled(FALSE);
    }

    int HLWindow::OnCreate(LPCREATESTRUCT cs)
    {
        icon_.LoadIcon(IDR_MAINFRAME);
        if (NULL != icon_.m_hIcon)
        {
            SetIcon(icon_, TRUE);
            SetIcon(icon_, FALSE);
        }

        HWND toolBarHandle = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, TRUE
            , WS_CHILD | WS_VISIBLE 
            | WS_CLIPCHILDREN | WS_CLIPSIBLINGS 
            | RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_AUTOSIZE
            | TBSTYLE_FLAT);

        if (NULL != toolBarHandle)
        {
            CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
            AddSimpleReBarBand(toolBarHandle);
            UIAddToolBar(toolBarHandle);
        }

        CreateSimpleStatusBar(_T(""));
        if (m_hWndStatusBar)
        {
            UIAddStatusBar(m_hWndStatusBar);
            Twins::Status().Attach(m_hWndStatusBar);
        }

        CRect rc;
        GetClientRect(rc);
        UpdateBarsPosition(rc, FALSE);

        CRect rcSensors(rc.left + 2, rc.top + 2, rc.right - 2, rc.top + SensorsHeight + 2);
        //sensorsFacility_.Prepare(m_hWnd, rcSensors, ID_SENSORSCONTAINER);

        //sensorsFacility_.AddSensor<Assist::Sensors::MemoryLoad>();
        //sensorsFacility_.AddSensor<Assist::Sensors::FreePhysicalMemory>();
        //sensorsFacility_.AddSensor<Assist::Sensors::FreeVirtualMemory>();
        //sensorsFacility_.AddSensor<Assist::Sensors::FreePagefileMemory>();

        CRect rcControl(rc.left + 2, rcSensors.bottom + 2, rc.right - 2, rc.bottom - 2);
        control_.Create(m_hWnd, rcControl, NULL, 0, 0, ID_HLCONTROL);

        DlgResize_Init(false, false);

        messageLoop_.AddMessageFilter(this);
        messageLoop_.AddIdleHandler(this);

        return 1;
    }
}
