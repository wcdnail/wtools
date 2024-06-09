/*********************************************************************

   Copyright (C) 2002 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com

   --------

   This code is based, in part, on:
   "A WTL-based Font preview combo box", Ramon Smits
   http://www.codeproject.com/wtl/rsprevfontcmb.asp

**********************************************************************/

#pragma once

#include "CCustomCtrl.h"
#include <atltheme.h>
#include <map>

/////////////////////////////////////////////////////////////////////////////
// CFontCombo window

class CFontCombo: public  CCustomControl<CFontCombo, WTL::CComboBox>,
                  private WTL::CThemeImpl<CFontCombo>,
                  private WTL::CBufferedPaintImpl<CFontCombo>
{
public:
    DELETE_COPY_MOVE_OF(CFontCombo);
    DECLARE_WND_SUPERCLASS(_T("WCCF::CFontCombo"), WTL::CComboBox::GetWndClassName())

    using FontMap = std::map<ATL::CString, WTL::CFontHandle>;

    // choose how the name and sample are displayed
    enum PreviewStyle: int
    {
        NAME_ONLY = 0,      // font name, drawn in font
        NAME_GUI_FONT,      // font name, drawn in GUI font
        NAME_THEN_SAMPLE,   // font name in GUI font, then sample text in font
        SAMPLE_THEN_NAME,   // sample text in font, then font name in GUI font
        SAMPLE_ONLY         // sample text in font
    };

    WCDAFX_API ~CFontCombo() override;
    WCDAFX_API CFontCombo();

    WCDAFX_API HRESULT PreCreateWindow() override;
    WCDAFX_API bool SubclassWindow(HWND hWnd);

    int GetPreviewStyle() const;
    void SetPreviewStyle(PreviewStyle style, bool bReload);
    int GetFontHeight() const;
    void SetFontHeight(int newHeight, bool bReload);

private:
    friend WTL::CThemeImpl<CFontCombo>;
    friend WTL::CBufferedPaintImpl<CFontCombo>;

    static BOOL EnumFontProc(PLOGFONT pLogFont, PTEXTMETRIC, DWORD dwType, CFontCombo *pThis);

    // call this to load the font strings
    void LoadAllFonts();

    void AddFont(PLOGFONT pLogFont);
    void DeleteAllFonts();
    static void DeleteAllFonts(FontMap& mapFonts);

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
    bool ThemedInit();

    int OnCreate(LPCREATESTRUCT pCS);
    void DoPaint(WTL::CDCHandle dc, RECT& rc);
    void OnDrawItem(LPDRAWITEMSTRUCT pDI);
    void OnMeasureItem(LPMEASUREITEMSTRUCT pMI);
    LRESULT OnDropdown(WORD, WORD, HWND, BOOL&);

    bool      m_bSubclassed;
    // set the "sample" text string, defaults to "abcdeABCDE"
    ATL::CString m_csSample;
    // choose the sample color (only applies with NAME_THEN_SAMPLE and SAMPLE_THEN_NAME)
    COLORREF    m_clrSample;
    PreviewStyle    m_style;
    int       m_iFontHeight;
    int     m_iMaxNameWidth;
    int   m_iMaxSampleWidth;
    FontMap      m_mapFonts;
    FontMap       m_mapTemp; // temporary map
};

inline void CFontCombo::SetFontHeight(int newHeight, bool bReload)
{
    if (newHeight == m_iFontHeight) {
        return;
    }
    m_iFontHeight = newHeight;
    if (bReload) {
        LoadAllFonts();
    }
}

inline int CFontCombo::GetFontHeight() const
{
    return m_iFontHeight;
}

inline void CFontCombo::SetPreviewStyle(PreviewStyle style, bool bReload)
{
    if (style == m_style) {
        return;
    }
    m_style = style;
    if (bReload) {
        LoadAllFonts();
    }
}

inline int CFontCombo::GetPreviewStyle() const
{
    return m_style;
}
