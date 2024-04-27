#ifndef _CF_windows_wtl_tooltip_dialog_h__
#define _CF_windows_wtl_tooltip_dialog_h__

//
//		ToolTipDialog 
//
// Written by Alain Rist (ar@navpoch.com)
// Copyright (c) 2003 Alain Rist.
//
// This file is NOT a part of the Windows Template Library.
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the author written consent, and 
// providing that this notice and the author name is included. 
//
// Beware of bugs.
//

// Refactor by Michael Nikonov 09/02/2011.
// Adopted to Common Framework.

#include <atlctrls.h>

namespace CF
{
    template <class T, class TT = WTL::CToolTipCtrl>
    class ToolTipDialog
    {
    public:
        ~ToolTipDialog()
        {}

    protected:
#if (_WIN32_WINNT >= 0x0501)
        enum { TOOLINFOSIZE = TTTOOLINFOW_V1_SIZE };
#else
        enum { TOOLINFOSIZE = sizeof(TOOLINFO) };
#endif

        ToolTipDialog(UINT style = TTS_NOANIMATE | TTS_NOFADE, UINT flags = TTF_IDISHWND | TTF_SUBCLASS) 
            : toolTipsCtl_(NULL)
            , toolTipsStyle_(style)
            , toolFlags_(flags | TTF_SUBCLASS)
        {}

	    TT& GetTT() 
        {
            return toolTipsCtl_; 
        }

        void ToolTipsActivate(BOOL bActivate)
        {
            toolTipsCtl_.Activate( bActivate ); 
        }

        void ToolTipsSize(int nPixel)
        { 
            toolTipsCtl_.SetMaxTipWidth(nPixel); 
        }

        void ToolTipSetText(HWND hTool, PCTSTR text)
        {
            WTL::CToolInfo info(TTF_PARSELINKS, hTool, 0, NULL, (PTSTR)text);
            info.cbSize = TOOLINFOSIZE;
            ::SendMessage(toolTipsCtl_, TTM_UPDATETIPTEXT, 0, info);
        }

        void ToolTipSetText(UINT idTool, PCTSTR text)
        {
            ToolTipSetText(GetHWND(idTool), text); 
        }

        BOOL ToolTipAdd(HWND hTool)
        {
            return SetTool(hTool, (LPARAM)(T*)this); 
        }

        BOOL ToolTipAdd(UINT idTool)
        { 
            return ToolTipAdd(GetHWND(idTool)); 
        }

        void ToolTipRemove(HWND hTool)
        {
            toolTipsCtl_.DelTool(hTool); 
        }

        void ToolTipRemove(UINT idTool) 
        {
            ToolTipRemove(GetHWND(idTool)); 
        }

        BEGIN_MSG_MAP(ToolTipDialog)
            MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST,WM_MOUSELAST, OnMouse)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        END_MSG_MAP()

    private:
	    TT toolTipsCtl_;
	    UINT toolTipsStyle_;
	    UINT toolFlags_;

        HWND GetHWND(UINT idTool) const 
        {
            return ::GetDlgItem(*(T*)this, idTool); 
        }

	    void ToolTipsInit()
        {
		    T* pT= (T*)this;
		    ATLASSERT( ::IsWindow( *pT ));
		    toolTipsCtl_.Create(*pT, NULL, NULL, toolTipsStyle_ | WS_POPUP, WS_EX_TOOLWINDOW);
		    CToolInfo info( pT->toolFlags_, *pT , 0, 0, MAKEINTRESOURCE(pT->IDD) );
            info.cbSize = TOOLINFOSIZE;
		    toolTipsCtl_.AddTool( &info );
		    ::EnumChildWindows( *pT, SetTool, (LPARAM)pT );
		    ToolTipsSize( 0 );
		    ToolTipsActivate( TRUE );
		 }
    		
	    LRESULT OnInitDialog(UINT , WPARAM , LPARAM, BOOL& bHandled)
	    {
		    ToolTipsInit();
		    bHandled = FALSE;
		    return TRUE;
	    }

	    LRESULT OnMouse(UINT, WPARAM, LPARAM, BOOL& bHandled)
	    {
		    T* pT = (T*)this;
		    bHandled = FALSE;
		    if(toolTipsCtl_.IsWindow())
			    toolTipsCtl_.RelayEvent((LPMSG)pT->GetCurrentMessage());
		    return 0;
	    }
      
	    static BOOL CALLBACK SetTool(HWND hTool, LPARAM pDlg)
        {
            T* pT = (T*)pDlg;
		    int idTool = ::GetWindowLong(hTool, GWL_ID);
            BOOL rv = FALSE;
		    if (idTool != IDC_STATIC)
            {
                WTL::CToolInfo info(pT->toolFlags_, hTool, 0, 0, (LPTSTR)idTool);
                info.cbSize = TOOLINFOSIZE;
                rv = pT->toolTipsCtl_.AddTool(&info);
            }
            return TRUE;
        }

    private:
        ToolTipDialog(ToolTipDialog const&);
        ToolTipDialog& operator = (ToolTipDialog const&);
    };
} 

#endif // _CF_windows_wtl_tooltip_dialog_h__
