#include "stdafx.h"
#if 0
#include "ut.wtl.window.h"
#include <dh.tracing.h>
#include <locale.helper.h>
#include <string.utils.error.code.h>

namespace Twins
{
}

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <dbt.h>
#include "theme.h"
#include "drive.enum.h"

namespace NewDriveTest
{
    // Forward declarations
    void OutputMessage(HWND hOutWnd, WPARAM wParam, LPARAM lParam);
    void ErrorHandler(LPTSTR lpszFunction);

    HDEVNOTIFY DoRegisterDeviceInterfaceToHwnd(HWND hWnd)
    {
        DEV_BROADCAST_DEVICEINTERFACE dbdi = {0};
        dbdi.dbcc_size = sizeof(dbdi);
        dbdi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

        HDEVNOTIFY rv = RegisterDeviceNotification(hWnd, &dbdi, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
        if (!rv) 
            ErrorHandler(TEXT("RegisterDeviceNotification"));

        return rv;
    }

    LRESULT OnDeviceChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, HWND hEditWnd);

    //
    // MessagePump
    //
    void MessagePump(
        HWND hWnd
        )
        // Routine Description:
        //     Simple main thread message pump.
        //

        // Parameters:
        //     hWnd - handle to the window whose messages are being dispatched

        // Return Value:
        //     None.
    {
        MSG msg; 
        int retVal;

        // Get all messages for any window that belongs to this thread,
        // without any filtering. Potential optimization could be
        // obtained via use of filter values if desired.

        while( (retVal = GetMessage(&msg, NULL, 0, 0)) != 0 ) 
        { 
            if ( retVal == -1 )
            {
                ErrorHandler(TEXT("GetMessage"));
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } 
    }

    INT_PTR WINAPI WinProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRet = 1;
        static HDEVNOTIFY hDeviceNotify;
        static HWND hEditWnd;
        
        switch (message)
        {
        case WM_CREATE:
            //hDeviceNotify = DoRegisterDeviceInterfaceToHwnd(hWnd);
            //if (!hDeviceNotify )
                //ErrorHandler(TEXT("DoRegisterDeviceInterfaceToHwnd"));

            hEditWnd = CreateWindow(TEXT("EDIT"),// predefined class 
                NULL,        // no window title 
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
                ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 
                0, 0, 0, 0,  // set size in WM_SIZE message 
                hWnd,        // parent window 
                (HMENU)1,    // edit control ID 
                (HINSTANCE) GetWindowLong(hWnd, (-6)), 
                NULL);       // pointer not needed 

            if ( hEditWnd == NULL )
            {
                // Terminate on failure.
                ErrorHandler(TEXT("CreateWindow: Edit Control"));
                ExitProcess(1);
            }

            ::SendMessage(hEditWnd, WM_SETFONT, (WPARAM)Twins::Theme().fixedFont.m_hFont, 1); 
            break;

        case WM_SETFOCUS: 
            SetFocus(hEditWnd); 
            break;

        case WM_SIZE: 
            // Make the edit control the size of the window's client area. 
            MoveWindow(hEditWnd, 
                0, 0,                  // starting x- and y-coordinates 
                LOWORD(lParam),        // width of client area 
                HIWORD(lParam),        // height of client area 
                TRUE);                 // repaint window 

            break;

        case WM_DEVICECHANGE:
            lRet = OnDeviceChanged(hWnd, message, wParam, lParam, hEditWnd);
            break;

        case WM_CLOSE:
            //if (!UnregisterDeviceNotification(hDeviceNotify) )
                //ErrorHandler(TEXT("UnregisterDeviceNotification")); 

            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            lRet = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

        return lRet;
    }

    static const TCHAR WND_CLASS_NAME[] = TEXT("SampleAppWindowClass");

    BOOL InitWindowClass()
    {
        WNDCLASSEX wndClass;

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wndClass.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
        wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(WinProcCallback);
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hIcon = LoadIcon(0,IDI_APPLICATION);
        wndClass.hbrBackground = CreateSolidBrush(RGB(192,192,192));
        wndClass.hCursor = LoadCursor(0, IDC_ARROW);
        wndClass.lpszClassName = WND_CLASS_NAME;
        wndClass.lpszMenuName = NULL;
        wndClass.hIconSm = wndClass.hIcon;


        if ( ! RegisterClassEx(&wndClass) )
        {
            ErrorHandler(TEXT("RegisterClassEx"));
            return FALSE;
        }
        return TRUE;
    }

    int Main(HINSTANCE hInstanceExe, HINSTANCE, PTSTR, int nCmdShow)
    {
        if ( ! InitWindowClass() )
            return -1;

        HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_APPWINDOW, WND_CLASS_NAME,
            _T("Device change notify"), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, 640, 480, NULL, NULL, hInstanceExe, NULL);

        if ( hWnd == NULL )
        {
            ErrorHandler(TEXT("CreateWindowEx: main appwindow hWnd"));
            return -1;
        }

        ShowWindow(hWnd, SW_SHOWNORMAL);
        UpdateWindow(hWnd);

        MessagePump(hWnd);

        return 1;
    }

    void OutputMessage(HWND hOutWnd, WPARAM, LPARAM lParam)
    {
        LRESULT   lResult;
        LONG      bufferLen;
        LONG      numLines;
        LONG      firstVis;

        // Make writable and turn off redraw.
        lResult = SendMessage(hOutWnd, EM_SETREADONLY, FALSE, 0L);
        lResult = SendMessage(hOutWnd, WM_SETREDRAW, FALSE, 0L);

        // Obtain current text length in the window.
        bufferLen = SendMessage (hOutWnd, WM_GETTEXTLENGTH, 0, 0L);
        numLines = SendMessage (hOutWnd, EM_GETLINECOUNT, 0, 0L);
        firstVis = SendMessage (hOutWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
        lResult = SendMessage (hOutWnd, EM_SETSEL, bufferLen, bufferLen);

        // Write the new text.
        lResult = SendMessage (hOutWnd, EM_REPLACESEL, 0, lParam);

        // See whether scrolling is necessary.
        if (numLines > (firstVis + 1))
        {
            int        lineLen = 0;
            int        lineCount = 0;
            int        charPos;

            // Find the last nonblank line.
            numLines--;
            while(!lineLen)
            {
                charPos = SendMessage(
                    hOutWnd, EM_LINEINDEX, (WPARAM)numLines, 0L);
                lineLen = SendMessage(
                    hOutWnd, EM_LINELENGTH, charPos, 0L);
                if(!lineLen)
                    numLines--;
            }
            // Prevent negative value finding min.
            lineCount = numLines - firstVis;
            lineCount = (lineCount >= 0) ? lineCount : 0;

            // Scroll the window.
            lResult = SendMessage(
                hOutWnd, EM_LINESCROLL, 0, (LPARAM)lineCount);
        }

        // Done, make read-only and allow redraw.
        lResult = SendMessage(hOutWnd, WM_SETREDRAW, TRUE, 0L);
        lResult = SendMessage(hOutWnd, EM_SETREADONLY, TRUE, 0L);
    }  

    void ErrorHandler(LPTSTR lpszFunction) 
    { 

        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        // Display the error message and exit the process.

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
            (lstrlen((LPCTSTR)lpMsgBuf)
            + lstrlen((LPCTSTR)lpszFunction)+40)
            * sizeof(TCHAR)); 
        StringCchPrintf((LPTSTR)lpDisplayBuf, 
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s"), 
            lpszFunction, dw, lpMsgBuf); 
        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, NULL, MB_OK); 

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
    }

    #define _RetStr(X) case X: return L#X;

    PCWSTR DbtDeviceTypeString(DWORD type)
    {
        switch (type)
        {
        _RetStr(DBT_DEVTYP_OEM                      ); // 0x00000000  // oem-defined device type
        _RetStr(DBT_DEVTYP_DEVNODE                  ); // 0x00000001  // devnode number
        _RetStr(DBT_DEVTYP_VOLUME                   ); // 0x00000002  // logical volume
        _RetStr(DBT_DEVTYP_PORT                     ); // 0x00000003  // serial, parallel
        _RetStr(DBT_DEVTYP_NET                      ); // 0x00000004  // network resource
#if(WINVER >= 0x040A)
        _RetStr(DBT_DEVTYP_DEVICEINTERFACE          ); // 0x00000005  // device interface class
        _RetStr(DBT_DEVTYP_HANDLE                   ); // 0x00000006  // file system handle
#endif /* WINVER >= 0x040A */

#if(WINVER >= _WIN32_WINNT_WIN7)
        _RetStr(DBT_DEVTYP_DEVINST                  ); // 0x00000007  // device instance
#endif /* WINVER >= _WIN32_WINNT_WIN7 */
        }

        return L"BAD";
    }

    PCWSTR DbtEventString(DWORD event)
    {
        switch (event)
        {
        _RetStr(DBT_APPYBEGIN                       ); // 0x0000
        _RetStr(DBT_APPYEND                         ); // 0x0001
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_DEVNODES_CHANGED
         * lParam  = 0
         *
         *      send when configmg finished a process tree batch. Some devnodes
         *      may have been added or removed. This is used by ring3 people which
         *      need to be refreshed whenever any devnode changed occur (like
         *      device manager). People specific to certain devices should use
         *      DBT_DEVICE* instead.
         */
        
        _RetStr(DBT_DEVNODES_CHANGED                ); // 0x0007
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_QUERYCHANGECONFIG
         * lParam  = 0
         *
         *      sent to ask if a config change is allowed
         */
        
        _RetStr(DBT_QUERYCHANGECONFIG               ); // 0x0017
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_CONFIGCHANGED
         * lParam  = 0
         *
         *      sent when a config has changed
         */
        
        _RetStr(DBT_CONFIGCHANGED                   ); // 0x0018
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_CONFIGCHANGECANCELED
         * lParam  = 0
         *
         *      someone cancelled the config change
         */
        
        _RetStr(DBT_CONFIGCHANGECANCELED            ); // 0x0019
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_MONITORCHANGE
         * lParam  = new resolution to use (LOWORD=x, HIWORD=y)
         *           if 0, use the default res for current config
         *
         *      this message is sent when the display monitor has changed
         *      and the system should change the display mode to match it.
         */
        
        _RetStr(DBT_MONITORCHANGE                   ); // 0x001B
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_SHELLLOGGEDON
         * lParam  = 0
         *
         *      The shell has finished login on: VxD can now do Shell_EXEC.
         */
        
        _RetStr(DBT_SHELLLOGGEDON                   ); // 0x0020
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_CONFIGMGAPI
         * lParam  = CONFIGMG API Packet
         *
         *      CONFIGMG ring 3 call.
         */
        _RetStr(DBT_CONFIGMGAPI32                   ); // 0x0022
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_VXDINITCOMPLETE
         * lParam  = 0
         *
         *      CONFIGMG ring 3 call.
         */
        _RetStr(DBT_VXDINITCOMPLETE                 ); // 0x0023
        
        /*
         * Message = WM_DEVICECHANGE
         * wParam  = DBT_VOLLOCK*
         * lParam  = pointer to VolLockBroadcast structure described below
         *
         *      Messages issued by IFSMGR for volume locking purposes on WM_DEVICECHANGE.
         *      All these messages pass a pointer to a struct which has no pointers.
         */
        
        _RetStr(DBT_VOLLOCKQUERYLOCK                ); // 0x8041
        _RetStr(DBT_VOLLOCKLOCKTAKEN                ); // 0x8042
        _RetStr(DBT_VOLLOCKLOCKFAILED               ); // 0x8043
        _RetStr(DBT_VOLLOCKQUERYUNLOCK              ); // 0x8044
        _RetStr(DBT_VOLLOCKLOCKRELEASED             ); // 0x8045
        _RetStr(DBT_VOLLOCKUNLOCKFAILED             ); // 0x8046
        
        _RetStr(DBT_NO_DISK_SPACE                   ); // 0x0047
        
        _RetStr(DBT_LOW_DISK_SPACE                  ); // 0x0048
        
        _RetStr(DBT_CONFIGMGPRIVATE                 ); // 0x7FFF
        
        /*
         * The following messages are for WM_DEVICECHANGE. The immediate list
         * is for the wParam. ALL THESE MESSAGES PASS A POINTER TO A STRUCT
         * STARTING WITH A DWORD SIZE AND HAVING NO POINTER IN THE STRUCT.
         *
         */
        _RetStr(DBT_DEVICEARRIVAL                   ); // 0x8000  // system detected a new device
        _RetStr(DBT_DEVICEQUERYREMOVE               ); // 0x8001  // wants to remove, may fail
        _RetStr(DBT_DEVICEQUERYREMOVEFAILED         ); // 0x8002  // removal aborted
        _RetStr(DBT_DEVICEREMOVEPENDING             ); // 0x8003  // about to remove, still avail.
        _RetStr(DBT_DEVICEREMOVECOMPLETE            ); // 0x8004  // device is gone
        _RetStr(DBT_DEVICETYPESPECIFIC              ); // 0x8005  // type specific event
        
#if(WINVER >= 0x040A)
        _RetStr(DBT_CUSTOMEVENT                     ); // 0x8006  // user-defined event
#endif /* WINVER >= 0x040A */

#if(WINVER >= _WIN32_WINNT_WIN7)
        _RetStr(DBT_DEVINSTENUMERATED               ); // 0x8007  // system detected a new device
        _RetStr(DBT_DEVINSTSTARTED                  ); // 0x8008  // device installed and started
        _RetStr(DBT_DEVINSTREMOVED                  ); // 0x8009  // device removed from system
        _RetStr(DBT_DEVINSTPROPERTYCHANGED          ); // 0x800A  // a property on the device changed
#endif /* WINVER >= _WIN32_WINNT_WIN7 */
        }

        return L"_UNK";
    }

    LRESULT OnDeviceChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, HWND hEditWnd)
    {
        DWORD event = wParam;
        CStringW out(DbtEventString(event));

        if (NULL != lParam)
        {
            PDEV_BROADCAST_HDR hdr = (PDEV_BROADCAST_HDR)lParam;
            ATLTRACE(L"sizeof(DEV_BROADCAST_HDR) = @%d\n", sizeof(DEV_BROADCAST_HDR));

            DWORD type = hdr->dbch_devicetype;
            out.AppendFormat(L" @%d %s(%d)", hdr->dbch_size
                , DbtDeviceTypeString(type)
                , type
                );

            bool haveVolumeInfo = DBT_DEVTYP_VOLUME == type;

            PDEV_BROADCAST_VOLUME vlm = haveVolumeInfo ? (PDEV_BROADCAST_VOLUME)hdr : NULL;

            if (haveVolumeInfo)
                out.AppendFormat(L" %x %x", vlm->dbcv_unitmask, vlm->dbcv_flags);
        }

        out += L"\r\n";
        OutputMessage(hEditWnd, 0, (LPARAM)(PCTSTR)out);
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

namespace Ut
{
    void Main(CAppModule& module, CMessageLoop& loop)
    {
        NewDriveTest::Main(module.GetModuleInstance(), NULL, _T(""), SW_SHOWNORMAL);
    }
}
#endif
