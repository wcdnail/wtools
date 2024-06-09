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

#include "stdafx.h"
#include "CFontCombo.h"
#include <string.utils.error.code.h>
#include <atlcrack.h>

#ifdef _DEBUG
#  include <crtdbg.h>
#  define new DEBUG_NEW
#  define _NEED_WMESSAGE_DUMP 1
#  include <debug.dump.msg.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static constexpr int     SPACING{10};
static constexpr int GLYPH_WIDTH{15};

/*
void WINAPI 
DDX_FontPreviewCombo (CDataExchange* pDX, int nIDC, CString& faceName)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    _ASSERTE (hWndCtrl != nullptr);                
    
    CFontCombo* ctrl = 
        (CFontCombo*) CWnd::FromHandle(hWndCtrl);
 
    if (pDX->m_bSaveAndValidate) //data FROM control
    {
        //no validation needed when coming FROM control
        int pos = ctrl->GetCurSel();
        if (pos != CB_ERR)
        {
            ctrl->GetLBText(pos, faceName);
        }
        else
            faceName = "";
    }
    else //data TO control
    {
        //need to make sure this fontname is one we have
        //but if it's not we can't use the Fail() DDX mechanism since we're
        //not in save-and-validate mode.  So instead we just set the 
        //selection to the first item, which is the default anyway, if the
        //facename isn't in the box.

        int pos = ctrl->FindString (-1, faceName);
        if (pos != CB_ERR)
        {
            ctrl->SetCurSel (pos);
        }
        else
            ctrl->SetCurSel (0);
    }
}
*/

/////////////////////////////////////////////////////////////////////////////
// CFontCombo

CFontCombo::~CFontCombo()
{
    DeleteAllFonts();
}

CFontCombo::CFontCombo()
    :     m_bSubclassed{false}
    ,        m_csSample{_T("abcdeABCDE")}
    ,       m_clrSample{GetSysColor(COLOR_WINDOWTEXT)}
    ,           m_style{NAME_THEN_SAMPLE}
    ,     m_iFontHeight{12}
    ,   m_iMaxNameWidth{0}
    , m_iMaxSampleWidth{0}
    ,        m_mapFonts{}
{
}

HRESULT CFontCombo::PreCreateWindow()
{
    static ATOM gs_CFontCombo_Atom{0};
    return CCustomControl::PreCreateWindowImpl(gs_CFontCombo_Atom, GetWndClassInfo());
}

bool CFontCombo::SubclassWindow(HWND hWnd)
{
    if (!WndSuper::SubclassWindow(hWnd)) {
        return false;
    }
    ThemedInit();
    //ModifyStyle(0, CBS_OWNERDRAWFIXED);
    m_bSubclassed = true;
    return true;
}

BOOL CFontCombo::EnumFontProc(PLOGFONT pLogFont, PTEXTMETRIC, DWORD dwType, CFontCombo *pThis)
{
    int const nItem{pThis->AddString(pLogFont->lfFaceName)};
    ATLASSERT(nItem!=-1);
    int const  nRes{pThis->SetItemData(nItem, dwType)};
    ATLASSERT(nRes!=-1);
    pThis->AddFont(pLogFont);
    return TRUE;
}

void CFontCombo::DeleteAllFonts()
{
    DeleteAllFonts(m_mapFonts);
    DeleteAllFonts(m_mapTemp);
}

void CFontCombo::DeleteAllFonts(FontMap& mapFonts)
{
    for (auto const& cf : mapFonts | std::views::values) {
        DeleteObject(cf.m_hFont);
    }
    FontMap{}.swap(mapFonts);
}

void CFontCombo::LoadAllFonts()
{
    ResetContent();
    DeleteAllFonts();
    WTL::CClientDC const dc{*this};
    EnumFonts(dc, nullptr, reinterpret_cast<FONTENUMPROC>(EnumFontProc), reinterpret_cast<LPARAM>(this));
    m_mapTemp.swap(m_mapFonts);
    SetCurSel(0);
}

/////////////////////////////////////////////////////////////////////////////
// CFontCombo message handlers

void CFontCombo::OnDrawItem(LPDRAWITEMSTRUCT pDI) 
{
    ATLASSERT(pDI->CtlType == ODT_COMBOBOX); 
    CRect          rc{pDI->rcItem};
    WTL::CDCHandle dc{pDI->hDC};
    if (pDI->itemState & ODS_FOCUS) {
        dc.DrawFocusRect(&rc);
    }
    if (pDI->itemID == -1) {
        return;
    }
    int const nIndexDC{dc.SaveDC()};
    WTL::CBrush     br{};
    COLORREF clrSample{m_clrSample};

    if (pDI->itemState & ODS_SELECTED) {
        br.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
        dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
        clrSample = GetSysColor(COLOR_HIGHLIGHTTEXT);
    }
    else {
        br.CreateSolidBrush(dc.GetBkColor());
    }
    
    dc.SetBkMode(TRANSPARENT);
    dc.FillRect(rc, br);
    
    // which one are we working on?
    ATL::CString csCurFontName{};
    GetLBText(pDI->itemID, csCurFontName);

    // draw the cute TTF glyph
    //DWORD_PTR const dwData{GetItemData(pDI->itemID)};
    //if (dwData & TRUETYPE_FONTTYPE) {
    //    //m_img.Draw(&dc, 0, CPoint(rc.left+5, rc.top+4),ILD_TRANSPARENT);
    //}
    //rc.left += GLYPH_WIDTH;
    
    int constexpr     iOffsetX{SPACING};
    CSize                   sz{};
    int                  iPosY{0};
    HFONT                   hf{nullptr};
    WTL::CFontHandle const& cf{m_mapFonts[csCurFontName]};
    ATLASSERT(cf.m_hFont != nullptr);

    // draw the text
    switch (m_style) {
    case NAME_GUI_FONT: {
        // font name in GUI font
        dc.GetTextExtent(csCurFontName.GetString(), csCurFontName.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        dc.TextOut(rc.left+iOffsetX, rc.top + iPosY, csCurFontName);
        break;
    }
    case NAME_ONLY: {
        // font name in current font
        hf = dc.SelectFont(cf);
        dc.GetTextExtent(csCurFontName.GetString(), csCurFontName.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        dc.TextOut(rc.left+iOffsetX, rc.top + iPosY,csCurFontName);
        dc.SelectFont(hf);
        break;
    }
    case NAME_THEN_SAMPLE: {
        // font name in GUI font
        dc.GetTextExtent(csCurFontName.GetString(), csCurFontName.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        dc.TextOut(rc.left+iOffsetX, rc.top + iPosY, csCurFontName);
        // condense, for edit
        int iSep{m_iMaxNameWidth};
        if ((pDI->itemState & ODS_COMBOBOXEDIT) == ODS_COMBOBOXEDIT) {
            iSep = sz.cx;
        }
        // sample in current font
        hf = dc.SelectFont(cf);
        dc.GetTextExtent(m_csSample.GetString(), m_csSample.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        COLORREF const clr{dc.SetTextColor(clrSample)};
        dc.TextOut(rc.left + iOffsetX + iSep + iOffsetX, rc.top + iPosY, m_csSample);
        dc.SetTextColor(clr);
        dc.SelectFont(hf);
        break;
    }
    case SAMPLE_THEN_NAME: {
        // sample in current font
        hf = dc.SelectFont(cf);
        dc.GetTextExtent(m_csSample.GetString(), m_csSample.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        COLORREF const clr{dc.SetTextColor(clrSample)};
        dc.TextOut(rc.left+iOffsetX, rc.top + iPosY, m_csSample);
        dc.SetTextColor(clr);
        dc.SelectFont(hf);
        // condense, for edit
        int iSep{m_iMaxNameWidth};
        if ((pDI->itemState & ODS_COMBOBOXEDIT) == ODS_COMBOBOXEDIT) {
            iSep = sz.cx;
        }
        // font name in GUI font
        dc.GetTextExtent(csCurFontName.GetString(), csCurFontName.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        dc.TextOut(rc.left + iOffsetX + iSep + iOffsetX, rc.top + iPosY, csCurFontName);
        break;
    }
    case SAMPLE_ONLY: {
        // sample in current font
        hf = dc.SelectFont(cf);
        dc.GetTextExtent(m_csSample.GetString(), m_csSample.GetLength(), &sz);
        iPosY = (rc.Height() - sz.cy) / 2;
        dc.TextOut(rc.left+iOffsetX, rc.top + iPosY, m_csSample);
        dc.SelectFont(hf);
        break;
    }
    }
    dc.RestoreDC(nIndexDC);
}

/////////////////////////////////////////////////////////////////////////////

void CFontCombo::OnMeasureItem(LPMEASUREITEMSTRUCT pMI) 
{
    // ok, how big is this ?
    ATL::CString csFontName{};
    GetLBText(pMI->itemID, csFontName);
    //if (!cf.CreateFont(m_iFontHeight, 0, 0, 0, FW_NORMAL, 
    //    FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    //    ANTIALIASED_QUALITY, DEFAULT_PITCH, csFontName)) {
    //    ATLASSERT(0);
    //    return;
    //}
    WTL::CFontHandle const& cf{m_mapFonts[csFontName]};
    ATLASSERT(cf.m_hFont != nullptr);
    LOGFONT lf{0};
    cf.GetLogFont(&lf);
    if ((m_style == NAME_ONLY) || (m_style == SAMPLE_ONLY) || (m_style == NAME_GUI_FONT)) {
        m_iMaxNameWidth = 0;
        m_iMaxSampleWidth = 0;
    }
    else {
        WTL::CClientDC dc{*this};
        // measure font name in GUI font
        HFONT const hFont{GetFont()}; // static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT))};
        HFONT          hf{dc.SelectFont(hFont)};
        CSize          sz{};
        dc.GetTextExtent(csFontName.GetString(), csFontName.GetLength(), &sz);
        m_iMaxNameWidth = max(m_iMaxNameWidth, sz.cx);
        dc.SelectFont(hf);
        // measure sample in cur font
        hf = dc.SelectFont(cf);
        if (hf) {
            dc.GetTextExtent(m_csSample.GetString(), m_csSample.GetLength(), &sz);
            m_iMaxSampleWidth = max(m_iMaxSampleWidth, sz.cx);
            dc.SelectFont(hf);
        }
    }
    pMI->itemHeight = lf.lfHeight + 4;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CFontCombo::OnDropdown(WORD, WORD, HWND, BOOL&) 
{
    int const nScrollWidth{GetSystemMetrics(SM_CXVSCROLL)};
    int             nWidth{nScrollWidth};
    //nWidth += GLYPH_WIDTH;
    switch (m_style) {
    case NAME_GUI_FONT:
        nWidth += m_iMaxNameWidth;
        break;
    case NAME_ONLY:
        nWidth += m_iMaxNameWidth;
        break;
    case NAME_THEN_SAMPLE:
        nWidth += m_iMaxNameWidth;
        nWidth += m_iMaxSampleWidth;
        nWidth += SPACING * 2;
        break;
    case SAMPLE_THEN_NAME:
        nWidth += m_iMaxNameWidth;
        nWidth += m_iMaxSampleWidth;
        nWidth += SPACING * 2;
        break;
    case SAMPLE_ONLY:
        nWidth += m_iMaxSampleWidth;
        break;
    }
    SetDroppedWidth(nWidth);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CFontCombo::AddFont(PLOGFONT pLogFont)
{
    if (m_style == NAME_GUI_FONT) {
        return ;
    }
    WTL::CFontHandle cf{};
    HFONT const hFontResult{
        cf.CreateFont(m_iFontHeight, 0, 0, 0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH, 
            pLogFont->lfFaceName
    )};
    _ASSERTE(hFontResult);
    m_mapTemp[pLogFont->lfFaceName] = cf;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CFontCombo::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled = TRUE;
    DBG_DUMP_WMESSAGE_EXT(0, L"FNTCOMBO", hWnd, uMsg, wParam, lParam);
    UNREFERENCED_PARAMETER(bHandled);
    switch (dwMsgMapID) {
    case 0:
        MSG_WM_CREATE(OnCreate)
        switch (uMsg) {
        case OCM_DRAWITEM:
        case WM_DRAWITEM: {
            auto*        pDI{reinterpret_cast<LPDRAWITEMSTRUCT>(lParam)};
            CRect       rect{pDI->rcItem};
            HDC const dcTemp{pDI->hDC};
            if(IsBufferedPaintSupported()) {
                m_BufferedPaint.Begin(dcTemp, rect, m_dwFormat, &m_PaintParams, &pDI->hDC);
            }
            OnDrawItem(pDI);
            if(IsBufferedPaintSupported()) {
                m_BufferedPaint.End();
            }
            break;
        }
        case OCM_MEASUREITEM:
        case WM_MEASUREITEM:
            OnMeasureItem(reinterpret_cast<LPMEASUREITEMSTRUCT>(lParam));
            break;
        }
        REFLECTED_COMMAND_CODE_HANDLER(CBN_DROPDOWN, OnDropdown)
        DEFAULT_REFLECTION_HANDLER()
        if (!m_bSubclassed) {
            CHAIN_MSG_MAP(WTL::CBufferedPaintImpl<CFontCombo>)
        }
        CHAIN_MSG_MAP(CThemeImpl<CFontCombo>)
        break;
    default:
        ATLTRACE(static_cast<int>(ATL::atlTraceWindowing), 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

bool CFontCombo::ThemedInit()
{
    PCWSTR const pszClasses{
        VSCLASS_COMBOBOX L";"
        VSCLASS_LISTBOX L";"
        VSCLASS_BUTTON
    };
    if (!OpenThemeData(pszClasses)) {
        auto const hCode{static_cast<HRESULT>(GetLastError())};
        DH::TPrintf(TL_Warning, L"OpenThemeData['%s'] failed: 0x%08x %s\n", pszClasses,
            hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
        return false;
    }
    return true;
}

int CFontCombo::OnCreate(LPCREATESTRUCT pCS)
{
    UNREFERENCED_PARAMETER(pCS);
    ThemedInit();
    ModifyStyle(0xffffffff, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE);

    //COMBOBOXINFO cbInfo{0};
    //cbInfo.cbSize = sizeof(COMBOBOXINFO);
    //GetComboBoxInfo(&cbInfo);
    //ATL::CWindow{cbInfo.hwndList}.ModifyStyle(0, LBS_OWNERDRAWVARIABLE);

    LoadAllFonts();
    DH::TPrintf(0, _T("%s OK for %p\n"), __FUNCTIONW__, this);
    return 0;
}

void CFontCombo::DoPaint(WTL::CDCHandle dc, RECT& rc)
{
    HRESULT const hCode{DrawThemeBackground(dc, CP_BACKGROUND, CBTBS_NORMAL, &rc, nullptr)};
    if (FAILED(hCode)) {
        DH::TPrintf(TL_Warning, L"DrawThemeBackground failed: 0x%08x %s\n",
            hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
    }
}
