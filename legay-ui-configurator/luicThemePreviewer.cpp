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
    using DrawCaptionTempWFn = BOOL(WINAPI*)(HWND hWnd, HDC hdc, const RECT* pRect, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags);
    using SetSysColorsTempFn = DWORD_PTR(WINAPI*)(const COLORREF* pPens, const HBRUSH* pBrushes, DWORD_PTR n);
    using  DrawMenuBarTempFn = int(WINAPI*)(HWND hWnd, HDC hdc, RECT* pRect, HMENU hMenu, HFONT hFont);

    DrawCaptionTempWFn DrawCaptionTempW;
    SetSysColorsTempFn SetSysColorsTemp;
    DrawMenuBarTempFn   DrawMenuBarTemp;

    static DrawRoutines& instance();

    static void DrawBorder(CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush);
    void DrawWindow(CDCHandle dc, CTheme const& theme, const CWndFrameRects& rects, UINT flags);

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

void DrawRoutines::DrawWindow(CDCHandle dc, const CTheme& theme, const CWndFrameRects& rects, UINT flags)
{
    HFONT  captFont = theme.m_Font[FONT_Caption];
    HICON  captIcon = CLegacyUIConfigurator::App()->GetIcon(IconAppearance);
    PCWSTR captText = L"Caption text...";
    UINT  captFlags = DC_TEXT;
    bool   mdiFrame = false;

    if (0 == (DC_SMALLCAP & flags)) {
        captIcon = CLegacyUIConfigurator::App()->GetIcon(IconAppearance);
        captFlags |= DC_ICON;
        mdiFrame = true;
    }
    else {
        captFlags |= DC_SMALLCAP;
    }

#if WINVER >= WINVER_2K
    if (theme.m_bGradientCaptions) {
        captFlags |= DC_GRADIENT;
    }
#endif
    int borderColorIndex = COLOR_INACTIVEBORDER;
    if (0 != (DC_ACTIVE & flags)) {
        borderColorIndex = COLOR_ACTIVEBORDER;
        captFlags |= DC_ACTIVE;
        captIcon = CLegacyUIConfigurator::App()->GetIcon(IconStartmenu);
    }
#if (_WIN32_WINNT >= 0x0501)
    captFlags |= DC_BUTTONS;
#endif

    DrawBorder(dc, rects.m_rcBorder, rects.m_BorderSize, theme.m_Brush[borderColorIndex]);
    dc.DrawEdge(CRect(rects.m_rcBorder), EDGE_RAISED, BF_RECT | BF_ADJUST); // *****

    dc.FillSolidRect(rects.m_rcFrame, theme.m_Color[COLOR_MENU]);

    DrawCaptionTempW(nullptr, dc, rects.m_rcCapt, captFont, captIcon, captText, captFlags);

    if (mdiFrame) {
        if (0) {
            //rcMDI = rcFrame;
            //rcMDI.Shrink(2, 2);

            //CRect rcLMDI = ToCRect<long>(rcMDI);
            //dc.FillSolidRect(rcLMDI, theme.m_Color[COLOR_APPWORKSPACE]);
            //dc.DrawEdge(rcLMDI, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

            //
        }
    }
}

///----------------------------------------------------------------------------

void CWndFrameRects::Calc(CRect const& rc, const CTheme& theme)
{
    long   dpiScale = ScaleForDpi<long>(8);
    LRect  rcBorder = FromCRect<long>(rc);
    LRect   rcFrame;
    LRect    rcCapt;
    LRect    rcMenu;
    LRect     rcMDI;

    m_rcBorder = ToCRect(rcBorder);

    m_BorderSize = theme.m_ncMetrics.iBorderWidth + 1;
#if WINVER >= WINVER_VISTA
    m_BorderSize += theme.m_ncMetrics.iPaddedBorderWidth;
#endif

    rcFrame = rcBorder;
    rcFrame.Shrink(m_BorderSize, m_BorderSize);
    rcFrame.PutInto(rcBorder, PutAt::Center);
    m_rcFrame = ToCRect(rcFrame);

    rcCapt = rcFrame;
    rcCapt.cy = theme.m_ncMetrics.iCaptionHeight;
    rcCapt.Shrink(1, 1);
    m_rcCapt = ToCRect(rcCapt);
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

    dc.FillSolidRect(rcClient, theme.m_Color[COLOR_BACKGROUND]);

    // active -> inactive order
    m_WndRect[2].Calc(ToCRect<double>(rcIcon3), theme);
    m_WndRect[1].Calc(ToCRect<double>(rcWin1), theme);
    m_WndRect[0].Calc(ToCRect<double>(rcWin2), theme);

    // inactive -> active order
    DrawRoutines::instance().DrawWindow(dc, theme, m_WndRect[1], 0);
    DrawRoutines::instance().DrawWindow(dc, theme, m_WndRect[0], DC_ACTIVE);
    DrawRoutines::instance().DrawWindow(dc, theme, m_WndRect[2], DC_ACTIVE | DC_SMALLCAP);
    
}
