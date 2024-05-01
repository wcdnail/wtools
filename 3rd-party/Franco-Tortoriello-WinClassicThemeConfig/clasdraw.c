/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Provides drawing functions for classic visual style previews.
 *
 * PROGRAMMERS: Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 *              Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "clasdraw.h"
#include "draw.h"
#include "mincrt.h"

static COLORREF *g_colors;
static HBRUSH *g_brushes;
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static BOOL g_bFlatMenus;
#endif

#if !defined(WITH_UNDOCUMENTED_API)

/*
 * Borders:
 * (None),            BDR_RAISEDOUTER,   BDR_SUNKENOUTER,   BDR_RAISEDOUTER|BDR_SUNKENOUTER,
 * BDR_RAISEDINNER,   EDGE_RAISED,       EDGE_ETCHED,       BDR_RAISEDOUTER|EDGE_ETCHED,
 * BDR_SUNKENINNER,   EDGE_BUMP,         EDGE_SUNKEN,       BDR_RAISEDOUTER|EDGE_SUNKEN,
 * BDR_SUNKENOUTER|EDGE_SUNKEN, BDR_RAISEDOUTER|BDR_SUNKENOUTER|EDGE_SUNKEN,
 *                    BDR_RAISEDINNER|EDGE_SUNKEN, BDR_RAISEDOUTER|BDR_RAISEDINNER|EDGE_SUNKEN
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

_Success_(return)
static
BOOL MyDrawFrameButton(HDC hdc, _Inout_ RECT *pRect, UINT uState)
{
    UINT edge;
    if (uState & (DFCS_PUSHED | DFCS_CHECKED | DFCS_FLAT))
        edge = EDGE_SUNKEN;
    else
        edge = EDGE_RAISED;

    return MyDrawEdge(
        hdc, pRect, edge, (uState & DFCS_FLAT) | BF_RECT | BF_SOFT | BF_MIDDLE);
}

static
void MyMakeSquareRect(_In_ const RECT *pSrc, _Out_ RECT *pDst)
{
    int width = pSrc->right - pSrc->left;
    int height = pSrc->bottom - pSrc->top;
    int smallDiam = ((width > height) ? height : width);

    *pDst = *pSrc;

    /* Make it a square box */
    if (width < height)       /* smallDiam == width */
    {
        pDst->top += (height - width) / 2;
        pDst->bottom = pDst->top + smallDiam;
    }
    else if (width > height)  /* smallDiam == height */
    {
        pDst->left += (width - height) / 2;
        pDst->right = pDst->left + smallDiam;
    }
}

_Success_(return != NULL)
static
HFONT CreateMarlettFont(LONG height)
{
    LOGFONT lf;

    if (!lstrcpy(lf.lfFaceName, TEXT("Marlett")))
        return NULL;

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

    return CreateFontIndirect(&lf);
}

_Success_(return)
static
BOOL MyDrawFrameCaption(HDC hdc, _Inout_ RECT *pRect, UINT uFlags)
{
    BOOL ret = TRUE;

    TCHAR symbol;
    switch (uFlags & 0xff)
    {
    case DFCS_CAPTIONCLOSE:
        symbol = TEXT('r');
        break;
#if 0
    case DFCS_CAPTIONHELP:
        symbol = TEXT('s');
        break;
#endif
    case DFCS_CAPTIONMIN:
        symbol = TEXT('0');
        break;
    case DFCS_CAPTIONMAX:
        symbol = TEXT('1');
        break;
#if 0
    case DFCS_CAPTIONRESTORE:
        symbol = TEXT('2');
        break;
#endif
    default:
        return FALSE;
    }
    ret &= MyDrawEdge(
        hdc, pRect, (UINT)((uFlags & DFCS_PUSHED) ? EDGE_SUNKEN : EDGE_RAISED),
        BF_RECT | BF_MIDDLE | BF_SOFT);

    RECT myRect;
    MyMakeSquareRect(pRect, &myRect);

    myRect.left += 2;
    myRect.top += 2;
    myRect.right -= 1;
    myRect.bottom -= 2;

    if (uFlags & DFCS_PUSHED)
        ret &= OffsetRect(&myRect, 1, 1);

    HFONT hFont = CreateMarlettFont(myRect.bottom - myRect.top);
    if (!hFont)
        return FALSE;

    HGDIOBJ hPreviousFont = SelectObject(hdc, hFont);
    if (!hPreviousFont)
    {
        DeleteObject(hFont);
        return FALSE;
    }

    if (uFlags & DFCS_INACTIVE)
    {
        /* Draw shadow */
        ret &= (SetTextColor(hdc, g_colors[COLOR_3DHILIGHT]) != CLR_INVALID);
        ret &= TextOut(hdc, myRect.left + 1, myRect.top + 1, &symbol, 1);
    }

    ret &= (SetTextColor(hdc, g_colors[
            (uFlags & DFCS_INACTIVE) ? COLOR_3DSHADOW : COLOR_BTNTEXT]) !=
            CLR_INVALID);

    /* Draw selected symbol */
    ret &= TextOut(hdc, myRect.left, myRect.top, &symbol, 1);

    SelectObject(hdc, hPreviousFont);
    DeleteObject(hFont);

    return ret;
}

_Success_(return)
static
BOOL MyDrawFrameScroll(HDC hdc, _Inout_ RECT *pRect, UINT uFlags)
{
    BOOL ret = TRUE;

    TCHAR symbol;
    switch (uFlags & 0xff)
    {
    case DFCS_SCROLLCOMBOBOX:
    case DFCS_SCROLLDOWN:
        symbol = TEXT('6');
        break;

    case DFCS_SCROLLUP:
        symbol = TEXT('5');
        break;

#if 0
    case DFCS_SCROLLLEFT:
        symbol = TEXT('3');
        break;

    case DFCS_SCROLLRIGHT:
        symbol = TEXT('4');
        break;
#endif

    default:
        return FALSE;
    }

    ret &= MyDrawEdge(hdc, pRect,
        (uFlags & DFCS_PUSHED) ? (UINT)EDGE_SUNKEN : (UINT)EDGE_RAISED,
        (uFlags & DFCS_FLAT) | BF_MIDDLE | BF_RECT);

    RECT myRect;
    MyMakeSquareRect(pRect, &myRect);

    myRect.left += 2;
    myRect.top += 2;
    myRect.right -= 2;
    myRect.bottom -= 2;

    if (uFlags & DFCS_PUSHED)
        ret &= OffsetRect(&myRect, 1, 1);

    HFONT hFont = CreateMarlettFont(myRect.bottom - myRect.top);
    if (!hFont)
        return FALSE;

    HGDIOBJ hPreviousFont = SelectObject(hdc, hFont);
    if (!hPreviousFont)
        return FALSE;

    if (uFlags & DFCS_INACTIVE)
    {
        /* Draw shadow */
        ret &= (SetTextColor(hdc, g_colors[COLOR_3DHILIGHT]) != CLR_INVALID);
        ret &= TextOut(hdc, myRect.left + 1, myRect.top + 1, &symbol, 1);
    }

    ret &= (SetTextColor(hdc, g_colors[
        (uFlags & DFCS_INACTIVE) ? COLOR_3DSHADOW : COLOR_BTNTEXT]) !=
        CLR_INVALID);

    /* Draw selected symbol */
    ret &= TextOut(hdc, myRect.left, myRect.top, &symbol, 1);

    SelectObject(hdc, hPreviousFont);
    DeleteObject(hFont);

    return ret;
}
#endif  /* !defined(WITH_UNDOCUMENTED_API) */

#if !defined(WITH_SYSTEM_DrawMenuBarTemp)
_Success_(return)
static
BOOL DrawMenuText(HDC hdc, _In_ const TCHAR *text, _Inout_ RECT *pRect,
    UINT format, int color)
{
    BOOL ret = TRUE;

    ret &= (SetTextColor(hdc, g_colors[color]) != CLR_INVALID);
    ret &= (DrawText(hdc, text, -1, pRect, format) > 0);

    return ret;
}

_Success_(return)
static
BOOL DrawDisabledMenuText(
    HDC hdc, _In_ const TCHAR *text, _Inout_ RECT *pRect, UINT format)
{
    BOOL ret = TRUE;

    ret &= OffsetRect(pRect, 1, 1);
    ret &= DrawMenuText(hdc, text, pRect, format, COLOR_3DHILIGHT);
    ret &= OffsetRect(pRect, -1, -1);
    ret &= DrawMenuText(hdc, text, pRect, format, COLOR_3DSHADOW);

    return ret;
}
#endif  /* !defined(WITH_SYSTEM_DrawMenuBarTemp)) */

void SetPreviewSettings(_In_ COLORREF *colors, _In_ HBRUSH *brushes
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    , BOOL bFlatMenus
#endif
)
{
    g_colors = colors;
    g_brushes = brushes;
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    g_bFlatMenus = bFlatMenus;
#endif
}

#if !defined(WITH_UNDOCUMENTED_API)
_Success_(return)
BOOL MyDrawFrameControl(HDC hdc, _Inout_ RECT *pRect, UINT uType, UINT uState)
{
    switch (uType)
    {
    case DFC_CAPTION:
        return MyDrawFrameCaption(hdc, pRect, uState);
    case DFC_SCROLL:
        return MyDrawFrameScroll(hdc, pRect, uState);
    case DFC_BUTTON:
        return MyDrawFrameButton(hdc, pRect, uState);
    }
    return FALSE;
}

/* Note: pRect can be modified with the flag BF_ADJUST */
_Success_(return)
BOOL MyDrawEdge(HDC hdc, _Inout_ RECT *pRect, UINT edge, UINT uFlags)
{
    BOOL ret = TRUE;
    RECT innerRect = *pRect;
    HPEN hPreviousPen = NULL;

    POINT savePoint;
    savePoint.x = 0;
    savePoint.y = 0;

    /* Determine the colors of the edges */
    unsigned char edgeIndex = edge & (BDR_INNER | BDR_OUTER);
    signed char LTInnerI, LTOuterI, RBInnerI, RBOuterI;
    if (uFlags & BF_SOFT)
    {
        LTInnerI = LTOuterNormal[edgeIndex];
        LTOuterI = LTInnerNormal[edgeIndex];
    }
    else
    {
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
    if (SetDCPenColor(hdc, g_colors[border]) == CLR_INVALID) goto Error

    hPreviousPen = (HPEN)SelectObject(hdc, GetStockObject(DC_PEN));
    if (!hPreviousPen)
        goto Error;
    SetPenColor(LTOuterI);

#else

    HPEN hPen = CreatePen(PS_SOLID, 1, g_colors[LTOuterI]);
    if (!hPen)
        goto Error;
    hPreviousPen = (HPEN)SelectObject(hdc, hPen);
    if (!hPreviousPen)
        goto Error;

#define SetPenColor(border) \
    SelectObject(hdc, hPreviousPen); \
    DeleteObject(hPen); \
    hPen = CreatePen(PS_SOLID, 1, g_colors[border]); \
    if (!hPen) goto Error; \
    if (!SelectObject(hdc, hPen)) goto Error

#endif  /* WINVER >= WINVER_2K && !defined(WINVER_IS_98) */

    if (uFlags & BF_MIDDLE)
        ret &= (FillRect(hdc, &innerRect, g_brushes[COLOR_3DFACE]) != 0);

    if (!MoveToEx(hdc, 0, 0, &savePoint))
        goto Error;

    /* Draw the outer edge */
    if (LTOuterI >= 0)
    {
        if (uFlags & BF_TOP)
        {
            if (!MoveToEx(hdc, innerRect.left, innerRect.top, NULL))
                goto Error;
            if (!LineTo(hdc, innerRect.right, innerRect.top))
                goto Error;
        }
        if (uFlags & BF_LEFT)
        {
            if (!MoveToEx(hdc, innerRect.left, innerRect.top, NULL))
                goto Error;
            if (!LineTo(hdc, innerRect.left, innerRect.bottom))
                goto Error;
        }
    }

    if (RBOuterI >= 0)
    {
        SetPenColor(RBOuterI);

        if (uFlags & BF_BOTTOM)
        {
            if (!MoveToEx(hdc, innerRect.left, innerRect.bottom - 1, NULL))
                goto Error;
            if (!LineTo(hdc, innerRect.right, innerRect.bottom - 1))
                goto Error;
        }
        if (uFlags & BF_RIGHT)
        {
            if (!MoveToEx(hdc, innerRect.right - 1, innerRect.top, NULL))
                goto Error;
            if (!LineTo(hdc, innerRect.right - 1, innerRect.bottom))
                goto Error;
        }
    }

    /* Draw the inner edge */
    if (LTInnerI >= 0)
    {
        SetPenColor(LTInnerI);

        if (uFlags & BF_TOP)
        {
            if (!MoveToEx(
                hdc, innerRect.left + iPenLTPlus, innerRect.top + 1, NULL))
            {
                goto Error;
            }
            if (!LineTo(hdc, innerRect.right - iPenRTPlus, innerRect.top + 1))
            {
                goto Error;
            }
        }
        if (uFlags & BF_LEFT)
        {
            if (!MoveToEx(
                hdc, innerRect.left + 1, innerRect.top + iPenLTPlus, NULL))
            {
                goto Error;
            }
            if (!LineTo(hdc, innerRect.left + 1, innerRect.bottom - iPenLBPlus))
            {
                goto Error;
            }
        }
    }

    if (RBInnerI >= 0)
    {
        SetPenColor(RBInnerI);

        if (uFlags & BF_BOTTOM)
        {
            if (!MoveToEx(
                hdc, innerRect.left + iPenLBPlus, innerRect.bottom - 2, NULL))
            {
                goto Error;
            }
            if (!LineTo(
                hdc, innerRect.right - iPenRBPlus, innerRect.bottom - 2))
            {
                goto Error;
            }
        }
        if (uFlags & BF_RIGHT)
        {
            if (!MoveToEx(
                hdc, innerRect.right - 2, innerRect.top + iPenRTPlus, NULL))
            {
                goto Error;
            }
            if (!LineTo(
                hdc, innerRect.right - 2, innerRect.bottom - iPenRBPlus))
            {
                goto Error;
            }
        }
    }

    if (uFlags & BF_ADJUST)
    {
        int add = (LTRBInnerMono[edgeIndex] >= 0 ? 1 : 0) +
            (LTRBOuterMono[edgeIndex] >= 0 ? 1 : 0);

        if (uFlags & BF_LEFT)
            innerRect.left += add;
        if (uFlags & BF_RIGHT)
            innerRect.right -= add;
        if (uFlags & BF_TOP)
            innerRect.top += add;
        if (uFlags & BF_BOTTOM)
            innerRect.bottom -= add;

        if (uFlags & BF_ADJUST)
            *pRect = innerRect;
    }

    MoveToEx(hdc, savePoint.x, savePoint.y, NULL);
    SelectObject(hdc, hPreviousPen);
#if WINVER < WINVER_2K
    DeleteObject(hPen);
#endif
    return ret;

Error:
    if (hPreviousPen)
        SelectObject(hdc, hPreviousPen);
#if WINVER < WINVER_2K
    if (hPen)
        DeleteObject(hPen);
#endif
    return FALSE;
}
#endif

#if !defined(WITH_SYSTEM_DrawCaptionTemp)
_Success_(return)
BOOL DrawCaptionTemp(HWND hWnd, HDC hdc, _In_ const RECT *pRect, HFONT hFont,
    HICON hIcon, _In_ const TCHAR *str, UINT uFlags)
{
    UNREFERENCED_PARAMETER(hWnd);

    BOOL ret = TRUE;
    RECT rcTmp = *pRect;
    int iColor1;

    if (uFlags & DC_ACTIVE)
        iColor1 = COLOR_ACTIVECAPTION;
    else
        iColor1 = COLOR_INACTIVECAPTION;

    if (hIcon)
    {
        int iconSize = GetSystemMetrics(SM_CYSMICON);  /* Same as SM_CXSMICON */
        int totalIconMargin = rcTmp.bottom - rcTmp.top - iconSize;
        int iconMargin = totalIconMargin / 2;

        /* Solid color under the icon. The gradient starts to the right. */
        rcTmp.right = rcTmp.left + iconSize + totalIconMargin;
        ret &= (FillRect(hdc, &rcTmp, g_brushes[iColor1]) != 0);

        if (DrawIconEx(hdc, rcTmp.left + iconMargin + 1, rcTmp.top + iconMargin,
            hIcon, 0, 0, 0, NULL, DI_NORMAL) != 0)
        {
            /* The icon was drawn */
            rcTmp.left = rcTmp.right;
        }

        rcTmp.right = pRect->right;
    }

#if WINVER >= WINVER_2K
    if (uFlags & DC_GRADIENT)
    {
        GRADIENT_RECT gcap = { 0, 1 };
        TRIVERTEX vert[2];
        COLORREF colors[2];

        colors[0] = g_colors[iColor1];
        if (uFlags & DC_ACTIVE)
            colors[1] = g_colors[COLOR_GRADIENTACTIVECAPTION];
        else
            colors[1] = g_colors[COLOR_GRADIENTINACTIVECAPTION];

        vert[0].x = rcTmp.left;
        vert[0].y = rcTmp.top;
        vert[0].Red = (COLOR16)(colors[0] << 8);
        vert[0].Green = (COLOR16)(colors[0] & 0xFF00);
        vert[0].Blue = (COLOR16)((colors[0] >> 8) & 0xFF00);
        vert[0].Alpha = 0;

        vert[1].x = rcTmp.right;
        vert[1].y = rcTmp.bottom;
        vert[1].Red = (COLOR16)(colors[1] << 8);
        vert[1].Green = (COLOR16)(colors[1] & 0xFF00);
        vert[1].Blue = (COLOR16)((colors[1] >> 8) & 0xFF00);
        vert[1].Alpha = 0;

#if defined(WINVER_IS_98)
        /* Win98 has GradientFill on Msimg32 only.
         * Later versions redirect to the implementation of GdiGradientFill on
         * Gdi32.
         */
        ret &= GradientFill(hdc, vert, 2, &gcap, 1, GRADIENT_FILL_RECT_H);
#else
        ret &= GdiGradientFill(hdc, vert, 2, &gcap, 1, GRADIENT_FILL_RECT_H);
#endif
    }
    else
#endif
    {
        ret &= (FillRect(hdc, &rcTmp, g_brushes[iColor1]) != 0);
    }

    HGDIOBJ hPreviousFont = SelectObject(hdc, hFont);
    if (!hPreviousFont)
        return FALSE;

    if (uFlags & DC_ACTIVE)
        ret &= (SetTextColor(
            hdc, g_colors[COLOR_CAPTIONTEXT]) != CLR_INVALID);
    else
        ret &= (SetTextColor(
            hdc, g_colors[COLOR_INACTIVECAPTIONTEXT]) != CLR_INVALID);

    rcTmp.left += 2;
    ret &= (DrawText(hdc, str, -1, &rcTmp,
        DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS) > 0);

    SelectObject(hdc, hPreviousFont);

    return ret;
}
#endif  /* defined(WITH_SYSTEM_DrawCaptionTemp) */

_Success_(return)
BOOL DrawMenuBarBackground(HDC hdc, RECT *pRect)
{
    int iColor;

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    if (g_bFlatMenus)
        iColor = COLOR_MENUBAR;
    else
#endif
        iColor = COLOR_MENU;

    return FillRect(hdc, pRect, g_brushes[iColor]) != 0;
}

#if !defined(WITH_SYSTEM_DrawMenuBarTemp)
_Success_(return == 0)
int DrawMenuBarTemp(
    HWND hWnd, HDC hdc, _In_ const RECT *pRect, HMENU hMenu, HFONT hFont)
{
#define MAX_MENUTEXT_LEN 30

    UNREFERENCED_PARAMETER(hWnd);

    if (!hMenu)
        return 1;

    if (!hFont)
        return 1;

    HGDIOBJ hPreviousFont = SelectObject(hdc, hFont);
    if (!hPreviousFont)
        return 1;

    BOOL ret = TRUE;
    RECT rcItem = *pRect;
    RECT rcText;
    TCHAR text[MAX_MENUTEXT_LEN];
    SIZE size;

    UINT textFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;

#if WINVER >= WINVER_2K
    BOOL bKeyboardCues = FALSE;
    if (SystemParametersInfo(SPI_GETKEYBOARDCUES, 0, &bKeyboardCues, 0) &&
        !bKeyboardCues)
    {
        textFormat |= DT_HIDEPREFIX;
    }
#endif

    /* Menu spacing */
    int spacing;
    TEXTMETRIC tm;
    if (GetTextMetrics(hdc, &tm))
        spacing = tm.tmAveCharWidth;
    else
        spacing = 10;
    int margin = spacing / 2;

    /* Exclude the bottom border from the rect */
    rcItem.bottom--;

    DrawMenuBarBackground(hdc, &rcItem);

    rcText.top = rcItem.top - 1;
    rcText.bottom = rcItem.bottom;

    int menuItemCount = GetMenuItemCount(hMenu);
    for (int iMenuItem = 0; iMenuItem < menuItemCount; iMenuItem++)
    {
        if (!GetMenuString(
            hMenu, iMenuItem, text, MAX_MENUTEXT_LEN, MF_BYPOSITION))
        {
            SelectObject(hdc, hPreviousFont);
            return 1;
        }

        if (!GetTextExtentPoint32(hdc, text, lstrlen(text), &size))
        {
            SelectObject(hdc, hPreviousFont);
            return 1;
        }

        rcItem.right = rcItem.left + size.cx + spacing;
        if (rcItem.right > (pRect->right - margin))
        {
            /* No space left */
            SelectObject(hdc, hPreviousFont);
            return 0;
        }

        rcText.left = rcItem.left + margin;
        rcText.right = rcItem.right - margin;

        UINT state = GetMenuState(hMenu, iMenuItem, MF_BYPOSITION);
        if (state & MF_GRAYED)
        {
            /* The system renders it with or without a shadow according to
             * this condition, for some reason...
             */
            if ((g_colors[COLOR_MENU] == g_colors[COLOR_3DFACE])
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
                || g_bFlatMenus
#endif
            )
            {
                ret &= DrawDisabledMenuText(hdc, text, &rcText, textFormat);
            }
            else
            {
                ret &= DrawMenuText(
                    hdc, text, &rcText, textFormat, COLOR_GRAYTEXT);
            }
        }
        else if (state & MF_HILITE)
        {
#if WINVER >= WINVER_2K
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
            if (g_bFlatMenus)
            {
                /* Frame */
                ret &= FrameRect(hdc, &rcItem, g_brushes[COLOR_HIGHLIGHT]);

                ret &= InflateRect(&rcItem, -1, -1);
                ret &= (FillRect(
                    hdc, &rcItem, g_brushes[COLOR_MENUHILIGHT]) != 0);
                ret &= InflateRect(&rcItem, 1, 1);

                ret &= DrawMenuText(
                    hdc, text, &rcText, textFormat, COLOR_HIGHLIGHTTEXT);
            }
            else
#endif
            {
                ret &= MyDrawEdge(hdc, &rcItem, BDR_SUNKENOUTER, BF_RECT);

                ret &= OffsetRect(&rcText, 1, 1);
                ret &= DrawMenuText(
                    hdc, text, &rcText, textFormat, COLOR_MENUTEXT);
            }
#else  /* WINVER < WINVER_2K */
            ret &= FillRect(hdc, &rcItem, g_brushes[COLOR_HIGHLIGHT]);
            ret &= DrawMenuText(
                hdc, text, &rcText, textFormat, COLOR_HIGHLIGHTTEXT);
#endif
        }
        else
        {
            ret &= DrawMenuText(hdc, text, &rcText, textFormat, COLOR_MENUTEXT);
        }

        rcItem.left = rcItem.right;
    }

    SelectObject(hdc, hPreviousFont);

    return !ret;

#undef MAX_MENUTEXT_LEN
}
#endif  /* !defined(WITH_SYSTEM_DrawMenuBarTemp) */

_Success_(return)
BOOL DrawBottomBorder(HDC hdc, _In_ const RECT *pRect)
{
    RECT rcBorder;
    rcBorder.left = pRect->left;
    rcBorder.right = pRect->right;
    rcBorder.top = pRect->bottom;
    rcBorder.bottom = pRect->bottom + 1;

    return FillRect(hdc, &rcBorder, g_brushes[COLOR_3DFACE]) != 0;
}

_Success_(return)
BOOL DrawBorderedWindow(
    HDC hdc, _In_ const RECT *prcOrig, int borderSize, int iBorderColor)
{
    BOOL ret = TRUE;
    RECT rc;
    rc.left = prcOrig->left;
    rc.right = prcOrig->right;
    rc.top = prcOrig->top;
    rc.bottom = prcOrig->bottom;

    /* 2 px outer edge */
    ret &= MyDrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT | BF_ADJUST);

    /* External border */
    ret &= DrawBorder(hdc, &rc, borderSize, g_brushes[iBorderColor]);

    /* Inner edge */
    ret &= InflateRect(&rc, -borderSize + 1, -borderSize + 1);
    ret &= DrawBorder(hdc, &rc, 1, g_brushes[COLOR_3DFACE]);

    return ret;
}

_Success_(return)
BOOL DrawCaptionButtons(HDC hdc, _In_ const RECT *prcCaption,
    BOOL withMinMax, int buttonWidth, UINT uFlags)
{
    BOOL ret = TRUE;

    static const int margin = 2;
    buttonWidth -= margin;

    /* Background (always solid) */
    int iColor;
#if WINVER >= WINVER_2K
    if (uFlags & DC_GRADIENT)
    {
        if (uFlags & DC_ACTIVE)
            iColor = COLOR_GRADIENTACTIVECAPTION;
        else
            iColor = COLOR_GRADIENTINACTIVECAPTION;
    }
    else
#endif
    {
        if (uFlags & DC_ACTIVE)
            iColor = COLOR_ACTIVECAPTION;
        else
            iColor = COLOR_INACTIVECAPTION;
    }
    ret &= (FillRect(hdc, prcCaption, g_brushes[iColor]) != 0);

    RECT rc;
    rc.top = prcCaption->top + margin;
    rc.bottom = prcCaption->bottom - margin;

    /* Close */
    rc.right = prcCaption->right - margin;
    rc.left = rc.right - buttonWidth;
    ret &= MyDrawFrameControl(hdc, &rc, DFC_CAPTION, DFCS_CAPTIONCLOSE);

    if (withMinMax)
    {
        /* Maximize */
        rc.right = rc.left - margin;
        rc.left = rc.right - buttonWidth;
        ret &= MyDrawFrameControl(hdc, &rc, DFC_CAPTION, DFCS_CAPTIONMAX);

        /* Minimize */
        rc.right = rc.left;
        rc.left = rc.right - buttonWidth;
        ret &= MyDrawFrameControl(hdc, &rc, DFC_CAPTION, DFCS_CAPTIONMIN);
    }

    return ret;
}

_Success_(return)
BOOL DrawScrollbar(
    HDC hdc, _In_ const RECT *pRect, int buttonHeight
#if defined(WITH_NEW_PREVIEW)
    , HBRUSH hbrScrollBk, BOOL enabled
#endif
)
{
    BOOL ret = TRUE;
    RECT rc;
#if defined(WITH_NEW_PREVIEW)
    UINT frameControlFlags = (enabled ? 0 : DFCS_INACTIVE);
#else
    const UINT frameControlFlags = 0;
#endif

    rc.left = pRect->left;
    rc.right = pRect->right;

    /* Buttons */

    if (pRect->bottom - pRect->top - buttonHeight * 2 <= 0)
        buttonHeight = (pRect->bottom - pRect->top) / 2;

    if (buttonHeight >= 5)
    {
        rc.top = pRect->top;
        rc.bottom = pRect->top + buttonHeight;
        ret &= MyDrawFrameControl(hdc, &rc, DFC_SCROLL,
            DFCS_SCROLLUP | frameControlFlags);

        rc.top = pRect->bottom - buttonHeight;
        rc.bottom = pRect->bottom;
        ret &= MyDrawFrameControl(hdc, &rc, DFC_SCROLL,
            DFCS_SCROLLDOWN | frameControlFlags);
    }
    else
        buttonHeight = 0;

    /* Background */

    rc.top = pRect->top + buttonHeight;
    rc.bottom = pRect->bottom - buttonHeight;
    if (rc.top >= rc.bottom)
        return ret;

#if defined(WITH_NEW_PREVIEW)
    /* The configured scrollbar color is only used by the OS for the scrollbar
     * background IF it is the same as the first 3D light color, for some
     * obscure reason...
     * Except when it also matches the window color (to be able to distinguish
     * it)
     */
    if (hbrScrollBk &&
        (g_colors[COLOR_SCROLLBAR] != g_colors[COLOR_3DHILIGHT] ||
            g_colors[COLOR_WINDOW] == g_colors[COLOR_SCROLLBAR]))
    {
        /* Set colors and draw checkered pattern */
        ret &= (SetTextColor(hdc, g_colors[COLOR_3DFACE]) != CLR_INVALID);
        COLORREF previousColor = SetBkColor(hdc, g_colors[COLOR_3DHILIGHT]);
        if (previousColor == CLR_INVALID)
        {
            ret = FALSE;
            FillRect(hdc, &rc, g_brushes[COLOR_SCROLLBAR]);
        }
        else
        {
            ret &= (FillRect(hdc, &rc, hbrScrollBk) != 0);
            SetBkColor(hdc, previousColor);
        }
    }
    else
#endif
        ret &= (FillRect(hdc, &rc, g_brushes[COLOR_SCROLLBAR]) != 0);

    return ret;
}
