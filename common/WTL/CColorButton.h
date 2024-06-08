#pragma once

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module  ColorButton.h - IO monitor main window |
//
// This module contains the definition of the io monitor window.
//
// Based on work by Chris Maunder, Alexander Bischofberger and James White.
// (See following original copyright statement)
//
// http://www.codetools.com/miscctrl/colorbutton.asp
// http://www.codetools.com/miscctrl/colour_picker.asp
//
// Copyright (c) 2000-2002 - Descartes Systems Sciences, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
// 2. Neither the name of Descartes Systems Sciences, Inc nor the names of 
//    its contributors may be used to endorse or promote products derived 
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --- ORIGINAL COPYRIGHT STATEMENT ---
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Updated 30 May 1998 to allow any number of colours, and to
//                     make the appearance closer to Office 97. 
//                     Also added "Default" text area.         (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColourPopup is a helper class for the colour picker control
// CColourPicker. Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
// @end
//
// $History: ColorButton.h $
//      
//      *****************  Version 3  *****************
//      User: Tim Smith    Date: 9/10/01    Time: 9:05a
//      Updated in $/Omni_V2/exe_cnf
//      
//      *****************  Version 2  *****************
//      User: Tim Smith    Date: 8/28/01    Time: 4:25p
//      Updated in $/Omni_V2/exe_cnf
//      Updated copyright dates.
//      
//      *****************  Version 1  *****************
//      User: Tim Smith    Date: 8/28/01    Time: 3:19p
//      Created in $/Omni_V2/exe_cnf
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Instructions on how to add CColorButton to you application:
//
//   1. CopyTo CColorButton.h and CColorButton.cpp into you application directory.
//
//   2. Add the two files into your project.
//
//   3. Many little features will not be enabled unless you have WINVER
//      and _WIN32_WINNT defined properly.  You can defined these values
//      in your stdafx.h.
//
//      _WIN32_WINNT >= 0x0501  - Theme support, XP flat menu support, XP drop 
//                                shadow support.
//
//      WINVER >= 0x0500        - Multi-monitor support, XP menu highlight 
//                                color support.
//
//      WINVER >= 0x0501        - XP menu highlight color support.
//
//      For MOST applications, both _WIN32_WINNT and WINVER should be 
//      defined to be 0x0501.
//
//   4. If you want XP theme support, add "#include <atltheme.h>" to your 
//      stdafx.h file.  This should be added after all the other atl includes.
//
//   5. CColorButton makes heavy use of helper types from ATL.  You will need
//      to make sure that "atltypes.h" and "atlgdi.h" are being included 
//      in stdafx.h.
//
//   6. Add a button to the dialog in question using the resource editor.
//      You don't have to make and style adjustments to the button.
//
//   7. Edit the class definition for the dialog box.  To the message map,
//      add "REFLECT_NOTIFICATIONS ()".
//
//   8. Add "#include "ColorButton.h"" prior to the definition of the dialog
//      box class.
//
//   9. Add a new member variable to the dialog.  The class will be 
//      "CColorButton" and give it any name you desire.  For this example,
//      we will call it "m_btnMyColor".
//
//  10. Inside your OnInitDialog for the dialog, add a line to subclass
//      the m_btnMyColor control.  It is important that is it subclassed
//      and not just assigned a window handle.
//
//      m_btnMyColor .SubclassWindow (GetDlgItem (IDC_MY_COLOR));
//
//  11. Compile and enjoy.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "IColor.h"
#include "IColorObserver.h"
#include <color.stuff.h>
#include <wcdafx.api.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atltheme.h>
#include <string>
#include <memory>

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Notification messages
//
//-----------------------------------------------------------------------------

static UINT constexpr    CPN_SELCHANGE{0x8000}; /* Colour Picker Selection change */
static UINT constexpr     CPN_DROPDOWN{0x8001}; /* Colour Picker drop down */
static UINT constexpr      CPN_CLOSEUP{0x8002}; /* Colour Picker close up */
static UINT constexpr     CPN_SELENDOK{0x8003}; /* Colour Picker end OK */
static UINT constexpr CPN_SELENDCANCEL{0x8004}; /* Colour Picker end (cancelled) */

struct NMCOLORBUTTON
{
    NMHDR hdr;
    BOOL fColorValid;
    COLORREF clr;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------
class CColorButton: private ATL::CWindowImpl<CColorButton>,
                    private WTL::CThemeImpl<CColorButton>,
                    private WTL::CBufferedPaintImpl<CColorButton>,
                    public  IColor,
                    public  IColorObserver
{
    // @access Types and enumerations
public:
    using String = std::basic_string<TCHAR>; // ATL::CString is great, but lacks some transactional behaviour

    DELETE_COPY_MOVE_OF(CColorButton);

    // @cmember General destructor
    ~CColorButton() override;

    // @cmember General constructor
    CColorButton();

    // @cmember Get the current color
    COLORREF GetColorRef() const override;

    // @cmember Get the current alpha
    int GetAlpha() const override;

    // @cmember Set the current color
    void SetColor(COLORREF clrCurrent, int nAlpha = 255) override;

    // @cmember Set the tracking color target
    void AddObserver(IColorObserver& rObserver) override;

    // @cmember Set the tracking color target
    void AddObservers(IColor& rColor) override;

    // @cmember Get the default color
    COLORREF GetDefaultColor() const;

    // @cmember Set the default color
    void SetDefaultColor(COLORREF clrDefault);

    // @cmember Set the custom text
    void SetCustomText(LPCTSTR pszText);

    // @cmember Set the custom text via a resource string
    void SetCustomText(UINT nID);

    // @cmember Set the default text
    void SetDefaultText(ATL::CString&& szText);

    // @cmember Set the default text
    void SetDefaultText(LPCTSTR pszText);

    // @cmember Set the default text via a resource string
    void SetDefaultText(UINT nID);

    // @cmember Get the tracking flag
    bool GetTrackSelection(void) const;

    // @cmember Set the tracking flag
    void SetTrackSelection(bool fTrack);

    // @cmember Set both strings from a resource
    void SetText(UINT nDefault, UINT nCustom);

    // @cmember Do we have custom text
    BOOL HasCustomText() const;

    // @cmember Do we have default text
    BOOL HasDefaultText() const;

    // @cmember Subclass the window
    BOOL SubclassWindow(HWND hWnd);

private:
    friend ATL::CWindowImpl<CColorButton>;
    friend WTL::CThemeImpl<CColorButton>;
    friend WTL::CBufferedPaintImpl<CColorButton>;

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;

    void _SetColor(COLORREF clrCurrent, int nAlpha);
    void OnColorUpdate(IColor const& clrSource) override;

    bool DrawThemeBkgnd(WTL::CDCHandle dc, CRect& rcDraw, UINT uState, bool fPopupActive, bool fMouseOver);

    void ButtonDraw(WTL::CDCHandle dc, CRect const& rc, UINT uState);
    UINT GetButtonState() const;
    void DoPaint(WTL::CDCHandle dc, RECT const& rc);

protected:
    // @cmember Handle mouse move
    LRESULT OnMouseMove(UINT nFlags, CPoint pt);

    // @cmember Handle mouse leave
    LRESULT OnMouseLeave();

    // @cmember Handle key down for picker
    LRESULT OnPickerKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) const;

    // @cmember Handle button up event for picker
    LRESULT OnPickerLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) const;

    // @cmember Handle mouse move for picker
    LRESULT OnPickerMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) const;

    // @cmember Handle paint for picker
    LRESULT DoPickerPaint(WTL::CDCHandle) const;

    // @cmember Handle palette query for picker
    LRESULT OnPickerQueryNewPalette();

    // @cmember Handle palette change for picker
    LRESULT OnPickerPaletteChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    // @access Notification handlers
public:
    // @cmember Handle on click
    LRESULT OnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
    struct CPickerImpl;

    // @cmember Send WM_NOTIFY to parent enabler
    bool m_bNotifyParent;

    // @cmember Default text
    String m_pszDefaultText;

    // @cmember Custom text
    String m_pszCustomText;

    // @cmember Current color
    COLORREF m_clrCurrent;

    // @cmember default color
    COLORREF m_clrDefault;

    // @cmember True if popup active override
    bool m_fPopupActive;

    // @cmember True if tracking selection
    bool m_fTrackSelection;

    // @cmember True if the mouse is over
    bool m_fMouseOver;

    // @cmember The contained picker control
    std::unique_ptr<CPickerImpl> m_pPicker;
};

inline COLORREF CColorButton::GetDefaultColor() const
{
    return m_clrDefault;
}

inline void CColorButton::SetDefaultColor(COLORREF clrDefault)
{
    m_clrDefault = clrDefault;
}

inline bool CColorButton::GetTrackSelection() const
{
    return m_fTrackSelection;
}

inline void CColorButton::SetTrackSelection(bool fTrack)
{
    m_fTrackSelection = fTrack;
}

inline void CColorButton::SetText(UINT nDefault, UINT nCustom)
{
    SetDefaultText(nDefault);
    SetCustomText(nCustom);
}

inline BOOL CColorButton::HasCustomText() const
{
    return !m_pszCustomText.empty();
}

inline BOOL CColorButton::HasDefaultText() const
{
    return !m_pszDefaultText.empty();
}
