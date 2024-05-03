#include "stdafx.h"
#include "luicDrawings.h"
#include "luicTheme.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "string.utils.error.code.h"

//
// https://stackoverflow.com/questions/14994012/how-draw-caption-in-alttab-switcher-when-paint-custom-captionframe
//

CIconHandle LoadShellIcon(ATL::CStringW const& entry, UINT flags = SHGFI_SMALLICON | SHGFI_ADDOVERLAYS, unsigned attrs = INVALID_FILE_ATTRIBUTES)
{
    if (INVALID_FILE_ATTRIBUTES == attrs) {
        attrs = GetFileAttributesW(entry.GetString());
    }
    if (INVALID_FILE_ATTRIBUTES == attrs) {
        const auto code = static_cast<HRESULT>(GetLastError());
        DH::ThreadPrintf(LTH_SHELL_ICON L" Can't get attrs for '%s' - %d '%s'\n",
            entry.GetString(), code, Str::ErrorCode<>::SystemMessage(code)
        );
        return {};
    }
    SHFILEINFOW info;
    ZeroMemory(&info, sizeof(info));
    const DWORD_PTR rv = SHGetFileInfoW(entry.GetString(), attrs, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | flags);
    if (0 == rv) {
        const auto code = static_cast<HRESULT>(GetLastError());
        DH::ThreadPrintf(LTH_SHELL_ICON L" Can't load icon for '%s' - %d '%s'\n",
            entry.GetString(), code, Str::ErrorCode<>::SystemMessage(code)
        );
        return {};
    }
    return CIconHandle(info.hIcon);
}


struct CDrawRoutine::StaticInit
{
    using DrawCaptionTempWFn = BOOL(WINAPI*)(HWND hWnd, HDC dc, const RECT* pRect, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags);
    using SetSysColorsTempFn = DWORD_PTR(WINAPI*)(const COLORREF* pPens, const HBRUSH* pBrushes, DWORD_PTR n);
    using  DrawMenuBarTempFn = int(WINAPI*)(HWND hWnd, HDC dc, RECT* pRect, HMENU hMenu, HFONT hFont);

    HMODULE                      USER32;
    DrawCaptionTempWFn DrawCaptionTempW;
    SetSysColorsTempFn SetSysColorsTemp;
    DrawMenuBarTempFn   DrawMenuBarTemp;

    CIcon   m_IconActiveWnd;
    CIcon m_IconInactiveWnd;

    static StaticInit& instance()
    {
        static StaticInit inst;
        return inst;
    }

private:
    template <typename T>
    static bool GetProcAddressEx(HMODULE hMod, T& routine, PCSTR routineName, PCSTR modAlias)
    {
        *reinterpret_cast<FARPROC*>(&routine) = GetProcAddress(hMod, routineName);
        if (!routine) {
            DH::ThreadPrintf("%14s| '%s' is NULL\n", modAlias, routineName);
        }
        return nullptr != routine;
    }

    #define GetProcAddressEX(Module, Routine) \
        GetProcAddressEx(Module, Routine, #Routine, #Module)

    void Init()
    {
        GetProcAddressEX(USER32, DrawCaptionTempW);
        GetProcAddressEX(USER32, SetSysColorsTemp);
        GetProcAddressEX(USER32, DrawMenuBarTemp);

        const ATL::CStringW shell32dll = L"%SYSTEMROOT%\\System32\\shell32.dll";
        HICON tempIcon[1024];
        ZeroMemory(tempIcon, sizeof(tempIcon));
        UINT extrNum = ExtractIconExW(shell32dll.GetString(), -1, tempIcon, nullptr, _countof(tempIcon));
        //m_IconActiveWnd = tempIcon;
    }

    ~StaticInit()
    {
        if (USER32) {
            FreeLibrary(USER32);
        }
    }

    StaticInit()
        :            USER32{ LoadLibraryW(L"USER32") }
        ,  DrawCaptionTempW{ nullptr }
        ,  SetSysColorsTemp{ nullptr }
        ,   DrawMenuBarTemp{ nullptr }
        ,   m_IconActiveWnd{}
        , m_IconInactiveWnd{}
    {
        Init();
    }
};

CDrawRoutine::~CDrawRoutine()
{
}

CDrawRoutine::CDrawRoutine(CTheme const& theme)
    : m_Theme{ theme }
{
    UNREFERENCED_PARAMETER(StaticInit::instance());
}

void CDrawRoutine::DrawBorder(CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush) const
{
    CRect     rc = rcParam;
    auto hPrevBr = dc.SelectBrush(hBrush); 
    auto  length = rc.bottom - rc.top;
    dc.PatBlt(rc.left, rc.top, borderWidth, length, PATCOPY);
    rc.left += borderWidth;
    rc.right -= borderWidth;
    dc.PatBlt(rc.right, rc.top, borderWidth, length, PATCOPY);
    length = rc.right - rc.left;
    dc.PatBlt(rc.left, rc.top, length, borderWidth, PATCOPY);
    rc.top += borderWidth;
    rc.bottom -= borderWidth;
    dc.PatBlt(rc.left, rc.bottom, length, borderWidth, PATCOPY);
    if (hPrevBr) {
        dc.SelectBrush(hPrevBr);
    }
}

LONG CDrawRoutine::DrawCaptionButtons(CDCHandle dc, CRect const& rcCaption, bool withMinMax, UINT uFlags) const
{
    static const int margin = 2;
    int         buttonWidth = m_Theme.GetNcMetrcs().iCaptionWidth;
    buttonWidth -= margin;

    int iColor;
#if WINVER >= WINVER_2K
    if (uFlags & DC_GRADIENT) {
        if (uFlags & DC_ACTIVE) {
            iColor = COLOR_GRADIENTACTIVECAPTION;
        }
        else {
            iColor = COLOR_GRADIENTINACTIVECAPTION;
        }
    }
    else
#endif
    {
        if (uFlags & DC_ACTIVE) {
            iColor = COLOR_ACTIVECAPTION;
        }
        else {
            iColor = COLOR_INACTIVECAPTION;
        }
    }
    dc.FillRect(rcCaption, m_Theme.GetBrush(iColor));

    CRect rc;
    rc.top = rcCaption.top + margin;
    rc.bottom = rcCaption.bottom - margin;

    rc.right = rcCaption.right - margin;
    rc.left = rc.right - buttonWidth;
    dc.DrawFrameControl(rc, DFC_CAPTION, DFCS_CAPTIONCLOSE);

    if (withMinMax) {
        rc.right = rc.left - margin;
        rc.left = rc.right - buttonWidth;
        dc.DrawFrameControl(rc, DFC_CAPTION, DFCS_CAPTIONMAX);

        rc.right = rc.left;
        rc.left = rc.right - buttonWidth;
        dc.DrawFrameControl(rc, DFC_CAPTION, DFCS_CAPTIONMIN);
    }
    return rc.left;
}

void CDrawRoutine::DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags) const
{
    CRect rcTmp = rcParam;
    int iColor1 = COLOR_INACTIVECAPTION;
    if (uFlags & DC_ACTIVE) {
        iColor1 = COLOR_ACTIVECAPTION;
    }
    if (hIcon) {
        int        iconSize = GetSystemMetrics(SM_CYSMICON);  /* Same as SM_CXSMICON */
        int totalIconMargin = rcTmp.bottom - rcTmp.top - iconSize;
        int      iconMargin = totalIconMargin / 2;
        rcTmp.right = rcTmp.left + iconSize + totalIconMargin;
        dc.FillRect(rcTmp, m_Theme.GetBrush(iColor1));
        if (DrawIconEx(dc, rcTmp.left + iconMargin + 1, rcTmp.top + iconMargin, hIcon, 0, 0, 0, nullptr, DI_NORMAL) != 0) {
            rcTmp.left = rcTmp.right;
        }
        rcTmp.right = rcParam.right;
    }

#if WINVER >= WINVER_2K
    if (uFlags & DC_GRADIENT) {
        GRADIENT_RECT gcap = { 0, 1 };
        TRIVERTEX  vert[2];
        COLORREF colors[2];
        colors[0] = m_Theme.GetColor(iColor1);
        if (uFlags & DC_ACTIVE) {
            colors[1] = m_Theme.GetColor(COLOR_GRADIENTACTIVECAPTION);
        }
        else {
            colors[1] = m_Theme.GetColor(COLOR_GRADIENTINACTIVECAPTION);
        }
        vert[0].x = rcTmp.left;
        vert[0].y = rcTmp.top;
        vert[0].Red = static_cast<COLOR16>(colors[0] << 8);
        vert[0].Green = static_cast<COLOR16>(colors[0] & 0xFF00);
        vert[0].Blue = static_cast<COLOR16>((colors[0] >> 8) & 0xFF00);
        vert[0].Alpha = 0;
        vert[1].x = rcTmp.right;
        vert[1].y = rcTmp.bottom;
        vert[1].Red = static_cast<COLOR16>(colors[1] << 8);
        vert[1].Green = static_cast<COLOR16>(colors[1] & 0xFF00);
        vert[1].Blue = static_cast<COLOR16>((colors[1] >> 8) & 0xFF00);
        vert[1].Alpha = 0;
#if defined(WINVER_IS_98)
        GradientFill(dc, vert, 2, &gcap, 1, GRADIENT_FILL_RECT_H);
#else
        GdiGradientFill(dc, vert, 2, &gcap, 1, GRADIENT_FILL_RECT_H);
#endif
    }
    else
#endif
    {
        dc.FillRect(rcTmp, m_Theme.GetBrush(iColor1));
    }

    HFONT prevFont = dc.SelectFont(hFont);
    if (uFlags & DC_ACTIVE) {
        SetTextColor(dc, m_Theme.GetColor(COLOR_CAPTIONTEXT));
    }
    else {
        SetTextColor(dc, m_Theme.GetColor(COLOR_INACTIVECAPTIONTEXT));
    }
    rcTmp.left += 2;
    dc.SetBkMode(TRANSPARENT);
    dc.DrawTextW(str, -1, rcTmp, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    if (prevFont) {
        dc.SelectFont(prevFont);
    }
}

void CDrawRoutine::DrawMenuText(CDCHandle hdc, PCWSTR text, CRect& rc, UINT format, int color) const
{
    SetTextColor(hdc, m_Theme.GetColor(color));
    DrawTextW(hdc, text, -1, rc, format);
}

void CDrawRoutine::DrawDisabledMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format) const
{
    OffsetRect(rc, 1, 1);
    DrawMenuText(dc, text, rc, format, COLOR_3DHILIGHT);
    OffsetRect(rc, -1, -1);
    DrawMenuText(dc, text, rc, format, COLOR_3DSHADOW);
}

void CDrawRoutine::DrawMenuBar(CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selectedItem) const
{
    int backColorIndex = COLOR_MENU;
#if WINVER >= WINVER_XP
    if (m_Theme.IsFlatMenus()) {
        backColorIndex = COLOR_MENUBAR;
    }
#endif
    dc.FillRect(rc, m_Theme.GetBrush(backColorIndex));

    if (!hMenu || !hFont) {
        return ;
    }

    int    spacing = 10;
    HFONT  prevFnt = dc.SelectFont(hFont);
    UINT txtFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;
    CRect   rcItem = rc;
    CRect   rcText;
    SIZE      size;
    int     margin;

#if WINVER >= WINVER_2K
    BOOL  bKbdCues = FALSE;
    if (SystemParametersInfoW(SPI_GETKEYBOARDCUES, 0, &bKbdCues, 0) && !bKbdCues) {
        txtFormat |= DT_HIDEPREFIX;
    }
#endif
    TEXTMETRIC tm;
    if (GetTextMetrics(dc, &tm)) {
        spacing = tm.tmAveCharWidth;
    }
    margin = spacing / 2;

    rcItem.bottom--;
    rcText.top = rcItem.top - 1;
    rcText.bottom = rcItem.bottom;

    int menuItemCount = GetMenuItemCount(hMenu);
    for (int iMenuItem = 0; iMenuItem < menuItemCount; iMenuItem++) {
        WCHAR text[32] = { 0 };
        if (!GetMenuStringW(hMenu, iMenuItem, text, _countof(text)-1, MF_BYPOSITION)) {
            continue;
        }
        if (!GetTextExtentPoint32W(dc, text, lstrlen(text), &size)) {
            continue;
        }
        rcItem.right = rcItem.left + size.cx + spacing;
        if (rcItem.right > (rc.right - margin)) {
            SelectObject(dc, prevFnt);
            break;
        }
        rcText.left = rcItem.left + margin;
        rcText.right = rcItem.right - margin;
        UINT state = GetMenuState(hMenu, iMenuItem, MF_BYPOSITION);
        if ((state & MF_GRAYED) || (state & MF_DISABLED)) {
            if ((m_Theme.GetColor(COLOR_MENU) == m_Theme.GetColor(COLOR_3DFACE))
#if WINVER >= WINVER_XP
                || m_Theme.IsFlatMenus()
#endif
                )
            {
                DrawDisabledMenuText(dc, text, rcText, txtFormat);
            }
            else {
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_GRAYTEXT);
            }
        }
        else if ((state & MF_HILITE) || (iMenuItem == selectedItem)) {
#if WINVER >= WINVER_2K
#if WINVER >= WINVER_XP
            if (m_Theme.IsFlatMenus()) {
                FrameRect(dc, &rcItem, m_Theme.GetBrush(COLOR_HIGHLIGHT));

                InflateRect(&rcItem, -1, -1);
                FillRect(dc, &rcItem, m_Theme.GetBrush(COLOR_MENUHILIGHT));
                InflateRect(&rcItem, 1, 1);

                DrawMenuText(dc, text, rcText, txtFormat, COLOR_HIGHLIGHTTEXT);
            }
            else
#endif
            {
                dc.DrawEdge(rcItem, BDR_SUNKENOUTER, BF_RECT);

                OffsetRect(&rcText, 1, 1);
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_MENUTEXT);
            }
#else  /* WINVER < WINVER_2K */
            FillRect(dc, rcItem, m_Theme.GetBrush(COLOR_HIGHLIGHT));
            DrawMenuText(dc, text, rcText, textFormat, COLOR_HIGHLIGHTTEXT);
#endif
        }
        else {
            DrawMenuText(dc, text, rcText, txtFormat, COLOR_MENUTEXT);
        }
        rcItem.left = rcItem.right;
    }

    SelectObject(dc, prevFnt);
}

void CDrawRoutine::DrawScrollbar(CDCHandle dc, CRect const& rcParam, BOOL enabled) const
{
    CRect               rc;
    int       buttonHeight = m_Theme.GetNcMetrcs().iScrollHeight;
    UINT frameControlFlags = (enabled ? 0 : DFCS_INACTIVE);
    HBRUSH     hbrScrollBk = m_Theme.GetBrush(COLOR_SCROLLBAR);

    rc.left = rcParam.left;
    rc.right = rcParam.right;

    if (rcParam.bottom - rcParam.top - buttonHeight * 2 <= 0) {
        buttonHeight = (rcParam.bottom - rcParam.top) / 2;
    }
    if (buttonHeight >= 5) {
        rc.top = rcParam.top;
        rc.bottom = rcParam.top + buttonHeight;
        dc.DrawFrameControl(rc, DFC_SCROLL, DFCS_SCROLLUP | frameControlFlags);

        rc.top = rcParam.bottom - buttonHeight;
        rc.bottom = rcParam.bottom;
        dc.DrawFrameControl(&rc, DFC_SCROLL, DFCS_SCROLLDOWN | frameControlFlags);
    }
    else {
        buttonHeight = 0;
    }

    /* Background */
    rc.top = rcParam.top + buttonHeight;
    rc.bottom = rcParam.bottom - buttonHeight;
    if (rc.top >= rc.bottom) {
        return ;
    }

    /* The configured scrollbar color is only used by the OS for the scrollbar
     * background IF it is the same as the first 3D light color, for some
     * obscure reason...
     * Except when it also matches the window color (to be able to distinguish
     * it)
     */
    if (hbrScrollBk 
    && (m_Theme.GetColor(COLOR_SCROLLBAR) != m_Theme.GetColor(COLOR_3DHILIGHT)
     || m_Theme.GetColor(COLOR_WINDOW) == m_Theme.GetColor(COLOR_SCROLLBAR))) {
        dc.SetTextColor(m_Theme.GetColor(COLOR_3DFACE));
        COLORREF previousColor = dc.SetBkColor(m_Theme.GetColor(COLOR_3DHILIGHT));
        if (previousColor == CLR_INVALID) {
            dc.FillRect(rc, m_Theme.GetBrush(COLOR_SCROLLBAR));
        }
        else {
            dc.FillRect(rc, hbrScrollBk);
            dc.SetBkColor(previousColor);
        }
    }
    else {
        dc.FillRect(rc, m_Theme.GetBrush(COLOR_SCROLLBAR));
    }
}

void CDrawRoutine::CalcRects(CRect const& rc, UINT captFlags, WindowRects& target) const
{
  //long        dpiScale = ScaleForDpi<long>(8);
    const bool isToolWnd = (0 != (DC_SMALLCAP & captFlags));
    const bool  isActive = (0 != (DC_ACTIVE & captFlags));
    LRect       rcBorder = FromCRect<long>(rc);
    LRect        rcFrame;
    LRect         rcCapt;
    LRect         rcMenu;
    LRect         rcWork;
    LRect       rcScroll;

    target.m_rcBorder = ToCRect(rcBorder);

    target.m_BorderSize = m_Theme.GetNcMetrcs().iBorderWidth + 1;
#if WINVER >= WINVER_VISTA
    target.m_BorderSize += m_Theme.GetNcMetrcs().iPaddedBorderWidth;
#endif

    rcFrame = rcBorder;
    rcFrame.Shrink(target.m_BorderSize, target.m_BorderSize);
    rcFrame.PutInto(rcBorder, PutAt::Center);
    target.m_rcFrame = ToCRect(rcFrame);
    rcWork = rcFrame;

    rcCapt = rcFrame;
    rcCapt.cy = m_Theme.GetNcMetrcs().iCaptionHeight + 2;
    rcWork.cy -= rcCapt.cy;
    rcCapt.Shrink(1, 1);
    target.m_rcCapt = ToCRect(rcCapt);

    rcMenu = rcCapt;
    if (!isToolWnd) {
        rcMenu.y  = rcCapt.Bottom() + 1;
        rcMenu.cy = m_Theme.GetNcMetrcs().iMenuHeight + 1;
        target.m_rcMenu = ToCRect(rcMenu);
        rcWork.cy -= rcCapt.cy;
    }

    if (!isToolWnd) {
        rcWork.Shrink(2, 2);
        rcWork.y = rcMenu.Bottom() + 1;
        target.m_rcWorkspace = ToCRect(rcWork);

        rcScroll = rcWork;
        rcScroll.Shrink(0, 2);
        rcScroll.cx = m_Theme.GetNcMetrcs().iScrollWidth;
        rcScroll.x = rcWork.Right() - rcScroll.cx - 2;
        target.m_rcScroll = ToCRect(rcScroll);
    }
}

void CDrawRoutine::DrawWindow(CDCHandle dc, DrawWindowArgs const& params) const
{
    WindowRects const&    rects = params.rects;
    HFONT              menuFont = m_Theme.GetFont(FONT_Menu);
    HFONT              captFont = m_Theme.GetFont(FONT_Caption);
    HICON              captIcon = nullptr;
    UINT              captFlags = params.captFlags | DC_TEXT;
    int     workspaceColorIndex = COLOR_APPWORKSPACE;
    int        borderColorIndex = COLOR_INACTIVEBORDER;
    const bool        isToolWnd = (0 != (DC_SMALLCAP & params.captFlags));
    const bool         isActive = (0 != (DC_ACTIVE & params.captFlags));

    if (!isToolWnd) {
        if (isActive) { captIcon = StaticInit::instance().m_IconActiveWnd; }
        else          { captIcon = StaticInit::instance().m_IconInactiveWnd; }
        if (captIcon) { captFlags |= DC_ICON; }
    }
    if (m_Theme.IsGradientCaptions()) {
        captFlags |= DC_GRADIENT;
    }
    if (isActive) {
        borderColorIndex    = COLOR_ACTIVEBORDER;
        workspaceColorIndex = COLOR_3DFACE;
    }

    if (isToolWnd) {
        dc.FillSolidRect(rects.m_rcFrame, m_Theme.GetColor(COLOR_MENU));
        CRect rcEdge(rects.m_rcFrame);
        dc.DrawEdge(rcEdge, EDGE_RAISED, BF_RECT /*| BF_ADJUST*/);
        rcEdge.InflateRect(1, 1);
        dc.DrawEdge(rcEdge, EDGE_RAISED, BF_RECT /*| BF_ADJUST*/);
    }
    else {
        DrawBorder(dc, rects.m_rcBorder, rects.m_BorderSize, m_Theme.GetBrush(borderColorIndex));
        dc.DrawEdge(CRect(rects.m_rcBorder), EDGE_RAISED, BF_RECT /*| BF_ADJUST*/);
        dc.FillSolidRect(rects.m_rcFrame, m_Theme.GetColor(COLOR_MENU));
    }

    CRect rcCapt = rects.m_rcCapt;
    rcCapt.right = DrawCaptionButtons(dc, rcCapt, !isToolWnd, captFlags);
    DrawCaption(dc, rcCapt, captFont, captIcon, params.caption, captFlags);

    if (!isToolWnd) {
        if (params.hMenu) {
            DrawMenuBar(dc, rects.m_rcMenu, params.hMenu, menuFont, params.selectedMenu);
        }

        CRect rcWork = rects.m_rcWorkspace;
        dc.FillSolidRect(rcWork, m_Theme.GetColor(workspaceColorIndex));
        dc.DrawEdge(rcWork, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

        if (params.text.lineCount > 0) {
            int textColorIndex = isActive ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT;
            HFONT      prevFnt = dc.SelectFont(m_Theme.GetFont(FONT_Desktop));
            COLORREF   prevClr = dc.SetTextColor(m_Theme.GetColor(textColorIndex));
            COLORREF prevBkClr = dc.SetBkColor(m_Theme.GetColor(workspaceColorIndex));
            int     prevBkMode = dc.SetBkMode(TRANSPARENT);
            CRect       rcText = rcWork;
            rcText.right = rects.m_rcScroll.left - 1;
            rcText.DeflateRect(rcWork.Width() / 16, 4);
            CRect  rcLine = rcText;
            LONG       cy = -(m_Theme.GetLogFont(FONT_Desktop)->lfHeight) + 2;
            rcLine.bottom = rcLine.top + cy;

            for (int i = 0; i < params.text.lineCount; i++) {
                if (rcLine.top > rcText.bottom) {
                    break;
                }
                const auto& line = params.text.line[i];
                if (line.selected) {
                    CRect rcSel = rcLine;
                    rcSel.top += 1;
                    rcSel.InflateRect(3, 2);
                    dc.FillSolidRect(rcSel, m_Theme.GetColor(COLOR_HIGHLIGHT));
                    dc.SetTextColor(m_Theme.GetColor(COLOR_HIGHLIGHTTEXT));
                }
                else {
                    dc.SetTextColor(m_Theme.GetColor(textColorIndex));
                }
                dc.DrawTextW(line.text.GetString(), line.text.GetLength(), rcLine, params.text.flags);
                rcLine.top = rcLine.bottom;
                rcLine.bottom = rcLine.top + cy;
            }

            dc.SetBkMode(prevBkMode);
            dc.SetTextColor(prevBkClr);
            dc.SetTextColor(prevClr);
            dc.SelectFont(prevFnt);
        }

        DrawScrollbar(dc, rects.m_rcScroll, isActive);
    }
}

