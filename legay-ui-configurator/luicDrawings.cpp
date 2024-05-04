#include "stdafx.h"
#include "luicDrawings.h"
#include "luicTheme.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "string.utils.error.code.h"

//
// https://stackoverflow.com/questions/14994012/how-draw-caption-in-alttab-switcher-when-paint-custom-captionframe
//

namespace
{

#if 0
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
#endif

/*
 * Borders:
 * (None),                        BDR_RAISEDOUTER,                                 BDR_SUNKENOUTER,               BDR_RAISEDOUTER | BDR_SUNKENOUTER
 * BDR_RAISEDINNER,               EDGE_RAISED,                                     EDGE_ETCHED,                   BDR_RAISEDOUTER | EDGE_ETCHED
 * BDR_SUNKENINNER,               EDGE_BUMP,                                       EDGE_SUNKEN,                   BDR_RAISEDOUTER | EDGE_SUNKEN
 * BDR_SUNKENOUTER | EDGE_SUNKEN, BDR_RAISEDOUTER | BDR_SUNKENOUTER | EDGE_SUNKEN, BDR_RAISEDINNER | EDGE_SUNKEN, BDR_RAISEDOUTER | BDR_RAISEDINNER | EDGE_SUNKEN
 *
 */

static const signed char LTInnerNormal[] = {
    -1,               -1,                -1,                -1,
    -1,               COLOR_3DHILIGHT,   COLOR_3DHILIGHT,   -1,
    -1,               COLOR_3DDKSHADOW,  COLOR_3DDKSHADOW,  -1,
    -1,               -1,                -1,                -1
};

static const signed char LTOuterNormal[] = {
    -1,               COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1,
    COLOR_3DHILIGHT,  COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1,
    COLOR_3DDKSHADOW, COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1,
    -1,               COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1
};

static const signed char RBInnerNormal[] = {
    -1,               -1,                -1,                -1,
    -1,               COLOR_3DSHADOW,    COLOR_3DSHADOW,    -1,
    -1,               COLOR_3DLIGHT,     COLOR_3DLIGHT,     -1,
    -1,               -1,                -1,                -1
};

static const signed char RBOuterNormal[] = {
    -1,               COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1,
    COLOR_3DSHADOW,   COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1,
    COLOR_3DLIGHT,    COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1,
    -1,               COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1
};

static const signed char LTRBOuterMono[] = {
    -1,               COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW,     COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW,     COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW,     COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
};

static const signed char LTRBInnerMono[] = {
    -1,               -1,                -1,                -1,
    -1,               COLOR_WINDOW,      COLOR_WINDOW,      COLOR_WINDOW,
    -1,               COLOR_WINDOW,      COLOR_WINDOW,      COLOR_WINDOW,
    -1,               COLOR_WINDOW,      COLOR_WINDOW,      COLOR_WINDOW,
};

} // namespace

struct CDrawRoutine::StaticInit
{
    enum : int
    {
        ICON_InactiveWnd = 0,
        ICON_ActiveWnd,
        ICON_Desktop1,
        ICON_Desktop2,
        ICON_Desktop3,
        ICON_Count
    };

    using DrawCaptionTempWFn = BOOL(WINAPI*)(HWND hWnd, HDC dc, const RECT* rcParam, HFONT fnMarlet, HICON hIcon, PCWSTR str, UINT uFlags);
    using SetSysColorsTempFn = DWORD_PTR(WINAPI*)(const COLORREF* pPens, const HBRUSH* pBrushes, DWORD_PTR n);
    using  DrawMenuBarTempFn = int(WINAPI*)(HWND hWnd, HDC dc, RECT* rcParam, HMENU hMenu, HFONT fnMarlet);

    HMODULE                      USER32;
    DrawCaptionTempWFn DrawCaptionTempW;
    SetSysColorsTempFn SetSysColorsTemp;
    DrawMenuBarTempFn   DrawMenuBarTemp;
    HICON           m_hIcon[ICON_Count];

    static StaticInit& instance()
    {
        static StaticInit inst;
        return inst;
    }

    static CFontHandle CreateMarlettFont(LONG height)
    {
        LOGFONT lf = { 0 };
        lstrcpy(lf.lfFaceName, TEXT("Marlett"));
        lf.lfHeight = height;
        lf.lfWidth = 0;
        lf.lfEscapement = 0;
        lf.lfOrientation = 0;
        lf.lfWeight = FW_NORMAL;
        lf.lfItalic = FALSE;
        lf.lfUnderline = FALSE;
        lf.lfStrikeOut = FALSE;
        lf.lfCharSet = SYMBOL_CHARSET;
        lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        lf.lfQuality = DEFAULT_QUALITY;
        lf.lfPitchAndFamily = DEFAULT_PITCH;
        return {CreateFontIndirect(&lf)};
    }

private:
    template <typename T>
    static bool GetProcAddressEx(HMODULE hMod, T& routine, PCSTR routineName, PCSTR modAlias)
    {
        *reinterpret_cast<FARPROC*>(&routine) = GetProcAddress(hMod, routineName);
        if (!routine) {
            DH::ThreadPrintf("%14s| '%s' is nullptr\n", modAlias, routineName);
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

        srand(static_cast<int>(time(nullptr)));
        auto const&     il = CLUIApp::App()->GetImageList(IL_SHELL_16x16);
        const int maxCount = il.GetImageCount();

        for (int i=0; i<ICON_Count; i++) {
            const int sellIconIndex = rand() % maxCount;
            m_hIcon[i] = il.GetIcon(sellIconIndex);
        }
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
    {
        ZeroMemory(&m_hIcon, sizeof(m_hIcon));
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

void CDrawRoutine::DrawEdge(CDCHandle dc, CRect& rcParam, UINT edge, UINT uFlags) const
{
    CRect rcInner = rcParam;
    HPEN  prevPen = nullptr;
    POINT savePoint;
    savePoint.x = 0;
    savePoint.y = 0;
    /* Determine the colors of the edges */
    unsigned char edgeIndex = edge & (BDR_INNER | BDR_OUTER);
    signed char LTInnerI, LTOuterI, RBInnerI, RBOuterI;
    if (uFlags & BF_SOFT) {
        LTInnerI = LTOuterNormal[edgeIndex];
        LTOuterI = LTInnerNormal[edgeIndex];
    }
    else {
        LTInnerI = LTInnerNormal[edgeIndex];
        LTOuterI = LTOuterNormal[edgeIndex];
    }
    RBInnerI = RBInnerNormal[edgeIndex];
    RBOuterI = RBOuterNormal[edgeIndex];

    int iPenLBPlus = (uFlags & BF_BOTTOMLEFT)  == BF_BOTTOMLEFT  ? 1 : 0;
    int iPenRTPlus = (uFlags & BF_TOPRIGHT)    == BF_TOPRIGHT    ? 1 : 0;
    int iPenRBPlus = (uFlags & BF_BOTTOMRIGHT) == BF_BOTTOMRIGHT ? 1 : 0;
    int iPenLTPlus = (uFlags & BF_TOPLEFT)     == BF_TOPLEFT     ? 1 : 0;

#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
#define SetPenColor(border) \
    SetDCPenColor(dc, m_Theme.GetColor(border))
    prevPen = dc.SelectPen(AtlGetStockPen(DC_PEN));
    SetPenColor(LTOuterI);
#else
    HPEN hPen = CreatePen(PS_SOLID, 1, m_Theme.GetColor(LTOuterI);
    prevPen = (HPEN)SelectObject(dc, hPen);
#define SetPenColor(border)                              \
    SelectObject(dc, prevPen);                            \
    DeleteObject(hPen);                                    \
    hPen = CreatePen(PS_SOLID, 1, m_Theme.GetColor(border); \
    SelectObject(dc, hPen)
#endif
    if (uFlags & BF_MIDDLE) {
        FillRect(dc, &rcInner, m_Theme.GetBrush(COLOR_3DFACE));
    }
    MoveToEx(dc, 0, 0, &savePoint);
    /* Draw the outer edge */
    if (LTOuterI >= 0) {
        if (uFlags & BF_TOP) {
            MoveToEx(dc, rcInner.left, rcInner.top, nullptr);
            LineTo(dc, rcInner.right, rcInner.top);
        }
        if (uFlags & BF_LEFT) {
            MoveToEx(dc, rcInner.left, rcInner.top, nullptr);
            LineTo(dc, rcInner.left, rcInner.bottom);
        }
    }
    if (RBOuterI >= 0) {
        SetPenColor(RBOuterI);
        if (uFlags & BF_BOTTOM) {
            MoveToEx(dc, rcInner.left, rcInner.bottom - 1, nullptr);
            LineTo(dc, rcInner.right, rcInner.bottom - 1);
        }
        if (uFlags & BF_RIGHT) {
            MoveToEx(dc, rcInner.right - 1, rcInner.top, nullptr);
            LineTo(dc, rcInner.right - 1, rcInner.bottom);
        }
    }
    /* Draw the inner edge */
    if (LTInnerI >= 0) {
        SetPenColor(LTInnerI);
        if (uFlags & BF_TOP) {
            MoveToEx(dc, rcInner.left + iPenLTPlus, rcInner.top + 1, nullptr);
            LineTo(dc, rcInner.right - iPenRTPlus, rcInner.top + 1);
        }
        if (uFlags & BF_LEFT) {
            MoveToEx(dc, rcInner.left + 1, rcInner.top + iPenLTPlus, nullptr);
            LineTo(dc, rcInner.left + 1, rcInner.bottom - iPenLBPlus);
        }
    }
    if (RBInnerI >= 0) {
        SetPenColor(RBInnerI);
        if (uFlags & BF_BOTTOM) {
            MoveToEx(dc, rcInner.left + iPenLBPlus, rcInner.bottom - 2, nullptr);
            LineTo(dc, rcInner.right - iPenRBPlus, rcInner.bottom - 2);
        }
        if (uFlags & BF_RIGHT) {
            MoveToEx(dc, rcInner.right - 2, rcInner.top + iPenRTPlus, nullptr);
            LineTo(dc, rcInner.right - 2, rcInner.bottom - iPenRBPlus);
        }
    }
    if (uFlags & BF_ADJUST) {
        const int add = (LTRBInnerMono[edgeIndex] >= 0 ? 1 : 0) +
                        (LTRBOuterMono[edgeIndex] >= 0 ? 1 : 0);
        if (uFlags & BF_LEFT) {
            rcInner.left += add;
        }
        if (uFlags & BF_RIGHT) {
            rcInner.right -= add;
        }
        if (uFlags & BF_TOP) {
            rcInner.top += add;
        }
        if (uFlags & BF_BOTTOM) {
            rcInner.bottom -= add;
        }
        if (uFlags & BF_ADJUST) {
            rcParam = rcInner;
        }
    }
    MoveToEx(dc, savePoint.x, savePoint.y, nullptr);
    SelectObject(dc, prevPen);
#if WINVER < WINVER_2K
    DeleteObject(hPen);
#endif
}

void CDrawRoutine::DrawFrameButton(CDCHandle dc, CRect& rcParam, UINT uState) const
{
    UINT edge;
    if (uState & (DFCS_PUSHED | DFCS_CHECKED | DFCS_FLAT)) {
        edge = EDGE_SUNKEN;
    }
    else {
        edge = EDGE_RAISED;
    }
    return DrawEdge(dc, rcParam, edge, (uState & DFCS_FLAT) | BF_RECT | BF_SOFT | BF_MIDDLE);
}

static CRect MakeSquareRect(CRect const& rcSrc)
{
    int     width = rcSrc.right - rcSrc.left;
    int    height = rcSrc.bottom - rcSrc.top;
    int smallDiam = ((width > height) ? height : width);
    CRect   rcDst = rcSrc;
    /* Make it a square box */
    if (width < height)       /* smallDiam == width */ {
        rcDst.top += (height - width) / 2;
        rcDst.bottom = rcDst.top + smallDiam;
    }
    else if (width > height)  /* smallDiam == height */ {
        rcDst.left += (width - height) / 2;
        rcDst.right = rcDst.left + smallDiam;
    }
    return rcDst;
}

void CDrawRoutine::DrawFrameCaption(CDCHandle dc, CRect& rcParam, UINT uFlags, CFont& fnMarlett) const
{
    TCHAR symbol = 0;
    switch (uFlags & 0xff) {
    case DFCS_CAPTIONCLOSE:   symbol = TEXT('r'); break;
    case DFCS_CAPTIONHELP:    symbol = TEXT('s'); break;
    case DFCS_CAPTIONMIN:     symbol = TEXT('0'); break;
    case DFCS_CAPTIONMAX:     symbol = TEXT('1'); break;
    case DFCS_CAPTIONRESTORE: symbol = TEXT('2'); break;
    default: break;
    }
    DrawEdge(dc, rcParam, static_cast<UINT>((uFlags & DFCS_PUSHED) ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT | BF_MIDDLE | BF_SOFT);
    CRect myRect = MakeSquareRect(rcParam);
    myRect.left += 2;
    myRect.top += 2;
    myRect.right -= 1;
    myRect.bottom -= 2;
    if (uFlags & DFCS_PUSHED) {
        OffsetRect(&myRect, 1, 1);
    }
    if (0 == symbol) {
        return ;
    }
    if (!fnMarlett.m_hFont) {
        fnMarlett = StaticInit::CreateMarlettFont(myRect.bottom - myRect.top);
    }
    HFONT prevFont = dc.SelectFont(fnMarlett);
    int   prevMode = dc.SetBkMode(TRANSPARENT);
    if (uFlags & DFCS_INACTIVE) { // Draw shadow
        SetTextColor(dc, m_Theme.GetColor(COLOR_3DHILIGHT));
        TextOut(dc, myRect.left + 1, myRect.top + 1, &symbol, 1);
    }
    SetTextColor(dc, m_Theme.GetColor((uFlags & DFCS_INACTIVE) ? COLOR_3DSHADOW : COLOR_BTNTEXT));
    TextOut(dc, myRect.left, myRect.top, &symbol, 1);
    dc.SetBkMode(prevMode);
    dc.SelectFont(prevFont);
}

void CDrawRoutine::DrawFrameScroll(CDCHandle dc, CRect& rcParam, UINT uFlags, CFont& fnMarlett) const
{
    TCHAR symbol = 0;
    switch (uFlags & 0xff) {
    case DFCS_SCROLLCOMBOBOX:
    case DFCS_SCROLLDOWN:       symbol = TEXT('6'); break;
    case DFCS_SCROLLUP:         symbol = TEXT('5'); break;
    case DFCS_SCROLLLEFT:       symbol = TEXT('3'); break;
    case DFCS_SCROLLRIGHT:      symbol = TEXT('4'); break;
    default: break;
    }
    DrawEdge(dc, rcParam,
        (uFlags & DFCS_PUSHED) ? (UINT)EDGE_SUNKEN : (UINT)EDGE_RAISED,
        (uFlags & DFCS_FLAT) | BF_MIDDLE | BF_RECT);
    CRect myRect = MakeSquareRect(rcParam);
    myRect.left += 2;
    myRect.top += 2;
    myRect.right -= 2;
    myRect.bottom -= 2;
    if (uFlags & DFCS_PUSHED) {
        OffsetRect(&myRect, 1, 1);
    }
    if (0 == symbol) {
        return ;
    }
    if (!fnMarlett.m_hFont) {
        fnMarlett = StaticInit::CreateMarlettFont(myRect.bottom - myRect.top);
    }
    HFONT prevFont = dc.SelectFont(fnMarlett);
    int   prevMode = dc.SetBkMode(TRANSPARENT);
    if (uFlags & DFCS_INACTIVE) {
        SetTextColor(dc, m_Theme.GetColor(COLOR_3DHILIGHT));
        dc.TextOutW(myRect.left + 1, myRect.top + 1, &symbol, 1);
    }
    dc.SetTextColor(m_Theme.GetColor((uFlags & DFCS_INACTIVE) ? COLOR_3DSHADOW : COLOR_BTNTEXT));
    dc.TextOutW(myRect.left, myRect.top, &symbol, 1);
    dc.SetBkMode(prevMode);
    dc.SelectFont(prevFont);
}


void CDrawRoutine::DrawFrameControl(CDCHandle dc, CRect& rcParam, UINT uType, UINT uState, CFont& fnMarlett) const
{
    switch (uType) {
    case DFC_CAPTION: DrawFrameCaption(dc, rcParam, uState, fnMarlett); break;
    case DFC_SCROLL:  DrawFrameScroll(dc, rcParam, uState, fnMarlett); break;
    case DFC_BUTTON:
    default:
        DrawFrameButton(dc, rcParam, uState);
        break;
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

    CFont fnMarlett;
    DrawFrameControl(dc, rc, DFC_CAPTION, DFCS_CAPTIONCLOSE, fnMarlett);

    if (withMinMax) {
        rc.right = rc.left - margin;
        rc.left = rc.right - buttonWidth;
        DrawFrameControl(dc, rc, DFC_CAPTION, DFCS_CAPTIONMAX, fnMarlett);

        rc.right = rc.left;
        rc.left = rc.right - buttonWidth;
        DrawFrameControl(dc, rc, DFC_CAPTION, DFCS_CAPTIONMIN, fnMarlett);
    }
    return rc.left;
}

void CDrawRoutine::DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT fnMarlet, HICON hIcon, PCWSTR str, UINT uFlags) const
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

    HFONT prevFont = dc.SelectFont(fnMarlet);
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

void CDrawRoutine::DrawMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format, int color) const
{
    SetTextColor(dc, m_Theme.GetColor(color));
    DrawTextW(dc, text, -1, rc, format);
}

void CDrawRoutine::DrawDisabledMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format) const
{
    OffsetRect(rc, 1, 1);
    DrawMenuText(dc, text, rc, format, COLOR_3DHILIGHT);
    OffsetRect(rc, -1, -1);
    DrawMenuText(dc, text, rc, format, COLOR_3DSHADOW);
}

void CDrawRoutine::DrawMenuBar(CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT fnMarlet, int selectedItem) const
{
    int backColorIndex = COLOR_MENU;
#if WINVER >= WINVER_XP
    if (m_Theme.IsFlatMenus()) {
        backColorIndex = COLOR_MENUBAR;
    }
#endif
    dc.FillRect(rc, m_Theme.GetBrush(backColorIndex));

    if (!hMenu || !fnMarlet) {
        return ;
    }

    int    spacing = 10;
    HFONT  prevFnt = dc.SelectFont(fnMarlet);
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
    CFont        fnMarlett;
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
        DrawFrameControl(dc, rc, DFC_SCROLL, DFCS_SCROLLUP | frameControlFlags, fnMarlett);
        rc.top = rcParam.bottom - buttonHeight;
        rc.bottom = rcParam.bottom;
        DrawFrameControl(dc, rc, DFC_SCROLL, DFCS_SCROLLDOWN | frameControlFlags, fnMarlett);
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
    LRect      rcMessage;
    LRect       rcButton;

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

    rcWork.Shrink(2, 2);
    rcWork.y = rcMenu.Bottom() + 1;
    target.m_rcWorkspace = ToCRect(rcWork);

    if (!isToolWnd) {
        rcScroll = rcWork;
        rcScroll.Shrink(0, 2);
        rcScroll.cx = m_Theme.GetNcMetrcs().iScrollWidth;
        rcScroll.x = rcWork.Right() - rcScroll.cx - 2;
        target.m_rcScroll = ToCRect(rcScroll);
    }
    else {
        long sx = ScaleForDpi<long>(4);
        long cy = -(m_Theme.GetLogFont(FONT_Message)->lfHeight) + 2;
        rcMessage = rcWork;
        rcMessage.Shrink(sx, sx - 1);
        rcMessage.cy = cy;
        target.m_rcMessage = ToCRect(rcMessage);
        rcMessage.y += cy + 2;
        target.m_rcURL = ToCRect(rcMessage);
        rcButton = rcWork;
        rcButton.cx = rcWork.Width() / 2;
        rcButton.cy = cy * 2;
        rcButton.PutInto(rcWork, PutAt::Bottom | PutAt::XCenter);
        rcButton.y -= sx;
        target.m_rcButton = ToCRect(rcButton);
    }
}

void CDrawRoutine::DrawWindow(CDCHandle dc, DrawWindowArgs const& params) const
{
    HICON const*          icons = StaticInit::instance().m_hIcon;
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
        if (isActive) { captIcon = icons[StaticInit::ICON_ActiveWnd]; }
        else          { captIcon = icons[StaticInit::ICON_InactiveWnd]; }
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
            rcText.DeflateRect(rcWork.Width() / 16, 10);
            CRect  rcLine = rcText;
            LONG       cy = -(m_Theme.GetLogFont(FONT_Desktop)->lfHeight) + 2;
            rcLine.bottom = rcLine.top + cy;

            for (int i = 0; i < params.text.lineCount; i++) {
                if (rcLine.top > rcText.bottom) {
                    break;
                }
                const auto& line = params.text.line[i];
                if (line.flags & WT_Select) {
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
    else {
        if (params.text.lineCount < 3) {
            return;
        }
        CRect rc = rects.m_rcMessage;
        if (rc.top >= rc.bottom) {
            return ;
        }
        ATL::CStringW const& line0 = params.text.line[0].text;
        ATL::CStringW const& line1 = params.text.line[1].text;
        ATL::CStringW const& line2 = params.text.line[2].text;
        int prevBkMode = dc.SetBkMode(TRANSPARENT);
        HFONT prevFont = dc.SelectFont(m_Theme.GetFont(FONT_Message));
        SetTextColor(dc, m_Theme.GetColor(COLOR_WINDOWTEXT));
        dc.DrawTextW(line0.GetString(), line0.GetLength(), rc, DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        dc.SelectFont(prevFont);
#if WINVER >= WINVER_2K
        rc = rects.m_rcURL;
        if (rc.top >= rc.bottom) {
            return ;
        }
        prevFont = dc.SelectFont(m_Theme.GetFont(FONT_Hyperlink));
        SetTextColor(dc, m_Theme.GetColor(COLOR_HOTLIGHT));
        dc.DrawTextW(line1.GetString(), line1.GetLength(), rc, DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        dc.SelectFont(prevFont);
#endif
        rc = rects.m_rcButton;
        if (rc.top >= rc.bottom) {
            return ;
        }
        InflateRect(rc, 1, 1);
        DrawBorder(dc, rc, 1, m_Theme.GetBrush(COLOR_WINDOWFRAME));
        DrawFrameButton(dc, rc, DFCS_BUTTONPUSH);
        rc.bottom--;
        prevFont = dc.SelectFont(m_Theme.GetFont(FONT_Message));
        dc.SetTextColor(m_Theme.GetColor(COLOR_BTNTEXT));
        dc.DrawTextW(line2.GetString(), line2.GetLength(), rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        dc.SelectFont(prevFont);
        dc.SetBkMode(prevBkMode);
    }
}
