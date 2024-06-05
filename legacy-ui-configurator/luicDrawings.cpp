#include "stdafx.h"
#include "luicDrawings.h"
#include "luicScheme.h"
#include "luicUtils.h"
#include "luicMain.h"
#include <dh.tracing.h>
#include <rect.putinto.h>
#include <atltheme.h>
#include <utility>

//
// https://stackoverflow.com/questions/14994012/how-draw-caption-in-alttab-switcher-when-paint-custom-captionframe
//

namespace
{
#define GetProcAddressEX(Module, Routine) \
    GetProcAddressEx(Module, Routine, #Routine, #Module)

/*
 * Borders:
 * (None),                        BDR_RAISEDOUTER,                                 BDR_SUNKENOUTER,               BDR_RAISEDOUTER | BDR_SUNKENOUTER
 * BDR_RAISEDINNER,               EDGE_RAISED,                                     EDGE_ETCHED,                   BDR_RAISEDOUTER | EDGE_ETCHED
 * BDR_SUNKENINNER,               EDGE_BUMP,                                       EDGE_SUNKEN,                   BDR_RAISEDOUTER | EDGE_SUNKEN
 * BDR_SUNKENOUTER | EDGE_SUNKEN, BDR_RAISEDOUTER | BDR_SUNKENOUTER | EDGE_SUNKEN, BDR_RAISEDINNER | EDGE_SUNKEN, BDR_RAISEDOUTER | BDR_RAISEDINNER | EDGE_SUNKEN
 *
 */

static constexpr signed char LTInnerNormal[] = {
    -1,               -1,                -1,                -1,
    -1,               COLOR_3DHILIGHT,   COLOR_3DHILIGHT,   -1,
    -1,               COLOR_3DDKSHADOW,  COLOR_3DDKSHADOW,  -1,
    -1,               -1,                -1,                -1
};

static constexpr signed char LTOuterNormal[] = {
    -1,               COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1,
    COLOR_3DHILIGHT,  COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1,
    COLOR_3DDKSHADOW, COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1,
    -1,               COLOR_3DLIGHT,     COLOR_3DSHADOW,    -1
};

static constexpr signed char RBInnerNormal[] = {
    -1,               -1,                -1,                -1,
    -1,               COLOR_3DSHADOW,    COLOR_3DSHADOW,    -1,
    -1,               COLOR_3DLIGHT,     COLOR_3DLIGHT,     -1,
    -1,               -1,                -1,                -1
};

static constexpr signed char RBOuterNormal[] = {
    -1,               COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1,
    COLOR_3DSHADOW,   COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1,
    COLOR_3DLIGHT,    COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1,
    -1,               COLOR_3DDKSHADOW,  COLOR_3DHILIGHT,   -1
};

static constexpr signed char LTRBOuterMono[] = {
    -1,               COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW,     COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW,     COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW,     COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
};

static constexpr signed char LTRBInnerMono[] = {
    -1,               -1,                -1,                -1,
    -1,               COLOR_WINDOW,      COLOR_WINDOW,      COLOR_WINDOW,
    -1,               COLOR_WINDOW,      COLOR_WINDOW,      COLOR_WINDOW,
    -1,               COLOR_WINDOW,      COLOR_WINDOW,      COLOR_WINDOW,
};

} // namespace

//-----------------------------------------------------------------------------
//
// CInTheme UXTheme wrapper
//
//-----------------------------------------------------------------------------
struct CDrawings::CInTheme: WTL::CThemeImpl<CInTheme>
{
    HWND m_hWnd;

    ~CInTheme();
    CInTheme();

    HRESULT Init(HWND hWnd);

    DWORD GetExStyle() const;

    bool TextPut(WTL::CDCHandle       dc,
                 CRect&           rcDraw,
                 PCWSTR           szText,
                 int                nLen,
                 int             nPartId,
                 int            nStateId,
                 DWORD           dwFlags,
                 const PDTTOPTS pOptions);
};

CDrawings::CInTheme::~CInTheme() = default;

CDrawings::CInTheme::CInTheme()
    : WTL::CThemeImpl<CInTheme>{}
    , m_hWnd{nullptr}
{
}

HRESULT CDrawings::CInTheme::Init(HWND hWnd)
{
    // All class names are in $(PROJECT_DIR)\resz\ClassesNames.txt
    //
    // Short list:
    // BUTTON, CLOCK, COMBOBOX, COMMUNICATIONS, CONTROLPANEL, DATEPICKER, DRAGDROP, 
    // EDIT, EXPLORERBAR, FLYOUT, GLOBALS, HEADER, LISTBOX, LISTVIEW, MENU, MENUBAND, 
    // NAVIGATION, PAGE, PROGRESS, REBAR, SCROLLBAR, SEARCHEDITBOX, SPIN, STARTPANEL, 
    // STATUS, TAB, TASKBAND, TASKBAR, TASKDIALOG, TEXTSTYLE, TOOLBAR, TOOLTIP, 
    // TRACKBAR, TRAYNOTIFY, TREEVIEW, WINDOW
    //
    // https://learn.microsoft.com/en-us/windows/win32/controls/parts-and-states?redirectedfrom=MSDN
    //
    static const PCWSTR pszClasses = L"BUTTON;EDIT;MENU;SCROLLBAR";
    HRESULT code = S_OK;
    if (!hWnd) {
        return S_FALSE;
    }
    if (!IsThemingSupported()) {
        return S_FALSE;
    }
    m_hWnd = hWnd;
    ATLASSERT(::IsWindow(m_hWnd));
    const HTHEME hTheme = ::OpenThemeDataEx(m_hWnd, pszClasses, OTD_FORCE_RECT_SIZING | OTD_NONCLIENT);
    if (!hTheme) {
        code = static_cast<HRESULT>(GetLastError());
        return code;
    }
    m_hTheme = hTheme;
    return S_OK;
}

DWORD CDrawings::CInTheme::GetExStyle() const
{
    return static_cast<DWORD>(::GetWindowLongW(m_hWnd, GWL_EXSTYLE));
}

bool CDrawings::CInTheme::TextPut(WTL::CDCHandle dc, CRect& rcDraw, PCWSTR szText, int nLen, int nPartId, int nStateId,
    DWORD dwFlags, const PDTTOPTS pOptions)
{
    if (!m_hTheme) {
        return false;
    }
    const HRESULT code = DrawThemeTextEx(dc, nPartId, nStateId, szText, nLen, dwFlags, rcDraw, pOptions);
    return SUCCEEDED(code);
}

//-----------------------------------------------------------------------------
//
// CStaticRes misc global resoursec
//
//-----------------------------------------------------------------------------
struct CDrawings::CStaticRes
{
    enum : int
    {
        ICON_InactiveWnd = 0,
        ICON_ActiveWnd,
        ICON_Desktop1,
        ICON_Cursor1,
        ICON_Count
    };

    CInTheme        m_InTheme;
    HICON m_hIcon[ICON_Count];

    static WTL::CFontHandle CreateMarlettFont(LONG height);

    ~CStaticRes();
    CStaticRes();

    HRESULT Init(HWND hWnd);
    HICON const* GetIcons() const;

private:
    template <typename T>
    static bool GetProcAddressEx(HMODULE hMod, T& routine, PCSTR routineName, PCSTR modAlias);
};

CDrawings::CStaticRes::~CStaticRes() = default;

CDrawings::CStaticRes::CStaticRes()
    : m_InTheme{}
{
    ZeroMemory(&m_hIcon, sizeof(m_hIcon));
}

template <typename T>
bool CDrawings::CStaticRes::GetProcAddressEx(HMODULE hMod, T& routine, PCSTR routineName, PCSTR modAlias)
{
    *reinterpret_cast<FARPROC*>(&routine) = GetProcAddress(hMod, routineName);
    if (!routine) {
        DH::TPrintf("%14s| '%s' is nullptr\n", modAlias, routineName);
    }
    return nullptr != routine;
}

WTL::CFontHandle CDrawings::CStaticRes::CreateMarlettFont(LONG height)
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

HRESULT CDrawings::CStaticRes::Init(HWND hWnd)
{
    auto const& ilBig = CLUIApp::App()->GetImageList(IL_OwnBig);
    m_hIcon[ICON_Desktop1] = ilBig.GetIcon(IconMyComp); // IconMatreshka
    m_hIcon[ICON_Cursor1] = (HICON)LoadCursorW(nullptr, IDC_APPSTARTING);

    srand(static_cast<int>(time(nullptr)));
    auto const& ilSmall = CLUIApp::App()->GetImageList(IL_SHELL_16x16);
    const int  maxCount = ilSmall.GetImageCount() - 1;
    m_hIcon[ICON_ActiveWnd]   = ilSmall.GetIcon(rand() % maxCount);
    m_hIcon[ICON_InactiveWnd] = ilSmall.GetIcon(rand() % maxCount);

    return m_InTheme.Init(hWnd);
}

HICON const* CDrawings::CStaticRes::GetIcons() const
{
    return m_hIcon;
}

//-----------------------------------------------------------------------------
//
// CDrawings's
//
//-----------------------------------------------------------------------------

CDrawings::PStaticRes CDrawings::m_pStaticRes = {};

CDrawings::~CDrawings() = default;

CDrawings::CDrawings(CScheme const& scheme, CSizePair const& sizePair)
    :     m_Scheme{scheme}
    ,   m_SizePair{sizePair}
    , m_BorderSize{m_SizePair.m_NCMetric.iBorderWidth}
{
    m_BorderSize += m_SizePair.m_NCMetric.iPaddedBorderWidth;
}

void CDrawings::CalcRects(CRect const& rcBorder, UINT captFlags, WindowRects& target)
{
    const long  brdScale{0}; // ScaleForDpi<long>(2)}; // make it more conviniet to select borders
    const bool isToolWnd{0 != (DC_SMALLCAP & captFlags)};
    CRect        rcFrame{};
    CRect         rcCapt{};
    CRect         rcMenu{};
    CRect         rcWork{};
    CRect       rcScroll{};
    CRect      rcMessage{};
    CRect    rcHyperlink{};
    CRect       rcButton{};
    CRect      rcTooltip{};

    target[WR_Border] = rcBorder;

    rcFrame = rcBorder;
    rcFrame.DeflateRect(m_BorderSize + brdScale, m_BorderSize + brdScale);
    target[WR_Frame] = rcFrame;
    rcFrame.DeflateRect(3, 2);
    rcWork = rcFrame;

    rcCapt = rcFrame;
    Rc::SetHeight(rcCapt, m_SizePair.m_NCMetric.iCaptionHeight);
    target[WR_Caption] = rcCapt;
    rcWork.top = rcCapt.bottom;

    if (!isToolWnd) {
        rcMenu = rcCapt;
        Rc::SetHeight(rcMenu, m_SizePair.m_NCMetric.iMenuHeight + 2);
        Rc::OffsetY(rcMenu, rcCapt.Height());
        target[WR_Menu] = rcMenu;
        rcWork.top = rcMenu.bottom;
    }
    target[WR_Workspace] = rcWork;

    if (!isToolWnd) {
        rcScroll = rcWork;
        Rc::ShrinkY(rcScroll, 2);
        Rc::SetWidth(rcScroll, m_SizePair.m_NCMetric.iScrollWidth);
        Rc::OffsetX(rcScroll, rcWork.Width() - rcScroll.Width() - 2);
        target[WR_Scroll] = rcScroll;
        rcWork.right = rcScroll.left;
    }
    else {
        const long nShrink = ScaleForDpi<long>(4);
        const long  cyFont = -(m_SizePair.GetLogFont(FONT_Message).lfHeight) + 2;
        rcMessage = rcWork;
        Rc::SetHeight(rcMessage, cyFont + 8);
        Rc::OffsetY(rcMessage, 4);
        rcMessage.DeflateRect(nShrink, nShrink);
        target[WR_Message] = rcMessage;

        rcHyperlink = rcMessage;
        Rc::OffsetY(rcHyperlink, cyFont + 8);
        target[WR_Hyperlink] = rcHyperlink;

        rcButton = CRect{ 0, 0, rcWork.Width() / 2, cyFont * 2 };
        Rc::PutInto(rcWork, rcButton, Rc::Bottom | Rc::XCenter);
        Rc::OffsetY(rcButton, -nShrink);
        target[WR_Button] = rcButton;

        rcTooltip = rcButton;
        rcTooltip.left = rcButton.right + 2;
        Rc::OffsetY(rcTooltip, rcButton.Height() / 2);
        rcTooltip.InflateRect(3, 3);
        target[WR_Tooltip] = rcTooltip;
    }
}

HRESULT CDrawings::StaticInit(HWND hWnd)
{
    m_pStaticRes = std::make_unique<CStaticRes>();
    return m_pStaticRes->Init(hWnd);
}

void CDrawings::StaticFree()
{
    PStaticRes{}.swap(m_pStaticRes);
}

UINT CDrawings::GetDrawItemFrameType(UINT nCtlType)
{
    switch (nCtlType) {
    case ODT_BUTTON:    return DFC_BUTTON;
    case ODT_MENU:      return DFC_MENU;
    case ODT_COMBOBOX:
    case ODT_LISTBOX:
    case ODT_LISTVIEW:
    case ODT_STATIC:
    case ODT_TAB:
        break;
    }
    return 0;
}

UINT CDrawings::ConvDrawItemState(UINT diState)
{
    UINT result = 0;
    if ((diState & ODS_DISABLED) || (diState & ODS_GRAYED) || (diState & ODS_INACTIVE)) {
        result |= DFCS_INACTIVE;
    }
    if (diState & ODS_CHECKED) {
        result |= DFCS_CHECKED;
    }
    if (diState & ODS_SELECTED) {
        result |= DFCS_PUSHED;
    }
    if (diState & ODS_HOTLIGHT) {
        result |= DFCS_HOT;
    }
    // ODS_FOCUS ODS_NOACCEL ODS_NOFOCUSRECT
    return result;
}

void CDrawings::DrawBorder(WTL::CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush)
{
    CRect           rc{rcParam};
    const auto hPrevBr{dc.SelectBrush(hBrush)};
    auto        length{rc.bottom - rc.top};
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

void CDrawings::DrawEdge(WTL::CDCHandle dc, CRect& rcParam, UINT edge, UINT uFlags) const
{
    CRect rcInner = rcParam;
    HPEN  prevPen = nullptr;
    POINT savePoint;
    savePoint.x = 0;
    savePoint.y = 0;
    /* Determine the colors of the edges */
    const unsigned char edgeIndex = edge & (BDR_INNER | BDR_OUTER);
    signed char LTInnerI = 0;
    signed char LTOuterI = 0;
    signed char RBInnerI = 0;
    signed char RBOuterI = 0;
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

    const int iPenLBPlus = (uFlags & BF_BOTTOMLEFT)  == BF_BOTTOMLEFT  ? 1 : 0;
    const int iPenRTPlus = (uFlags & BF_TOPRIGHT)    == BF_TOPRIGHT    ? 1 : 0;
    const int iPenRBPlus = (uFlags & BF_BOTTOMRIGHT) == BF_BOTTOMRIGHT ? 1 : 0;
    const int iPenLTPlus = (uFlags & BF_TOPLEFT)     == BF_TOPLEFT     ? 1 : 0;

#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
#define SetPenColor(border) \
    SetDCPenColor(dc, m_Scheme.GetColor(border))
    prevPen = dc.SelectPen(WTL::AtlGetStockPen(DC_PEN));
    SetPenColor(LTOuterI);
#else
    HPEN hPen = CreatePen(PS_SOLID, 1, m_Scheme.GetColor(LTOuterI);
    prevPen = (HPEN)SelectObject(dc, hPen);
#define SetPenColor(border)                               \
    SelectObject(dc, prevPen);                             \
    DeleteObject(hPen);                                     \
    hPen = CreatePen(PS_SOLID, 1, m_Scheme.GetColor(border); \
    SelectObject(dc, hPen)
#endif
    if (uFlags & BF_MIDDLE) {
        FillRect(dc, &rcInner, m_Scheme.GetBrush(COLOR_3DFACE));
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

void CDrawings::DrawFrameButton(WTL::CDCHandle dc, CRect& rcParam, UINT uState) const
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
    const int     width{rcSrc.right - rcSrc.left};
    const int    height{rcSrc.bottom - rcSrc.top};
    const int smallDiam{((width > height) ? height : width)};
    CRect         rcDst{rcSrc};
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

void CDrawings::DrawFrameCaption(WTL::CDCHandle dc, CRect& rcParam, UINT uFlags)
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
    CRect rcSymbol = MakeSquareRect(rcParam);
    rcSymbol.DeflateRect(2, 2);
    if (uFlags & DFCS_PUSHED) {
        OffsetRect(&rcSymbol, 1, 1);
    }
    if (0 == symbol) {
        return ;
    }
    if (!m_ftMarlett.m_hFont) {
        m_ftMarlett = CStaticRes::CreateMarlettFont(rcSymbol.Height());
    }
    const HFONT prevFont = dc.SelectFont(m_ftMarlett);
    const int   prevMode = dc.SetBkMode(TRANSPARENT);
    if (uFlags & DFCS_INACTIVE) { // Draw shadow
        SetTextColor(dc, m_Scheme.GetColor(COLOR_3DHILIGHT));
        TextOut(dc, rcSymbol.left, rcSymbol.top, &symbol, 1);
    }
    SetTextColor(dc, m_Scheme.GetColor((uFlags & DFCS_INACTIVE) ? COLOR_3DSHADOW : COLOR_BTNTEXT));
    TextOut(dc, rcSymbol.left, rcSymbol.top, &symbol, 1);
    dc.SetBkMode(prevMode);
    dc.SelectFont(prevFont);
}

void CDrawings::DrawFrameScroll(WTL::CDCHandle dc, CRect& rcParam, UINT uFlags)
{
    TCHAR symbol = 0;
    switch (uFlags & 0xff) {
    case DFCS_SCROLLCOMBOBOX:
    case DFCS_SCROLLDOWN:  symbol = TEXT('6'); break;
    case DFCS_SCROLLUP:    symbol = TEXT('5'); break;
    case DFCS_SCROLLLEFT:  symbol = TEXT('3'); break;
    case DFCS_SCROLLRIGHT: symbol = TEXT('4'); break;
    default: break;
    }
    DrawEdge(dc, rcParam,
        (uFlags & DFCS_PUSHED) ? EDGE_SUNKEN : EDGE_RAISED,
        (uFlags & DFCS_FLAT) | BF_MIDDLE | BF_RECT);
    CRect rcSymbol = MakeSquareRect(rcParam);
    rcSymbol.DeflateRect(2, 2);
    if (uFlags & DFCS_PUSHED) {
        OffsetRect(rcSymbol, 1, 1);
    }
    if (0 == symbol) {
        return ;
    }
    auto fnMarlett = CStaticRes::CreateMarlettFont(rcSymbol.bottom - rcSymbol.top);
    m_ftMarlett.Attach(fnMarlett.Detach());
    const HFONT prevFont = dc.SelectFont(m_ftMarlett);
    const int   prevMode = dc.SetBkMode(TRANSPARENT);
    if (uFlags & DFCS_INACTIVE) {
        SetTextColor(dc, m_Scheme.GetColor(COLOR_3DHILIGHT));
        dc.TextOutW(rcSymbol.left, rcSymbol.top, &symbol, 1);
    }
    dc.SetTextColor(m_Scheme.GetColor((uFlags & DFCS_INACTIVE) ? COLOR_3DSHADOW : COLOR_BTNTEXT));
    dc.TextOutW(rcSymbol.left, rcSymbol.top, &symbol, 1);
    dc.SetBkMode(prevMode);
    dc.SelectFont(prevFont);
}


void CDrawings::DrawFrameControl(WTL::CDCHandle dc, CRect& rcParam, UINT uType, UINT uState)
{
    switch (uType) {
    case DFC_CAPTION: DrawFrameCaption(dc, rcParam, uState); break;
    case DFC_SCROLL:  DrawFrameScroll(dc, rcParam, uState); break;
    case DFC_BUTTON:  DrawFrameButton(dc, rcParam, uState); break;
    }
}

LONG CDrawings::DrawCaptionButtons(WTL::CDCHandle dc, CRect const& rcCaption, bool withMinMax, UINT uFlags)
{
    static constexpr int margin = 2;
    int             buttonWidth = m_SizePair.m_NCMetric.iCaptionWidth;
    buttonWidth -= margin;
    int iColor;
    if (uFlags & DC_GRADIENT) {
        if (uFlags & DC_ACTIVE) {
            iColor = COLOR_GRADIENTACTIVECAPTION;
        }
        else {
            iColor = COLOR_GRADIENTINACTIVECAPTION;
        }
    }
    else {
        if (uFlags & DC_ACTIVE) {
            iColor = COLOR_ACTIVECAPTION;
        }
        else {
            iColor = COLOR_INACTIVECAPTION;
        }
    }
    dc.FillRect(rcCaption, m_Scheme.GetBrush(iColor));

    CRect rc;
    rc.top = rcCaption.top + margin;
    rc.bottom = rcCaption.bottom - margin;
    rc.right = rcCaption.right - margin;
    rc.left = rc.right - buttonWidth;

    DrawFrameControl(dc, rc, DFC_CAPTION, DFCS_CAPTIONCLOSE);

    if (withMinMax) {
        rc.right = rc.left - margin;
        rc.left = rc.right - buttonWidth;
        DrawFrameControl(dc, rc, DFC_CAPTION, DFCS_CAPTIONMAX);

        rc.right = rc.left;
        rc.left = rc.right - buttonWidth;
        DrawFrameControl(dc, rc, DFC_CAPTION, DFCS_CAPTIONMIN);
    }
    return rc.left;
}

void CDrawings::DrawCaption(WTL::CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags) const
{
    CRect rcTmp = rcParam;
    int iColor1 = COLOR_INACTIVECAPTION;
    if (uFlags & DC_ACTIVE) {
        iColor1 = COLOR_ACTIVECAPTION;
    }
    if (hIcon) {
        const int        iconSize = GetSystemMetrics(SM_CYSMICON);  /* Same as SM_CXSMICON */
        const int totalIconMargin = rcTmp.bottom - rcTmp.top - iconSize;
        const int      iconMargin = totalIconMargin / 2;
        rcTmp.right = rcTmp.left + iconSize + totalIconMargin;
        dc.FillRect(rcTmp, m_Scheme.GetBrush(iColor1));
        if (DrawIconEx(dc, rcTmp.left + iconMargin + 1, rcTmp.top + iconMargin, hIcon, 0, 0, 0, nullptr, DI_NORMAL) != 0) {
            rcTmp.left = rcTmp.right;
        }
        rcTmp.right = rcParam.right;
    }
    if (uFlags & DC_GRADIENT) {
        GRADIENT_RECT gcap = { 0, 1 };
        TRIVERTEX  vert[2];
        COLORREF colors[2];
        colors[0] = m_Scheme.GetColor(iColor1);
        if (uFlags & DC_ACTIVE) {
            colors[1] = m_Scheme.GetColor(COLOR_GRADIENTACTIVECAPTION);
        }
        else {
            colors[1] = m_Scheme.GetColor(COLOR_GRADIENTINACTIVECAPTION);
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
        GdiGradientFill(dc, vert, 2, &gcap, 1, GRADIENT_FILL_RECT_H);
    }
    else {
        dc.FillRect(rcTmp, m_Scheme.GetBrush(iColor1));
    }
    const HFONT prevFont = dc.SelectFont(hFont);
    if (uFlags & DC_ACTIVE) {
        SetTextColor(dc, m_Scheme.GetColor(COLOR_CAPTIONTEXT));
    }
    else {
        SetTextColor(dc, m_Scheme.GetColor(COLOR_INACTIVECAPTIONTEXT));
    }
    rcTmp.left += 2;
    dc.SetBkMode(TRANSPARENT);
    dc.DrawTextW(str, -1, rcTmp, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    if (prevFont) {
        dc.SelectFont(prevFont);
    }
}

void CDrawings::DrawMenuText(WTL::CDCHandle dc, PCWSTR text, CRect& rc, UINT format, int color) const
{
    SetTextColor(dc, m_Scheme.GetColor(color));
    DrawTextW(dc, text, -1, rc, format);
}

void CDrawings::DrawDisabledMenuText(WTL::CDCHandle dc, PCWSTR text, CRect& rc, UINT format) const
{
    OffsetRect(rc, 1, 1);
    DrawMenuText(dc, text, rc, format, COLOR_3DHILIGHT);
    OffsetRect(rc, -1, -1);
    DrawMenuText(dc, text, rc, format, COLOR_3DSHADOW);
}

void CDrawings::DrawMenuBar(WTL::CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selIt, WindowRects& rects) const
{
    if (!hMenu || !hFont) {
        return ;
    }
    int  backColorIndex{COLOR_MENU};
    BOOL       bKbdCues{FALSE};
    int         spacing{10};
    int          margin{0};
    const HFONT prevFnt{dc.SelectFont(hFont)};
    UINT      txtFormat{DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP};
    CRect        rcItem{rc};
    CRect        rcText{};
    SIZE           size{};

    if (m_Scheme.IsFlatMenus()) {
        backColorIndex = COLOR_MENUBAR;
    }
    if (SystemParametersInfoW(SPI_GETKEYBOARDCUES, 0, &bKbdCues, 0) && !bKbdCues) {
        txtFormat |= DT_HIDEPREFIX;
    }
    TEXTMETRIC tm;
    if (dc.GetTextMetricsW(&tm)) {
        spacing = tm.tmAveCharWidth;
    }
    margin = spacing / 2;
    rcItem.bottom--;
    rcText.top = rcItem.top - 1;
    rcText.bottom = rcItem.bottom;

    dc.FillRect(rc, m_Scheme.GetBrush(backColorIndex));

    enum : int { MENU_ITEMS_TARGET_MAX = 3 };
    CRect* prcTarget[MENU_ITEMS_TARGET_MAX] = {
        &rects[WR_MenuItem], &rects[WR_MenuDisabled], &rects[WR_MenuSelected]
    };
    const auto itCount = std::min<int>(MENU_ITEMS_TARGET_MAX, GetMenuItemCount(hMenu));
    for (int iMenuItem = 0; iMenuItem < itCount; iMenuItem++) {
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
        const UINT state = GetMenuState(hMenu, iMenuItem, MF_BYPOSITION);
        if ((state & MF_GRAYED) || (state & MF_DISABLED)) {
            if ((m_Scheme.GetColor(COLOR_MENU) == m_Scheme.GetColor(COLOR_3DFACE)) || m_Scheme.IsFlatMenus()) {
                DrawDisabledMenuText(dc, text, rcText, txtFormat);
            }
            else {
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_GRAYTEXT);
            }
        }
        else if ((state & MF_HILITE) || (iMenuItem == selIt)) {
            if (m_Scheme.IsFlatMenus()) {
                FrameRect(dc, rcItem, m_Scheme.GetBrush(COLOR_HIGHLIGHT));
                InflateRect(rcItem, -1, -1);
                dc.FillRect(rcItem, m_Scheme.GetBrush(COLOR_MENUHILIGHT));
                InflateRect(rcItem, 1, 1);
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_HIGHLIGHTTEXT);
            }
            else {
                DrawEdge(dc, rcItem, BDR_SUNKENOUTER, BF_RECT);

                OffsetRect(&rcText, 1, 1);
                DrawMenuText(dc, text, rcText, txtFormat, COLOR_MENUTEXT);
            }
        }
        else {
            DrawMenuText(dc, text, rcText, txtFormat, COLOR_MENUTEXT);
        }
        *(prcTarget[iMenuItem]) = rcItem;
        rcItem.left = rcItem.right;
    }
    dc.SelectFont(prevFnt);
}

void CDrawings::DrawScrollbar(WTL::CDCHandle dc, CRect const& rcParam, BOOL enabled)
{
    CRect                     rc{};
    int             buttonHeight{m_SizePair.m_NCMetric.iScrollHeight};
    const UINT frameControlFlags{static_cast<UINT>(enabled ? 0 : DFCS_INACTIVE)};
    const HBRUSH     hbrScrollBk{m_Scheme.GetBrush(COLOR_SCROLLBAR)};
    rc.left = rcParam.left;
    rc.right = rcParam.right;
    if (rcParam.bottom - rcParam.top - buttonHeight * 2 <= 0) {
        buttonHeight = (rcParam.bottom - rcParam.top) / 2;
    }
    if (buttonHeight >= 5) {
        rc.top = rcParam.top;
        rc.bottom = rcParam.top + buttonHeight;
        DrawFrameControl(dc, rc, DFC_SCROLL, DFCS_SCROLLUP | frameControlFlags);
        rc.top = rcParam.bottom - buttonHeight;
        rc.bottom = rcParam.bottom;
        DrawFrameControl(dc, rc, DFC_SCROLL, DFCS_SCROLLDOWN | frameControlFlags);
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
    && (m_Scheme.GetColor(COLOR_SCROLLBAR) != m_Scheme.GetColor(COLOR_3DHILIGHT)
     || m_Scheme.GetColor(COLOR_WINDOW) == m_Scheme.GetColor(COLOR_SCROLLBAR))) {
        dc.SetTextColor(m_Scheme.GetColor(COLOR_3DFACE));
        const COLORREF previousColor = dc.SetBkColor(m_Scheme.GetColor(COLOR_3DHILIGHT));
        if (previousColor == CLR_INVALID) {
            dc.FillRect(rc, m_Scheme.GetBrush(COLOR_SCROLLBAR));
        }
        else {
            dc.FillRect(rc, hbrScrollBk);
            dc.SetBkColor(previousColor);
        }
    }
    else {
        dc.FillRect(rc, m_Scheme.GetBrush(COLOR_SCROLLBAR));
    }
}

void CDrawings::DrawToolTip(WTL::CDCHandle dc, CRect& rcParam, ATL::CStringW&& tooltip) const
{
    CSize  szText;
    CRect& rcText = rcParam;
    if (rcText.left < 0) {
        return ;
    }
    const int   prevMode = SetBkMode(dc, TRANSPARENT);
    const HFONT prevFont = dc.SelectFont(m_SizePair.GetFont(FONT_Tooltip));
    if (!GetTextExtentPoint32(dc, tooltip.GetString(), tooltip.GetLength(), &szText)) {
        szText.cx = ScaleForDpi(45);
        szText.cy = ScaleForDpi(14);
    }
    rcText.top    = rcText.bottom - szText.cy;
    rcText.right  = rcText.left   + szText.cx;
    rcText.InflateRect(3, 3);
    DrawBorder(dc, rcText, 1, m_Scheme.GetBrush(COLOR_INFOTEXT));
    InflateRect(rcText, -1, -1);
    FillRect(dc, rcText, m_Scheme.GetBrush(COLOR_INFOBK));
    SetTextColor(dc, m_Scheme.GetColor(COLOR_INFOTEXT));
    dc.DrawTextW(tooltip.GetString(), tooltip.GetLength(), rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    dc.SetBkMode(prevMode);
    dc.SelectFont(prevFont);
}

#define IsDarkColor(color) \
    ((GetRValue(color) * 2 + GetGValue(color) * 5 + GetBValue(color)) <= 128 * 8)

void CDrawings::DrawDesktopIcon(WTL::CDCHandle dc, CRect& rcParam, ATL::CStringW&& text, bool drawCursor) const
{
    ATLASSERT(m_pStaticRes.get() != nullptr);
    const HICON hIcon{m_pStaticRes->m_hIcon[CStaticRes::ICON_Desktop1]};
    //FillRect(dc, rcParam, m_Scheme.GetBrush(COLOR_APPWORKSPACE)); // DEBUG
    CRect rcIcon{0, 0, 64, 64}; // ##TODO: get desktop icon dimensions
    Rc::PutInto(rcParam, rcIcon, Rc::Center);
    rcParam = rcIcon;
    dc.DrawIconEx(rcIcon.TopLeft(), hIcon, rcIcon.Size(), 0, nullptr, DI_NORMAL);
    CSize szText;
    CRect rcText = rcIcon;
    rcText.DeflateRect(4, 4);
    if (rcText.left < 0) {
        return ;
    }
    const int   prevMode = SetBkMode(dc, TRANSPARENT);
    const HFONT prevFont = dc.SelectFont(m_SizePair.GetFont(FONT_Desktop));
    if (!GetTextExtentPoint32(dc, text.GetString(), text.GetLength(), &szText)) {
        szText.cx = ScaleForDpi(45);
        szText.cy = ScaleForDpi(14);
    }
    rcText.top = rcIcon.bottom + 4;
    rcText.bottom = rcText.top + szText.cy + 4;
    COLORREF     clrDesiredTextColor = RGB(0xFF, 0xFF, 0xFF);
    constexpr COLORREF clrTextShadow = RGB(0, 0, 0);;
    bool bThemedTextOut = false;
#if 0
    bThemedTextOut = 0 != DrawShadowText(dc, text.GetString(), text.GetLength(), rcText,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS, clrDesiredTextColor, clrShadow, 3, 3);
#else
    DTTOPTS dtOptions = {
        /* dwSize;              */ sizeof(dtOptions),
        /* dwFlags;             */ DTT_TEXTCOLOR | DTT_SHADOWCOLOR | DTT_SHADOWTYPE | DTT_SHADOWOFFSET | DTT_GLOWSIZE,
        /* crText;              */ clrDesiredTextColor,
        /* crBorder;            */ 0,
        /* crShadow;            */ clrTextShadow,
        /* iTextShadowType;     */ TST_CONTINUOUS,
        /* ptShadowOffset;      */ { 3, 2 },
        /* iBorderSize;         */ 0,
        /* iFontPropId;         */ 0,
        /* iColorPropId;        */ 0,
        /* iStateId;            */ 0,
        /* fApplyOverlay;       */ FALSE,
        /* iGlowSize;           */ 32,
        /* pfnDrawTextCallback; */ nullptr,
        /* lParam;              */ 0
    };
    bThemedTextOut = m_pStaticRes->
        m_InTheme.TextPut(dc, rcText, text.GetString(), text.GetLength(), 0, 0,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS,
            &dtOptions);
#endif
    if (!bThemedTextOut) {
        CRect rcShadow = rcText;
        for (int i=0; i<3; i++) {
            int c = i * 50;
            dc.SetTextColor(RGB(c, c, c));
            OffsetRect(rcShadow, 1, 1);
            dc.DrawTextW(text.GetString(), text.GetLength(), rcShadow, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        }
        SetTextColor(dc, clrDesiredTextColor);
        dc.DrawTextW(text.GetString(), text.GetLength(), rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    }
    dc.SelectFont(prevFont);
    dc.SetBkMode(prevMode);
    if (drawCursor) {
        const HICON hCursor = m_pStaticRes->m_hIcon[CStaticRes::ICON_Cursor1];
        CRect rcCursor{rcIcon.right + 8, rcIcon.bottom - 16, 0, 0};
        rcCursor.right = rcCursor.left + 32; // ##TODO: GetSystemMetrics for cursor painting
        rcCursor.bottom = rcCursor.top + 32;
        dc.DrawIconEx(rcCursor.TopLeft(), hCursor, rcCursor.Size(), 0, nullptr, DI_NORMAL);
    }
}

void CDrawings::DrawWindow(WTL::CDCHandle dc, DrawWindowArgs const& params, WindowRects& rects)
{
    ATLASSERT(m_pStaticRes.get() != nullptr);

    HICON const*      icons = m_pStaticRes->GetIcons();
    const HFONT    menuFont = m_SizePair.GetFont(FONT_Menu);
    const HFONT    captFont = m_SizePair.GetFont(FONT_Caption);
    HICON          captIcon = nullptr;
    UINT          captFlags = params.captFlags | DC_TEXT;
    int workspaceColorIndex = COLOR_APPWORKSPACE;
    int    borderColorIndex = COLOR_INACTIVEBORDER;
    const bool    isToolWnd = (0 != (DC_SMALLCAP & params.captFlags));
    const bool     isActive = (0 != (DC_ACTIVE & params.captFlags));

    m_ftMarlett.Attach(nullptr);

    if (!isToolWnd) {
        if (isActive) { captIcon = icons[CStaticRes::ICON_ActiveWnd]; }
        else          { captIcon = icons[CStaticRes::ICON_InactiveWnd]; }
        if (captIcon) { captFlags |= DC_ICON; }
    }
    if (m_Scheme.IsGradientCaptions()) {
        captFlags |= DC_GRADIENT;
    }
    if (isActive) {
        borderColorIndex    = COLOR_ACTIVEBORDER;
        workspaceColorIndex = COLOR_WINDOW;
    }

    if (isToolWnd) {
        static constexpr int nmbBorder = 4;
        CRect rcBrd = rects[WR_Frame];
        rcBrd.InflateRect(nmbBorder, nmbBorder);
        DrawBorder(dc, rcBrd, nmbBorder, m_Scheme.GetBrush(COLOR_3DFACE));
        DrawEdge(dc, rcBrd, EDGE_RAISED, BF_RECT | BF_ADJUST);
    }
    else {
        CRect rcBrd = rects[WR_Border];
        DrawBorder(dc, rcBrd, m_BorderSize, m_Scheme.GetBrush(borderColorIndex));
        DrawEdge(dc, rcBrd, EDGE_RAISED, BF_RECT | BF_ADJUST);
        dc.FillRect(rcBrd, m_Scheme.GetBrush(borderColorIndex));
    }
    dc.FillRect(rects[WR_Frame], m_Scheme.GetBrush(COLOR_3DFACE));

    CRect rcCapt = rects[WR_Caption];
    rcCapt.right = DrawCaptionButtons(dc, rcCapt, !isToolWnd, captFlags);
    DrawCaption(dc, rcCapt, captFont, captIcon, params.caption, captFlags);

    if (!isToolWnd) {
        if (params.hMenu) {
            DrawMenuBar(dc, rects[WR_Menu], params.hMenu, menuFont, params.selectedMenu, rects);
        }
        const COLORREF clrWinBk = m_Scheme.GetColor(workspaceColorIndex);
        CRect rcWork = rects[WR_Workspace];
        dc.FillSolidRect(rcWork, clrWinBk);
        DrawEdge(dc, rcWork, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

        if (params.text.lineCount > 0) {
            const int  textColorIndex{isActive ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT};
            const COLORREF clrWinText{m_Scheme.GetColor(textColorIndex)};
            const HFONT       prevFnt{dc.SelectFont(m_SizePair.GetFont(FONT_Desktop))};
            const COLORREF    prevClr{dc.SetTextColor(clrWinText == clrWinBk ? RGB(GetRValue(clrWinText) - 16, GetGValue(clrWinText) - 16, GetBValue(clrWinText) - 16) : clrWinText)};
            const COLORREF  prevBkClr{dc.SetBkColor(m_Scheme.GetColor(workspaceColorIndex))};
            const int      prevBkMode{dc.SetBkMode(TRANSPARENT)};
            CRect              rcText{rcWork};
            rcText.right = rects[WR_Scroll].left - 1;
            rcText.DeflateRect(rcWork.Width() / 16, 10);
            CRect  rcLine = rcText;
            const LONG cy = FontPtToLog<LONG>(m_SizePair.GetLogFont(FONT_Desktop).lfHeight);
            rcLine.bottom = rcLine.top + cy;
            for (int i = 0; i < params.text.lineCount; i++) {
                if (rcLine.top > rcText.bottom) {
                    break;
                }
                if (rcLine.bottom > rcText.bottom) {
                    rcLine.bottom = rcText.bottom;
                }
                const auto& line = params.text.line[i];
                if (line.flags & WT_Select) {
                    CRect rcSel = rcLine;
                    Rc::ShrinkX(rcSel, -2);
                    Rc::OffsetY(rcSel, 1);
                    dc.FillSolidRect(rcSel, m_Scheme.GetColor(COLOR_HIGHLIGHT));
                    dc.SetTextColor(m_Scheme.GetColor(COLOR_HIGHLIGHTTEXT));
                }
                else {
                    dc.SetTextColor(m_Scheme.GetColor(textColorIndex));
                }
                dc.DrawTextW(line.text.GetString(), line.text.GetLength(), rcLine, params.text.flags);
                rcLine.top = rcLine.bottom;
                rcLine.bottom = rcLine.top + cy;
            }
            dc.SetBkMode(prevBkMode);
            dc.SetTextColor(prevBkClr);
            dc.SetTextColor(prevClr);
            dc.SelectFont(prevFnt);
            rects[WR_WinText] = rcText;
        }
        DrawScrollbar(dc, rects[WR_Scroll], isActive);
    }
    else {
        if (params.text.lineCount < 3) {
            return;
        }
        CRect rc = rects[WR_Message];
        if (rc.top >= rc.bottom) {
            return ;
        }
        ATL::CStringW const& line0 = params.text.line[0].text;
        ATL::CStringW const& line1 = params.text.line[1].text;
        ATL::CStringW const& line2 = params.text.line[2].text;
        const int prevBkMode = dc.SetBkMode(TRANSPARENT);
        HFONT prevFont = dc.SelectFont(m_SizePair.GetFont(FONT_Message));
        SetTextColor(dc, m_Scheme.GetColor(COLOR_WINDOWTEXT));
        dc.DrawTextW(line0.GetString(), line0.GetLength(), rc, DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        dc.SelectFont(prevFont);
        rc = rects[WR_Hyperlink];
        if (rc.top >= rc.bottom) {
            return ;
        }
        prevFont = dc.SelectFont(m_SizePair.GetFont(FONT_Hyperlink));
        SetTextColor(dc, m_Scheme.GetColor(COLOR_HOTLIGHT));
        dc.DrawTextW(line1.GetString(), line1.GetLength(), rc, DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        dc.SelectFont(prevFont);
        rc = rects[WR_Button];
        if (rc.top >= rc.bottom) {
            return ;
        }
        InflateRect(rc, 1, 1);
        DrawBorder(dc, rc, 1, m_Scheme.GetBrush(COLOR_WINDOWFRAME));
        DrawFrameButton(dc, rc, DFCS_BUTTONPUSH);
        rc.bottom--;
        prevFont = dc.SelectFont(m_SizePair.GetFont(FONT_Message));
        dc.SetTextColor(m_Scheme.GetColor(COLOR_BTNTEXT));
        dc.DrawTextW(line2.GetString(), line2.GetLength(), rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
        dc.SelectFont(prevFont);
        dc.SetBkMode(prevBkMode);
    }
}

