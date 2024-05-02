#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "dh.tracing.h"
#include "rect.alloc.h"

// https://stackoverflow.com/questions/14994012/how-draw-caption-in-alttab-switcher-when-paint-custom-captionframe

class DrawRoutines
{
    HMODULE USER32;

public:
    using DrawCaptionTempWFn = BOOL(WINAPI*)(HWND hWnd, HDC dc, const RECT* pRect, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags);
    using SetSysColorsTempFn = DWORD_PTR(WINAPI*)(const COLORREF* pPens, const HBRUSH* pBrushes, DWORD_PTR n);
    using  DrawMenuBarTempFn = int(WINAPI*)(HWND hWnd, HDC dc, RECT* pRect, HMENU hMenu, HFONT hFont);

    DrawCaptionTempWFn DrawCaptionTempW;
    SetSysColorsTempFn SetSysColorsTemp;
    DrawMenuBarTempFn   DrawMenuBarTemp;

    static DrawRoutines& instance();

    static void DrawBorder(CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush);
    static LONG DrawCaptionButtons(CDCHandle dc, CRect const& rcCaption, bool withMinMax, int buttonWidth, UINT uFlags,
                                   const CTheme& theme);
    static void DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags, const CTheme& theme);
    void DrawWindow(CDCHandle dc, CTheme const& theme, const CWndFrameRects& rects, UINT flags, HMENU hMenu);

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
    if (USER32) {
        FreeLibrary(USER32);
    }
}

DrawRoutines::DrawRoutines()
    :           USER32{ LoadLibraryW(L"USER32") }
    , DrawCaptionTempW{ nullptr }
    , SetSysColorsTemp{ nullptr }
    ,  DrawMenuBarTemp{ nullptr }

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
    GetProcAddressEX(USER32, DrawCaptionTempW);
    GetProcAddressEX(USER32, SetSysColorsTemp);
    GetProcAddressEX(USER32, DrawMenuBarTemp);
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
                                      UINT uFlags, const CTheme& theme)
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

void DrawRoutines::DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags, const CTheme& theme)
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


void DrawRoutines::DrawWindow(CDCHandle dc, const CTheme& theme, const CWndFrameRects& rects, UINT flags, HMENU hMenu)
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
    int borderColorIndex = COLOR_INACTIVEBORDER;
    if (0 != (DC_ACTIVE & flags)) {
        borderColorIndex = COLOR_ACTIVEBORDER;
    }

    DrawBorder(dc, rects.m_rcBorder, rects.m_BorderSize, theme.GetBrush(borderColorIndex));
    dc.DrawEdge(CRect(rects.m_rcBorder), EDGE_RAISED, BF_RECT | BF_ADJUST); // *****
    dc.FillSolidRect(rects.m_rcFrame, theme.GetColor(COLOR_MENU));

    CRect rcCapt = rects.m_rcCapt;
    rcCapt.right = DrawCaptionButtons(dc, rcCapt, (0 == (DC_SMALLCAP & flags)), 32, captFlags, theme);
    DrawCaption(dc, rcCapt, captFont, captIcon, captText, captFlags, theme);

    if (hMenu) {
        DrawMenuBarTemp(nullptr, dc, CRect(rects.m_rcMenu), hMenu, menuFont);
    }
    if (0) {
        //rcMDI = rcFrame;
        //rcMDI.Shrink(2, 2);

        //CRect rcLMDI = ToCRect<long>(rcMDI);
        //dc.FillSolidRect(rcLMDI, theme.m_Color[COLOR_APPWORKSPACE]);
        //dc.DrawEdge(rcLMDI, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

        //
    }
}

///----------------------------------------------------------------------------

void CWndFrameRects::Calc(CRect const& rc, const CTheme& theme, bool wMenu, bool wWorkspace)
{
    long   dpiScale = ScaleForDpi<long>(8);
    LRect  rcBorder = FromCRect<long>(rc);
    LRect   rcFrame;
    LRect    rcCapt;
    LRect    rcMenu;
    LRect     rcMDI;

    m_rcBorder = ToCRect(rcBorder);

    m_BorderSize = theme.GetNcMetrcs().iBorderWidth + 1;
#if WINVER >= WINVER_VISTA
    m_BorderSize += theme.GetNcMetrcs().iPaddedBorderWidth;
#endif

    rcFrame = rcBorder;
    rcFrame.Shrink(m_BorderSize, m_BorderSize);
    rcFrame.PutInto(rcBorder, PutAt::Center);
    m_rcFrame = ToCRect(rcFrame);

    rcCapt = rcFrame;
    rcCapt.cy = theme.GetNcMetrcs().iCaptionHeight;
    rcCapt.Shrink(1, 1);
    m_rcCapt = ToCRect(rcCapt);

    if (wMenu) {
        rcMenu = rcCapt;
        rcMenu.y += rcCapt.cy + 1;
        m_rcMenu = ToCRect(rcMenu);
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

    dc.FillSolidRect(rcClient, theme.GetColor(COLOR_BACKGROUND));

    // active -> inactive order
    m_WndRect[2].Calc(ToCRect<double>(rcIcon3), theme, false, false);
    m_WndRect[1].Calc(ToCRect<double>(rcWin1), theme, true, true);
    m_WndRect[0].Calc(ToCRect<double>(rcWin2), theme, true, true);

    // inactive -> active order
    DrawRoutines::instance().DrawWindow(dc, theme, m_WndRect[1], 0, CLegacyUIConfigurator::App()->GetMenu());
    DrawRoutines::instance().DrawWindow(dc, theme, m_WndRect[0], DC_ACTIVE, CLegacyUIConfigurator::App()->GetMenu());
    DrawRoutines::instance().DrawWindow(dc, theme, m_WndRect[2], DC_ACTIVE | DC_SMALLCAP, nullptr);
    
}
