#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "dh.tracing.h"
#include "rect.alloc.h"

// https://stackoverflow.com/questions/14994012/how-draw-caption-in-alttab-switcher-when-paint-custom-captionframe

class DrawRoutines
{
    //HMODULE USER32;

public:
    //using DrawCaptionTempWFn = BOOL(WINAPI*)(HWND hWnd, HDC dc, const RECT* pRect, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags);
    //using SetSysColorsTempFn = DWORD_PTR(WINAPI*)(const COLORREF* pPens, const HBRUSH* pBrushes, DWORD_PTR n);
    //using  DrawMenuBarTempFn = int(WINAPI*)(HWND hWnd, HDC dc, RECT* pRect, HMENU hMenu, HFONT hFont);
    //
    //DrawCaptionTempWFn DrawCaptionTempW;
    //SetSysColorsTempFn SetSysColorsTemp;
    //DrawMenuBarTempFn   DrawMenuBarTemp;

    static DrawRoutines& instance();

    static void DrawBorder(CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush);
    static LONG DrawCaptionButtons(CDCHandle dc, CRect const& rcCaption, bool withMinMax, int buttonWidth, UINT uFlags, CTheme const& theme);
    static void DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags, CTheme const& theme);
    static void DrawMenuText(CDCHandle hdc, PCWSTR text, CRect& rc, UINT format, int color, CTheme const& theme);
    static void DrawDisabledMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format, CTheme const& theme);
    static void DrawMenuBar(CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selectedItem, CTheme const& theme);
    static void DrawWindow(CDCHandle dc, const CWndFrameRects& rects, UINT flags, HMENU hMenu, int selectedMenu, CTheme const& theme);

private:
    ~DrawRoutines();
    DrawRoutines();

    void Init();
};

DrawRoutines& DrawRoutines::instance()
{
    static DrawRoutines inst;
    return inst;
}

DrawRoutines::~DrawRoutines()
{
    //if (USER32) {
    //    FreeLibrary(USER32);
    //}
}

DrawRoutines::DrawRoutines()
    //:           USER32{ LoadLibraryW(L"USER32") }
    //, DrawCaptionTempW{ nullptr }
    //, SetSysColorsTemp{ nullptr }
    //,  DrawMenuBarTemp{ nullptr }

{
    Init();
}

template <typename T>
static inline bool GetProcAddressEx(HMODULE hMod, T& routine, PCSTR routineName, PCSTR modAlias)
{
    *reinterpret_cast<FARPROC*>(&routine) = GetProcAddress(hMod, routineName);
    if (!routine) {
        DH::ThreadPrintf("%14s| '%s' is NULL\n", modAlias, routineName);
    }
    return nullptr != routine;
}

#define GetProcAddressEX(Module, Routine) \
    GetProcAddressEx(Module, Routine, #Routine, #Module)

void DrawRoutines::Init()
{
    //GetProcAddressEX(USER32, DrawCaptionTempW);
    //GetProcAddressEX(USER32, SetSysColorsTemp);
    //GetProcAddressEX(USER32, DrawMenuBarTemp);
}

void DrawRoutines::DrawBorder(CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush)
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

LONG DrawRoutines::DrawCaptionButtons(CDCHandle dc, CRect const& rcCaption, bool withMinMax, int buttonWidth,
                                      UINT uFlags, CTheme const& theme)
{
    static const int margin = 2;
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
    dc.FillRect(rcCaption, theme.GetBrush(iColor));

    CRect rc;
    rc.top = rcCaption.top + margin;
    rc.bottom = rcCaption.bottom - margin;

    /* Close */
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

void DrawRoutines::DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags, CTheme const& theme)
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
        dc.FillRect(rcTmp, theme.GetBrush(iColor1));
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
        colors[0] = theme.GetColor(iColor1);
        if (uFlags & DC_ACTIVE) {
            colors[1] = theme.GetColor(COLOR_GRADIENTACTIVECAPTION);
        }
        else {
            colors[1] = theme.GetColor(COLOR_GRADIENTINACTIVECAPTION);
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
        dc.FillRect(rcTmp, theme.GetBrush(iColor1));
    }

    HFONT prevFont = dc.SelectFont(hFont);
    if (uFlags & DC_ACTIVE) {
        SetTextColor(dc, theme.GetColor(COLOR_CAPTIONTEXT));
    }
    else {
        SetTextColor(dc, theme.GetColor(COLOR_INACTIVECAPTIONTEXT));
    }
    rcTmp.left += 2;
    dc.SetBkMode(TRANSPARENT);
    dc.DrawTextW(str, -1, rcTmp, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    if (prevFont) {
        dc.SelectFont(prevFont);
    }
}

void DrawRoutines::DrawMenuText(CDCHandle hdc, PCWSTR text, CRect& rc, UINT format, int color, CTheme const& theme)
{
    SetTextColor(hdc, theme.GetColor(color));
    DrawTextW(hdc, text, -1, rc, format);
}

void DrawRoutines::DrawDisabledMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format, CTheme const& theme)
{
    OffsetRect(rc, 1, 1);
    DrawMenuText(dc, text, rc, format, COLOR_3DHILIGHT, theme);
    OffsetRect(rc, -1, -1);
    DrawMenuText(dc, text, rc, format, COLOR_3DSHADOW, theme);
}

void DrawRoutines::DrawMenuBar(CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selectedItem, CTheme const& theme)
{
    int backColorIndex = COLOR_MENU;
#if WINVER >= WINVER_XP
    if (theme.IsFlatMenus()) {
        backColorIndex = COLOR_MENUBAR;
    }
#endif
    dc.FillRect(rc, theme.GetBrush(backColorIndex));

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
            if ((theme.GetColor(COLOR_MENU) == theme.GetColor(COLOR_3DFACE))
#if WINVER >= WINVER_XP
                || theme.IsFlatMenus()
#endif
                )
            {
                DrawDisabledMenuText(dc, text, rcText, txtFormat, theme);
            }
            else {
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_GRAYTEXT, theme);
            }
        }
        else if ((state & MF_HILITE) || (iMenuItem == selectedItem)) {
#if WINVER >= WINVER_2K
#if WINVER >= WINVER_XP
            if (theme.IsFlatMenus()) {
                FrameRect(dc, &rcItem, theme.GetBrush(COLOR_HIGHLIGHT));

                InflateRect(&rcItem, -1, -1);
                FillRect(dc, &rcItem, theme.GetBrush(COLOR_MENUHILIGHT));
                InflateRect(&rcItem, 1, 1);

                DrawMenuText(dc, text, rcText, txtFormat, COLOR_HIGHLIGHTTEXT, theme);
            }
            else
#endif
            {
                dc.DrawEdge(rcItem, BDR_SUNKENOUTER, BF_RECT);

                OffsetRect(&rcText, 1, 1);
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_MENUTEXT, theme);
            }
#else  /* WINVER < WINVER_2K */
            FillRect(dc, rcItem, theme.GetBrush(COLOR_HIGHLIGHT));
            DrawMenuText(dc, text, rcText, textFormat, COLOR_HIGHLIGHTTEXT, theme);
#endif
        }
        else {
            DrawMenuText(dc, text, rcText, txtFormat, COLOR_MENUTEXT, theme);
        }
        rcItem.left = rcItem.right;
    }

    SelectObject(dc, prevFnt);
}


void DrawRoutines::DrawWindow(CDCHandle dc, const CWndFrameRects& rects, UINT flags, HMENU hMenu, int selectedMenu, CTheme const& theme)
{
    HFONT  menuFont = theme.GetFont(FONT_Menu);
    HFONT  captFont = theme.GetFont(FONT_Caption);
    HICON  captIcon = nullptr;
    PCWSTR captText = L"Tool window...";
    UINT  captFlags = flags | DC_TEXT;

    if (0 == (DC_SMALLCAP & flags)) {
        if (0 != (DC_ACTIVE & flags)) {
            captIcon = CLegacyUIConfigurator::App()->GetIcon(IconStartmenu);
            captText = L"Active window...";
        }
        else {
            captIcon = CLegacyUIConfigurator::App()->GetIcon(IconAppearance);
            captText = L"Inactive window...";
        }
        captFlags |= DC_ICON;
    }

#if WINVER >= WINVER_2K
    if (theme.IsGradientCaptions()) {
        captFlags |= DC_GRADIENT;
    }
#endif
    int workspaceColorIndex = COLOR_APPWORKSPACE;
    int    borderColorIndex = COLOR_INACTIVEBORDER;
    if (0 != (DC_ACTIVE & flags)) {
        borderColorIndex    = COLOR_ACTIVEBORDER;
        workspaceColorIndex = COLOR_3DFACE;
    }

    DrawBorder(dc, rects.m_rcBorder, rects.m_BorderSize, theme.GetBrush(borderColorIndex));
    dc.DrawEdge(CRect(rects.m_rcBorder), EDGE_RAISED, BF_RECT | BF_ADJUST);
    dc.FillSolidRect(rects.m_rcFrame, theme.GetColor(COLOR_MENU));

    CRect rcCapt = rects.m_rcCapt;
    rcCapt.right = DrawCaptionButtons(dc, rcCapt, (0 == (DC_SMALLCAP & flags)), theme.GetNcMetrcs().iCaptionWidth, captFlags, theme);
    DrawCaption(dc, rcCapt, captFont, captIcon, captText, captFlags, theme);

    if (hMenu) {
        DrawMenuBar(dc, rects.m_rcMenu, hMenu, menuFont, selectedMenu, theme);
    }
    if (0 == (DC_SMALLCAP & flags)) {
        CRect rcWork = rects.m_rcWorkspace;
        rcWork.DeflateRect(1, 1);

        dc.FillSolidRect(rcWork, theme.GetColor(workspaceColorIndex));
        dc.DrawEdge(rcWork, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
    }
}

///----------------------------------------------------------------------------

void CWndFrameRects::Calc(CRect const& rc, CTheme const& theme, bool wMenu, bool wWorkspace)
{
    long   dpiScale = ScaleForDpi<long>(8);
    LRect  rcBorder = FromCRect<long>(rc);
    LRect   rcFrame;
    LRect    rcCapt;
    LRect    rcMenu;
    LRect    rcWork;

    m_rcBorder = ToCRect(rcBorder);

    m_BorderSize = theme.GetNcMetrcs().iBorderWidth + 1;
#if WINVER >= WINVER_VISTA
    m_BorderSize += theme.GetNcMetrcs().iPaddedBorderWidth;
#endif

    rcFrame = rcBorder;
    rcFrame.Shrink(m_BorderSize, m_BorderSize);
    rcFrame.PutInto(rcBorder, PutAt::Center);
    m_rcFrame = ToCRect(rcFrame);
    rcWork = rcFrame;

    rcCapt = rcFrame;
    rcCapt.cy = theme.GetNcMetrcs().iCaptionHeight + 2;
    rcWork.cy -= rcCapt.cy;
    rcCapt.Shrink(1, 1);
    m_rcCapt = ToCRect(rcCapt);

    if (wMenu) {
        rcMenu = rcCapt;
        rcMenu.y  = rcCapt.Bottom() + 1;
        rcMenu.cy = theme.GetNcMetrcs().iMenuHeight + 1;
        m_rcMenu = ToCRect(rcMenu);
        rcWork.cy -= rcCapt.cy;
    }

    if (wWorkspace) {
        rcWork.y = rcMenu.Bottom() + 1;
        m_rcWorkspace = ToCRect(rcWork);
    }
}

CThemePreviewer::~CThemePreviewer()
{
}

CThemePreviewer::CThemePreviewer()
{
    auto& classInfo = GetWndClassInfo();
    ATOM atom = classInfo.Register(&this->m_pfnSuperWindowProc);
    if (!atom) {
        HRESULT code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Registering '%s' failure!", classInfo.m_wc.lpszClassName), code, true, MB_ICONERROR);
    }

    UNREFERENCED_PARAMETER(DrawRoutines::instance());
}

void CThemePreviewer::OnPaint(CDCHandle dcParam)
{
    UNREFERENCED_PARAMETER(dcParam);

    const auto& theme = CLegacyUIConfigurator::App()->CurrentTheme();

    CPaintDC dcPaint(m_hWnd);
    CDCHandle dc(dcPaint.m_hDC);
    // ##TODO: later via CreateCompatibleDC

    CRect rcClient;
    GetClientRect(rcClient);

    const double sx = 5.;
    const double sy = 5.;

    DRect rcFull = FromCRect<double>(rcClient);
    rcFull.Shrink(sx, sy);

    CF::RectzAllocator<double> rcAlloc{ rcFull };
    DRect rcIcon1 = rcAlloc.Next(rcFull.Width() / 3., rcFull.Height() / 3., sx, sy);
    DRect  rcWin1 = rcAlloc.NextFromRight(rcFull.Width(), rcFull.Height() / 1.2, sx, sy);
    rcAlloc.Shift(0, -(rcWin1.Height() - rcIcon1.Height()));
    DRect rcIcon2 = rcAlloc.Next(rcFull.Width() / 5., rcFull.Height() / 2., sx, sy);
    DRect  rcWin2 = rcAlloc.Next(rcFull.Width(), rcFull.Height(), sx, sy);
    rcAlloc.Shift(0, -(rcWin2.Height() - rcIcon2.Height()));
    DRect rcIcon3 = rcAlloc.Next(rcFull.Width() / 5., rcFull.Height(), sx, sy);
    rcWin1.cy -= rcFull.Height() / 6.;
    rcWin2.cx -= rcFull.Width() / 15.;
    rcIcon3.y -= rcFull.Height() / 3.5;
    rcIcon3.cx += rcFull.Width() / 15.;
    rcIcon3.cy += rcFull.Height() / 5.5;

    // active -> inactive order
    m_WndRect[WND_Back].Calc(ToCRect<double>(rcWin1), theme, true, true);
    m_WndRect[WND_Front].Calc(ToCRect<double>(rcWin2), theme, true, true);
    m_WndRect[WND_MsgBox].Calc(ToCRect<double>(rcIcon3), theme, false, true);

    dc.FillSolidRect(rcClient, theme.GetColor(COLOR_BACKGROUND));

    // inactive -> active order
    DrawRoutines::instance().DrawWindow(dc, m_WndRect[WND_Back], 0, CLegacyUIConfigurator::App()->GetMenu(), 2, theme);
    DrawRoutines::instance().DrawWindow(dc, m_WndRect[WND_Front], DC_ACTIVE, CLegacyUIConfigurator::App()->GetMenu(), -1, theme);
    DrawRoutines::instance().DrawWindow(dc, m_WndRect[WND_MsgBox], DC_ACTIVE | DC_SMALLCAP, nullptr, -1, theme);
}
