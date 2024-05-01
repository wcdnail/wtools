/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Preview window drawing
 *
 * PROGRAMMERS: Eric Kohl
 *              Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "clasdraw.h"
#include "draw.h"
#include "classtyl.h"
#include "mincrt.h"
#include "preview.h"
#include "reg.h"
#include "resource.h"
#include "util.h"
#include "wndclas.h"

#include <windowsx.h>
#include <shellapi.h>

static const TCHAR schemePreviewWndClass[] = TEXT(SCHEMEPREVIEWWND_CLASS);

#if defined(WITH_NEW_PREVIEW)
static const TCHAR classicPreviewRegValue[] = TEXT("ClassicPreview");
#endif

/* GetSystemMetrics(SM_CXEDGE) */
static const int edgeSize = 2;

/* SM_CXBORDER = SM_CXFIXEDFRAME - SM_CXEDGE */
static const int innerBorderSize = 1;

#if defined(WITH_UNDOCUMENTED_API)
static HMODULE g_hUser32 = NULL;
#endif

#if WINVER >= WINVER_XP
static BOOL g_bIconLabelShadows = TRUE;
#endif

#if defined(WITH_UNDOCUMENTED_API)
static DWORD_PTR(WINAPI *SetSysColorsTemp)(
    const COLORREF *pPens, const HBRUSH *pBrushes, DWORD_PTR n) = NULL;
#endif

#if defined(WITH_SYSTEM_DrawCaptionTemp)
static BOOL(WINAPI *DrawCaptionTemp)(
    HWND hWnd, HDC hdc, const RECT *pRect, HFONT hFont, HICON hIcon, TCHAR *str,
    UINT uFlags) = NULL;
#endif

#if defined(WITH_SYSTEM_DrawMenuBarTemp)
static int(WINAPI *DrawMenuBarTemp)(
    HWND hWnd, HDC hdc, RECT *pRect, HMENU hMenu, HFONT hFont) = NULL;
#endif

typedef struct tagPREVIEW_DATA
{
    HDC hdcPreview;
    HBITMAP hbmpPreview;
    HBITMAP hbmpPrevious;

    HBRUSH brushes[NUM_COLORS];
    HBRUSH hbrScrollbarPattern;

    SCHEME_DATA scheme;
    BOOL bPainting;

    RECT rcDesktop;

#if defined(WITH_NEW_PREVIEW_BIG)
    RECT rcMdiFrame;
    RECT rcMdiCaption;
    RECT rcMdiWorkspace;
#endif
    RECT rcMenuBar;

    RECT rcInactiveFrame;
    RECT rcInactiveCaption;
#if defined(WITH_NEW_PREVIEW_BIG)
    RECT rcInactiveClient;
    RECT rcInactiveScrollBar;
#endif

    RECT rcActiveFrame;
    RECT rcActiveCaption;
    RECT rcActiveClient;
#if defined(WITH_NEW_PREVIEW)
    RECT rcSelectedText;
#endif
    RECT rcActiveScrollBar;

    RECT rcMessageFrame;
    RECT rcMessageCaption;
    RECT rcMessageClient;
#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    RECT rcHyperlink;
#endif
    RECT rcMessageButton;

#if defined(WITH_NEW_PREVIEW)
    RECT rcToolTip;
#endif

    TCHAR *inactiveCaption;
    TCHAR *activeCaption;
    TCHAR *windowText;
    TCHAR *messageCaption;
    TCHAR *messageText;
    TCHAR *okText;

#if defined(WITH_NEW_PREVIEW)
    TCHAR *normalText;
    TCHAR *selectedText;
    TCHAR *paletteCaption;
    TCHAR *buttonText;
    TCHAR *toolTipText;
    TCHAR *iconLabel;
#endif
#if defined(WITH_NEW_PREVIEW_BIG)
    TCHAR *mainCaption;
    TCHAR *disabledText;
#if WINVER >= WINVER_2K
    TCHAR *hyperlinkText;
#endif
#endif

    HFONT hFontCaption;
    HFONT hFontMenu;
    HFONT hFontMessage;
#if defined(WITH_NEW_PREVIEW)
    HFONT hFontSmCaption;
    HFONT hFontToolTip;
    HFONT hFontDesktop;
    HFONT hFontWindow;
#endif
#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    HFONT hFontHyperlink;
#endif

    HMENU hMenu;

#if defined(WITH_NEW_PREVIEW)
    HICON hCaptionIcon;
    BOOL bClassicPreview;
#endif

    int borderSize;
#if WINVER >= WINVER_VISTA
    int paddedBorderSize;
#endif
    int captionWidth;
    int captionHeight;
#if defined(WITH_NEW_PREVIEW)
    int smCaptionWidth;
    int smCaptionHeight;
#endif
    int menuHeight;
    int scrollBarWidth;
    int scrollButtonSize;
} PREVIEW_DATA;

#if defined(WITH_UNDOCUMENTED_API)
static
void UnloadSystemDrawFunctions(void)
{
    FreeLibrary(g_hUser32);
    g_hUser32 = NULL;
}

_Success_(return)
static
BOOL LoadSystemDrawFunctions(void)
{
    g_hUser32 = LoadLibrary(TEXT("user32.dll"));
    if (!g_hUser32)
        return FALSE;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

    *(FARPROC *)&SetSysColorsTemp =
        GetProcAddress(g_hUser32, "SetSysColorsTemp");
    if (!SetSysColorsTemp)
    {
        UnloadSystemDrawFunctions();
        return FALSE;
    }

#if defined(WITH_SYSTEM_DrawCaptionTemp)
    *(FARPROC *)&DrawCaptionTemp = GetProcAddress(g_hUser32,
#if defined(UNICODE)
        "DrawCaptionTempW"
#else
        "DrawCaptionTempA"
#endif
    );
    if (!DrawCaptionTemp)
    {
        UnloadSystemDrawFunctions();
        return FALSE;
    }
#endif

#if defined(WITH_SYSTEM_DrawMenuBarTemp)
    *(FARPROC *)&DrawMenuBarTemp = GetProcAddress(g_hUser32, "DrawMenuBarTemp");
    if (!DrawMenuBarTemp)
    {
        UnloadSystemDrawFunctions();
        return FALSE;
    }
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    return TRUE;
}
#endif

#if defined(WITH_NEW_PREVIEW)
_Success_(return != NULL)
static
HFONT CreateWindowFont(BYTE underline)
{
    LOGFONT lf;

    if (!lstrcpy(lf.lfFaceName, TEXT(DIALOG_FONTNAME)))
        return NULL;

    lf.lfHeight = FontPtToLog(DIALOG_FONTSIZE);
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_NORMAL;
    lf.lfItalic = 0;
    lf.lfUnderline = underline;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH;

    return CreateFontIndirect(&lf);
}

_Success_(return)
static
BOOL LoadPreviewSettings(_Out_ PREVIEW_DATA *pPreview)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSchemesKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
    {
        pPreview->bClassicPreview = FALSE;
        return FALSE;
    }

    BOOL ret = TRUE;

    pPreview->bClassicPreview = RegQueryBool(hKey, classicPreviewRegValue,
        FALSE);

    RegCloseKey(hKey);
    return ret;
}

_Success_(return)
static
BOOL SavePreviewSettings(_In_ const PREVIEW_DATA *pPreview)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSchemesKey, 0,
        KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, classicPreviewRegValue, 0, REG_DWORD,
        (BYTE *)&pPreview->bClassicPreview, sizeof(DWORD));

    RegCloseKey(hKey);
    return status == ERROR_SUCCESS;
}
#endif  /* defined(WITH_NEW_PREVIEW) */

_Success_(return != NULL)
static
HMENU LoadPreviewMenu(void)
{
    HMENU hMenu = LoadMenu(
        g_propSheet.hInstance, MAKEINTRESOURCE(IDM_PREVIEW_MENU));

    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE;

    mii.fState = MFS_DISABLED;
    SetMenuItemInfo(hMenu, IDM_DISABLED, FALSE, &mii);

    mii.fState = MFS_HILITE;
    SetMenuItemInfo(hMenu, IDM_SELECTED, FALSE, &mii);

    return hMenu;
}

#if defined(WITH_NEW_PREVIEW)
/* Create a checkered pattern brush for the scrollbar background. */
_Success_(return != NULL)
static
HBRUSH CreateScrollbarPatternBrush(void)
{
    /* WORD because bits must be word-aligned */
    static const WORD pattern[] =
    { 0x55, 0xAA, 0x55, 0xAA,
      0x55, 0xAA, 0x55, 0xAA
    };

    HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, pattern);
    if (!hBitmap)
        return NULL;

    return CreatePatternBrush(hBitmap);
}
#endif

/* Create a brush with the configured the desktop pattern.
 * Unlike GetSysColorBrush(COLOR_DESKTOP), this does not include colors.
 */
_Success_(return != NULL)
static
HBRUSH CreateDesktopBrush(_In_ const PREVIEW_DATA *pPreview)
{
    const SCHEME_DATA *pData = &pPreview->scheme;

#if defined(WITH_NEW_PREVIEW)
    if (!pData->desktopPattern || pPreview->bClassicPreview)
#endif
        return CreateSolidBrush(pData->colors[COLOR_DESKTOP]);

#if defined(WITH_NEW_PREVIEW)
    /* Pad bytes to words */
    WORD wPattern[8];
    for (int iLine = 0; iLine < 8; iLine++)
        wPattern[iLine] = (WORD)pData->desktopPattern[iLine];

    HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, wPattern);
    if (!hBitmap)
        return NULL;

    return CreatePatternBrush(hBitmap);
#endif
}

#if WINVER >= WINVER_XP
static
void LoadExplorerSettings(void)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
        0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return;

    g_bIconLabelShadows = RegQueryBool(hKey, TEXT("ListviewShadow"), TRUE);
}
#endif

_Success_(return)
static
BOOL InitPreviewWindow(HWND hWnd, _Out_ PREVIEW_DATA *pPreview)
{
#if defined(WITH_UNDOCUMENTED_API)
    if (!LoadSystemDrawFunctions())
        return FALSE;
#endif

    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
        pPreview->brushes[iColor] = NULL;

    pPreview->hFontCaption = NULL;
    pPreview->hFontMenu = NULL;
    pPreview->hFontMessage = NULL;
#if defined(WITH_NEW_PREVIEW)
    pPreview->hFontSmCaption = NULL;
    pPreview->hFontToolTip = NULL;
    pPreview->hFontDesktop = NULL;
    pPreview->hFontWindow = CreateWindowFont(0);
#endif
#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    pPreview->hFontHyperlink = CreateWindowFont(1);
#endif

    RECT rc;
    if (!GetClientRect(hWnd, &rc))
        return FALSE;

    HDC hdcWnd = GetDC(hWnd);
    if (!hdcWnd)
        return FALSE;

    pPreview->hdcPreview = CreateCompatibleDC(hdcWnd);
    if (!pPreview->hdcPreview)
    {
        ReleaseDC(hWnd, hdcWnd);
        return FALSE;
    }

    pPreview->hbmpPreview = CreateCompatibleBitmap(hdcWnd, rc.right, rc.bottom);
    if (!pPreview->hbmpPreview)
    {
        DeleteDC(pPreview->hdcPreview);
        pPreview->hdcPreview = NULL;

        ReleaseDC(hWnd, hdcWnd);
        return FALSE;
    }

    ReleaseDC(hWnd, hdcWnd);

    pPreview->hbmpPrevious =
        SelectObject(pPreview->hdcPreview, pPreview->hbmpPreview);

    pPreview->hMenu = LoadPreviewMenu();

    AllocAndLoadString(&pPreview->inactiveCaption, IDS_INACTIVE_CAPTION);
    AllocAndLoadString(&pPreview->activeCaption,   IDS_ACTIVE_CAPTION);
    AllocAndLoadString(&pPreview->windowText,      IDS_WINDOW_TEXT);
    AllocAndLoadString(&pPreview->messageCaption,  IDS_MESSAGE_CAPTION);
    AllocAndLoadString(&pPreview->messageText,     IDS_MESSAGE_TEXT);
    AllocAndLoadString(&pPreview->okText,          IDS_OK);

#if defined(WITH_NEW_PREVIEW)
    AllocAndLoadString(&pPreview->paletteCaption, IDS_PALETTE_CAPTION);
    AllocAndLoadString(&pPreview->normalText,     IDS_NORMAL_TEXT);
    AllocAndLoadString(&pPreview->selectedText,   IDS_SELECTED_TEXT);
    AllocAndLoadString(&pPreview->buttonText,     IDS_BUTTON_TEXT);
    AllocAndLoadString(&pPreview->toolTipText,    IDS_TOOLTIP_TEXT);
    AllocAndLoadString(&pPreview->iconLabel,      IDS_ICON_LABEL);
#if defined(WITH_NEW_PREVIEW_BIG)
    AllocAndLoadString(&pPreview->mainCaption,    IDS_MAIN_CAPTION);
    AllocAndLoadString(&pPreview->disabledText,   IDS_DISABLED_TEXT);
#if WINVER >= WINVER_2K
    AllocAndLoadString(&pPreview->hyperlinkText,  IDS_HYPERLINK_TEXT);
#endif
#endif

    int iconSize = ScaleForDpi(16);
    pPreview->hCaptionIcon = LoadImage(
        g_propSheet.hInstance, MAKEINTRESOURCE(IDI_PROGRAM),
        IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR);
#endif  /* defined(WITH_NEW_PREVIEW) */

#if WINVER >= WINVER_XP
    LoadExplorerSettings();
#endif

    return TRUE;
}

_Success_(return == 0)
static
LRESULT InitPreview(HWND hWnd)
{
    PREVIEW_DATA *pPreview = (PREVIEW_DATA *)Alloc(
        HEAP_ZERO_MEMORY, sizeof(PREVIEW_DATA));
    if (!pPreview)
    {
        ShowMessageFromResource(NULL, IDS_ERROR_MEM, IDS_ERROR, MB_OK);
        return -1;
    }

    SetLastError(0);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pPreview);
    if (GetLastError() != 0)
    {
        ShowMessageFromResource(NULL, IDS_ERROR_GENERIC, IDS_ERROR, MB_OK);
        return -1;
    }

    if (!InitPreviewWindow(hWnd, pPreview))
    {
        ShowMessageFromResource(NULL, IDS_ERROR_GENERIC, IDS_ERROR, MB_OK);
        return -1;
    }

    pPreview->bPainting = FALSE;
    return 0;
}

static
void DestroyPreviewFonts(_Inout_ PREVIEW_DATA *pPreview)
{
#define DeletePreviewObject(hObject) \
    if (pPreview->hObject) DeleteObject(pPreview->hObject)

    DeletePreviewObject(hFontCaption);
    DeletePreviewObject(hFontMenu);
    DeletePreviewObject(hFontMessage);
#if defined(WITH_NEW_PREVIEW)
    DeletePreviewObject(hFontSmCaption);
    DeletePreviewObject(hFontToolTip);
    DeletePreviewObject(hFontDesktop);
#endif

#undef DeleteFont
}

static
void FreePreviewStrings(_Inout_ PREVIEW_DATA *pPreview)
{
#define FreeStr(str) Free((HLOCAL)pPreview->str)

    FreeStr(inactiveCaption);
    FreeStr(activeCaption);
    FreeStr(windowText);
    FreeStr(messageCaption);
    FreeStr(messageText);
    FreeStr(okText);

#if defined(WITH_NEW_PREVIEW)
    FreeStr(paletteCaption);
    FreeStr(normalText);
    FreeStr(selectedText);
    FreeStr(buttonText);
    FreeStr(toolTipText);
    FreeStr(iconLabel);
#endif
#if defined(WITH_NEW_PREVIEW_BIG)
    FreeStr(mainCaption);
    FreeStr(disabledText);
#endif
#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    FreeStr(hyperlinkText);
#endif

#undef FreeStr
}

static
void DeleteBrushes(_Inout_ PREVIEW_DATA *pPreview)
{
#define DelBrush(br) if (br) { DeleteObject(br); br = NULL; }

    DelBrush(pPreview->hbrScrollbarPattern);

    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
        DelBrush(pPreview->brushes[iColor]);

#undef DelBrush
}

static
void DestroyPreview(_Inout_ PREVIEW_DATA *pPreview)
{
    SelectObject(pPreview->hdcPreview, pPreview->hbmpPrevious);

    if (pPreview->hbmpPreview)
        DeleteObject(pPreview->hbmpPreview);
    if (pPreview->hdcPreview)
        DeleteDC(pPreview->hdcPreview);

    DestroyPreviewFonts(pPreview);
#if defined(WITH_NEW_PREVIEW)
    if (pPreview->hFontWindow)
        DeleteObject(pPreview->hFontWindow);
#endif
#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    if (pPreview->hFontHyperlink)
        DeleteObject(pPreview->hFontHyperlink);
#endif

    if (pPreview->hMenu)
        DestroyMenu(pPreview->hMenu);

    FreePreviewStrings(pPreview);
    DeleteBrushes(pPreview);

    Free(pPreview);
    pPreview = NULL;

#if defined(WITH_UNDOCUMENTED_API)
    UnloadSystemDrawFunctions();
#endif
}

static
void SetPvNcMetrics(
    _In_ const NONCLIENTMETRICS *ncMetrics, _Out_ PREVIEW_DATA *pPreview)
{
    /* Preview data member       ncMetrics member               GetSystemMetrics indexes */
    pPreview->borderSize       = ncMetrics->iBorderWidth;  /* SM_CXFRAME - SM_CXEDGE - 1 */
#if WINVER >= WINVER_VISTA
    pPreview->paddedBorderSize = ncMetrics->iPaddedBorderWidth; /* SM_CXPADDEDBORDER */
#endif
    pPreview->captionWidth     = ncMetrics->iCaptionWidth;      /* SM_CXSIZE */
    pPreview->captionHeight    = ncMetrics->iCaptionHeight;     /* SM_CYCAPTION */
#if defined(WITH_NEW_PREVIEW)
    pPreview->smCaptionWidth   = ncMetrics->iSmCaptionWidth;    /* SM_CXSMSIZE */
    pPreview->smCaptionHeight  = ncMetrics->iSmCaptionHeight;   /* SM_CYSMCAPTION */
#endif
    pPreview->menuHeight       = ncMetrics->iMenuHeight;        /* SM_CYMENU - 1 */
    pPreview->scrollBarWidth   = ncMetrics->iScrollWidth;       /* SM_CXVSCROLL */
    pPreview->scrollButtonSize = ncMetrics->iScrollHeight;      /* SM_CXHSCROLL */
}

/* Returns TRUE if the metric has changed */
static
BOOL SetMetric(int *pMetric, int value)
{
    if (*pMetric == value)
        return FALSE;

    *pMetric = value;
    return TRUE;
}

static
BOOL SetPvNcMetric(int metric, int value, _Inout_ PREVIEW_DATA *pPreview)
{
    switch (metric)
    {
    case SIZE_BORDER:
        return SetMetric(&pPreview->borderSize, value);

    case SIZE_SCROLLWIDTH:
        return SetMetric(&pPreview->scrollBarWidth, value);

    case SIZE_SCROLLHEIGHT:
        return SetMetric(&pPreview->scrollButtonSize, value);

    case SIZE_CAPTIONWIDTH:
        return SetMetric(&pPreview->captionWidth, value);

    case SIZE_CAPTIONHEIGHT:
        return SetMetric(&pPreview->captionHeight, value);

#if defined(WITH_NEW_PREVIEW)
    case SIZE_SMCAPTIONWIDTH:
        return SetMetric(&pPreview->smCaptionWidth, value);

    case SIZE_SMCAPTIONHEIGHT:
        return SetMetric(&pPreview->smCaptionHeight, value);
#endif

    case SIZE_MENUHEIGHT:
        return SetMetric(&pPreview->menuHeight, value);

#if WINVER >= WINVER_VISTA
    case SIZE_PADDEDBORDER:
        return SetMetric(&pPreview->paddedBorderSize, value);
#endif
    }

    return FALSE;
}

static
void UnsetRect(_In_ RECT *prc)
{
    prc->left   = -1;
    prc->top    = -1;
    prc->right  = -1;
    prc->bottom = -1;
}

static
void UnsetInactiveRects(_Inout_ PREVIEW_DATA *pPreview)
{
    UnsetRect(&pPreview->rcInactiveFrame);
    UnsetRect(&pPreview->rcInactiveCaption);
#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview)
    {
        UnsetRect(&pPreview->rcInactiveClient);
        UnsetRect(&pPreview->rcInactiveScrollBar);
    }
#endif
}

static
void UnsetActiveRects(_Inout_ PREVIEW_DATA *pPreview)
{
    UnsetRect(&pPreview->rcActiveFrame);
    UnsetRect(&pPreview->rcActiveCaption);
    UnsetRect(&pPreview->rcActiveClient);
#if defined(WITH_NEW_PREVIEW)
    if (!pPreview->bClassicPreview)
    {
#if defined(WITH_NEW_PREVIEW_BIG)
        UnsetRect(&pPreview->rcSelectedText);
#endif
        UnsetRect(&pPreview->rcToolTip);
    }
    else
#endif
        UnsetRect(&pPreview->rcMenuBar);
    UnsetRect(&pPreview->rcActiveScrollBar);
}

static
void UnsetMessageRects(_Inout_ PREVIEW_DATA *pPreview)
{
    UnsetRect(&pPreview->rcMessageFrame);
    UnsetRect(&pPreview->rcMessageCaption);
    UnsetRect(&pPreview->rcMessageClient);
#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    if (!pPreview->bClassicPreview)
        UnsetRect(&pPreview->rcHyperlink);
#endif
    UnsetRect(&pPreview->rcMessageButton);
}

static
void CalculateWindowRect(
    _Out_ RECT *prcWindow, _In_ const RECT *prcFrame, int borderSize)
{
    prcWindow->left   = prcFrame->left   + borderSize;
    prcWindow->top    = prcFrame->top    + borderSize;
    prcWindow->right  = prcFrame->right  - borderSize;
    prcWindow->bottom = prcFrame->bottom - borderSize;

    if (prcWindow->top  >= prcWindow->bottom ||
        prcWindow->left >= prcWindow->right)
    {
        UnsetRect(prcWindow);
    }
}

static
void CalculateCaptionRect(
    _Out_ RECT *prcCaption, _In_ const RECT *prcWindow, int captionHeight)
{
    prcCaption->left   = prcWindow->left;
    prcCaption->top    = prcWindow->top;
    prcCaption->right  = prcWindow->right;
    /* Includes the bottom border */
    prcCaption->bottom = prcCaption->top + captionHeight;

    if (prcCaption->top  >= prcCaption->bottom)
        UnsetRect(prcCaption);
}

static
void CalculateMenuBarRect(_Out_ RECT *prc,
    _In_ const RECT *prcWnd, _In_ const RECT *prcCaption, int menuHeight)
{
    prc->left   = prcWnd->left;
    prc->top    = prcCaption->bottom + 1;
    prc->right  = prcWnd->right;

    /* + 1 because DrawMenuBarTemp expects the bottom border to be included in
     * the rect
     */
    prc->bottom = prc->top + menuHeight + 1;

    if (prc->bottom > prcWnd->bottom)
        UnsetRect(prc);
}

static
void CalculateClientRect(_Inout_ RECT *prc, _In_ const RECT *prcWnd, int top)
{
    if (top > prcWnd->bottom)
    {
        UnsetRect(prc);
        return;
    }

    prc->top    = top;
    prc->left   = prcWnd->left;
    prc->right  = prcWnd->right;
    prc->bottom = prcWnd->bottom;
}

static
void CalculateWindowsRects(_Inout_ PREVIEW_DATA *pPreview)
{
    int totalBorderSize = edgeSize + pPreview->borderSize + innerBorderSize;
#if WINVER >= WINVER_VISTA
    totalBorderSize += pPreview->paddedBorderSize;
#endif

#if defined(WITH_NEW_PREVIEW_BIG)
    if (pPreview->bClassicPreview)
    {
#endif
        pPreview->rcInactiveFrame.left =
            pPreview->rcDesktop.left + ScaleForDpi(8);
        pPreview->rcInactiveFrame.top =
            pPreview->rcDesktop.top + ScaleForDpi(8);
        pPreview->rcInactiveFrame.right =
            pPreview->rcDesktop.right - ScaleForDpi(26);
        pPreview->rcInactiveFrame.bottom =
            pPreview->rcDesktop.bottom - ScaleForDpi(30);

        pPreview->rcActiveFrame.right =
            pPreview->rcDesktop.right - ScaleForDpi(4);
        pPreview->rcActiveFrame.bottom =
            pPreview->rcDesktop.bottom - ScaleForDpi(25);
#if defined(WITH_NEW_PREVIEW_BIG)
    }
    else
    {
        {
            pPreview->rcMdiFrame.left =
                pPreview->rcDesktop.left + ScaleForDpi(8);
            pPreview->rcMdiFrame.top =
                pPreview->rcDesktop.top + ScaleForDpi(8);
            pPreview->rcMdiFrame.right =
                pPreview->rcDesktop.right - ScaleForDpi(8);
            pPreview->rcMdiFrame.bottom =
                pPreview->rcDesktop.bottom - ScaleForDpi(20);
        }

        RECT rcMdiWindow;

        CalculateWindowRect(
            &rcMdiWindow, &pPreview->rcMdiFrame, totalBorderSize);
        CalculateCaptionRect(&pPreview->rcMdiCaption, &rcMdiWindow,
            pPreview->captionHeight);

        CalculateMenuBarRect(&pPreview->rcMenuBar,
            &rcMdiWindow, &pPreview->rcMdiCaption, pPreview->menuHeight);

        pPreview->rcMdiWorkspace.top    = (pPreview->rcMenuBar.bottom > 0 ?
            pPreview->rcMenuBar.bottom :
            pPreview->rcMdiCaption.bottom + 1);
        pPreview->rcMdiWorkspace.left   = pPreview->rcMdiFrame.left   + totalBorderSize;
        pPreview->rcMdiWorkspace.right  = pPreview->rcMdiFrame.right  - totalBorderSize;
        pPreview->rcMdiWorkspace.bottom = pPreview->rcMdiFrame.bottom - totalBorderSize;

        if (pPreview->rcMdiWorkspace.bottom < pPreview->rcMdiCaption.bottom)
        {
            UnsetRect(&pPreview->rcMdiFrame);
            UnsetRect(&pPreview->rcMdiCaption);
            UnsetRect(&pPreview->rcMenuBar);
            UnsetRect(&pPreview->rcMdiWorkspace);
            UnsetInactiveRects(pPreview);
            UnsetActiveRects(pPreview);
            return;
        }

        pPreview->rcInactiveFrame.left = rcMdiWindow.left + edgeSize;
        pPreview->rcInactiveFrame.top = (pPreview->rcMenuBar.bottom >= 0 ?
            pPreview->rcMenuBar.bottom :
            pPreview->rcMdiCaption.bottom + 1) + edgeSize;
        pPreview->rcInactiveFrame.right =
            rcMdiWindow.right - ScaleForDpi(36);
        pPreview->rcInactiveFrame.bottom = rcMdiWindow.bottom - ScaleForDpi(30);

        pPreview->rcActiveFrame.right = pPreview->rcInactiveFrame.right + 4;
        pPreview->rcActiveFrame.bottom = rcMdiWindow.bottom - ScaleForDpi(6);
    }
#endif

    /* Rectangles of windows without the frame and border */

    RECT rcInactiveWindow;

    CalculateWindowRect(
        &rcInactiveWindow, &pPreview->rcInactiveFrame, totalBorderSize);
    CalculateCaptionRect(&pPreview->rcInactiveCaption, &rcInactiveWindow,
        pPreview->captionHeight);

    if (rcInactiveWindow.top < 0 ||
        rcInactiveWindow.bottom < pPreview->rcInactiveCaption.bottom)
    {
        UnsetInactiveRects(pPreview);
        UnsetActiveRects(pPreview);
        return;
    }

#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview)
    {
        CalculateClientRect(&pPreview->rcInactiveClient, &rcInactiveWindow,
            pPreview->rcInactiveCaption.bottom + 1);
    }
#endif

    pPreview->rcActiveFrame.left = rcInactiveWindow.left;
    pPreview->rcActiveFrame.top  = pPreview->rcInactiveCaption.bottom +
#if defined(WITH_NEW_PREVIEW_BIG)
        (pPreview->bClassicPreview ? 1 : ScaleForDpi(18));
#else
        1;
#endif

    RECT rcActiveWindow;
    CalculateWindowRect(
        &rcActiveWindow, &pPreview->rcActiveFrame, totalBorderSize);
    CalculateCaptionRect(
        &pPreview->rcActiveCaption, &rcActiveWindow, pPreview->captionHeight);

    if (rcActiveWindow.top < 0 ||
        rcActiveWindow.bottom < pPreview->rcActiveCaption.bottom)
    {
        UnsetActiveRects(pPreview);
        return;
    }

#if defined(WITH_NEW_PREVIEW_BIG)
    if (pPreview->bClassicPreview)
#endif
    {
        CalculateMenuBarRect(&pPreview->rcMenuBar,
            &rcActiveWindow, &pPreview->rcActiveCaption, pPreview->menuHeight);
    }

    CalculateClientRect(&pPreview->rcActiveClient, &rcActiveWindow, (
#if defined(WITH_NEW_PREVIEW_BIG)
        pPreview->bClassicPreview &&
#endif
            pPreview->rcMenuBar.bottom >= 0 ?
                pPreview->rcMenuBar.bottom :
                pPreview->rcActiveCaption.bottom + 1));

#if defined(WITH_NEW_PREVIEW)
    if (!pPreview->bClassicPreview)
    {
        if (pPreview->rcActiveClient.left >= 0)
        {
            pPreview->rcToolTip.bottom = pPreview->rcActiveFrame.bottom;
            pPreview->rcToolTip.left = pPreview->rcActiveClient.left + 8;
        }
        else
            UnsetRect(&pPreview->rcToolTip);
    }
#endif
}

static
void CalculateScrollBarRect(
    _Inout_ RECT *prc, _In_ const RECT *prcClient, int width)
{
    if (prcClient->top < 0)
    {
        UnsetRect(prc);
        return;
    }

    prc->top    = prcClient->top    + edgeSize;
    prc->bottom = prcClient->bottom - edgeSize;
    if (prc->top <= prc->bottom)
    {
        prc->right = prcClient->right - edgeSize;
        prc->left  = prc->right - width;
    }
    else
    {
        UnsetRect(prc);
    }
}

static
void CalculateMessageBoxRects(_Inout_ PREVIEW_DATA *pPreview)
{
    pPreview->rcMessageFrame.bottom =
        pPreview->rcDesktop.bottom - ScaleForDpi(4);
#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
    {
#endif
        /* Message Box */
        pPreview->rcMessageFrame.left  = pPreview->rcActiveClient.left +
            ScaleForDpi(4);
        pPreview->rcMessageFrame.top = (pPreview->rcActiveClient.top +
            pPreview->rcActiveClient.bottom) / 2;
        pPreview->rcMessageFrame.right = (pPreview->rcActiveClient.left +
            pPreview->rcActiveClient.right) / 2 + pPreview->captionHeight * 3;
#if defined(WITH_NEW_PREVIEW)
    }
    else
    {
        /* Palette Window */
        pPreview->rcMessageFrame.left =
#if defined(WITH_NEW_PREVIEW_BIG)
            pPreview->rcMdiFrame.left +
#else
            pPreview->rcActiveFrame.left +
#endif
            ScaleForDpi(157);
        pPreview->rcMessageFrame.top   = pPreview->rcMessageFrame.bottom -
#if defined(WITH_NEW_PREVIEW_BIG)
            ScaleForDpi(88);
#else
            ScaleForDpi(74);
#endif
        pPreview->rcMessageFrame.right = pPreview->rcMessageFrame.left +
            ScaleForDpi(130);
    }
#endif

    if (pPreview->rcMessageFrame.top < 0)
    {
        UnsetMessageRects(pPreview);
        return;
    }

    RECT rcMessageWindow;

    CalculateWindowRect(&rcMessageWindow, &pPreview->rcMessageFrame,
        edgeSize + innerBorderSize);
    CalculateCaptionRect(&pPreview->rcMessageCaption, &rcMessageWindow,
#if defined(WITH_NEW_PREVIEW)
        (pPreview->bClassicPreview ?
            pPreview->captionHeight :
            pPreview->smCaptionHeight));
#else
        pPreview->captionHeight);
#endif

    if (rcMessageWindow.bottom < pPreview->rcMessageCaption.bottom)
    {
        UnsetMessageRects(pPreview);
        return;
    }

    pPreview->rcMessageClient.left   = rcMessageWindow.left;
    pPreview->rcMessageClient.top    = pPreview->rcMessageCaption.bottom + 1;
    pPreview->rcMessageClient.right  = rcMessageWindow.right;
    pPreview->rcMessageClient.bottom = rcMessageWindow.bottom;

    CalculateClientRect(&pPreview->rcMessageClient, &rcMessageWindow,
        pPreview->rcMessageCaption.bottom + 1);

    if (pPreview->rcMessageClient.top >= pPreview->rcMessageClient.bottom)
    {
        UnsetMessageRects(pPreview);
        return;
    }

    int buttonWidth;
    int buttonHeight;
#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
    {
#endif
        buttonWidth = ScaleForDpi(72);
        buttonHeight = ScaleForDpi(24);
#if defined(WITH_NEW_PREVIEW)
    }
    else
    {
        buttonWidth = ScaleForDpi(75);
        buttonHeight = ScaleForDpi(21);
    }
#endif
    pPreview->rcMessageButton.bottom =
        pPreview->rcMessageClient.bottom - ScaleForDpi(3);
    pPreview->rcMessageButton.top =
        pPreview->rcMessageButton.bottom - buttonHeight;
    if (pPreview->rcMessageButton.top >= pPreview->rcMessageCaption.top)
    {
        pPreview->rcMessageButton.left =
            (pPreview->rcMessageClient.right + pPreview->rcMessageClient.left -
                buttonWidth) / 2;
        pPreview->rcMessageButton.right =
            pPreview->rcMessageButton.left + buttonWidth;
    }
    else
    {
        UnsetMessageRects(pPreview);
    }
}

static
void CalculateItemsRects(_Inout_ PREVIEW_DATA *pPreview)
{
    CalculateWindowsRects(pPreview);

#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview)
    {
        CalculateScrollBarRect(
            &pPreview->rcInactiveScrollBar, &pPreview->rcInactiveClient,
            pPreview->scrollBarWidth);
    }
#endif

    CalculateScrollBarRect(
        &pPreview->rcActiveScrollBar, &pPreview->rcActiveClient,
        pPreview->scrollBarWidth);

    CalculateMessageBoxRects(pPreview);
}

static
void Resize(UINT width, UINT height, _Out_ PREVIEW_DATA *pPreview)
{
    pPreview->rcDesktop.left   = 0;
    pPreview->rcDesktop.top    = 0;
    pPreview->rcDesktop.right  = width;
    pPreview->rcDesktop.bottom = height;

    CalculateItemsRects(pPreview);
}

static
void RecreatePreviewFonts(_Inout_ PREVIEW_DATA *pPreview)
{
    const SCHEME_DATA *pData = &pPreview->scheme;

    /* Even after calling DeleteObject(), the memory does not seem to be
     * released...
     */
    DestroyPreviewFonts(pPreview);

    pPreview->hFontCaption =
        CreateFontIndirect(&pData->ncMetrics.lfCaptionFont);

    pPreview->hFontMenu =
        CreateFontIndirect(&pData->ncMetrics.lfMenuFont);

    pPreview->hFontMessage =
        CreateFontIndirect(&pData->ncMetrics.lfMessageFont);

#if defined(WITH_NEW_PREVIEW)
    pPreview->hFontSmCaption =
        CreateFontIndirect(&pData->ncMetrics.lfSmCaptionFont);

    pPreview->hFontToolTip =
        CreateFontIndirect(&pData->ncMetrics.lfStatusFont);

    pPreview->hFontDesktop =
        CreateFontIndirect(&pData->lfIconFont);
#endif
}

#if defined(WITH_NEW_PREVIEW) && WINVER >= WINVER_XP
static
BOOL ShouldDrawIconLabelShadow(_In_ const PREVIEW_DATA *pPreview)
{
    return g_bIconLabelShadows && (pPreview->scheme.desktopPattern);
}
#endif

_Success_(return)
static
BOOL DrawIconLabel(
    _In_ const PREVIEW_DATA *pPreview, _In_ const TCHAR *szLabel, RECT *prc)
{
#define DrawIL() DrawText(hdc, szLabel, -1, prc, \
    DT_LEFT | DT_WORD_ELLIPSIS | DT_CENTER)

#define IsDarkColor(color) \
((GetRValue(color) * 2 + GetGValue(color) * 5 + GetBValue(color)) <= 128 * 8)

    const HDC hdc = pPreview->hdcPreview;
    BOOL ret = TRUE;
#if defined(WITH_NEW_PREVIEW) && WINVER >= WINVER_XP
    BOOL bDrawShadow = ShouldDrawIconLabelShadow(pPreview);

    if (bDrawShadow)
    {
        /* It should be blurred, but this is better than nothing */
        if (SetTextColor(hdc, RGB(0, 0, 0)) != CLR_INVALID &&
            OffsetRect(prc, 2, 2))
        {
            ret &= (DrawIL() > 0);
            ret &= OffsetRect(prc, -1, -1);
            ret &= (DrawIL() > 0);
            ret &= OffsetRect(prc, -1, -1);
        }
        else
            ret = FALSE;
    }
#endif

    COLORREF color;
    if (IsDarkColor(pPreview->scheme.colors[COLOR_DESKTOP])
#if defined(WITH_NEW_PREVIEW) && WINVER >= WINVER_XP
        || bDrawShadow
#endif
    )
    {
        color = RGB(0xFF, 0xFF, 0xFF);
    }
    else
        color = RGB(0, 0, 0);

    if (SetTextColor(hdc, color) == CLR_INVALID)
        return FALSE;

    ret &= (DrawIL() > 0);
    return ret;

#undef DrawIL
#undef IsDarkColor
}

_Success_(return)
static
BOOL DrawSchemeIconLabel(_In_ const PREVIEW_DATA *pPreview)
{
    SIZE size;

    if (!GetTextExtentPoint32(pPreview->hdcPreview,
        pPreview->iconLabel, lstrlen(pPreview->iconLabel), &size))
    {
        size.cx = ScaleForDpi(20);
        size.cy = ScaleForDpi(8);
    }

    RECT rc;
    rc.right  = pPreview->rcDesktop.right - ScaleForDpi(58) + size.cx / 2;
    rc.bottom = pPreview->rcDesktop.bottom - ScaleForDpi(3);
    rc.left   = rc.right - size.cx;
    rc.top    = rc.bottom - size.cy;

    return DrawIconLabel(pPreview, pPreview->iconLabel, &rc);
}

_Success_(return)
static
BOOL DrawDesktop(_In_ const PREVIEW_DATA *pPreview)
{
    const HDC hdc = pPreview->hdcPreview;
    const SCHEME_DATA *pData = &pPreview->scheme;

    BOOL ret = TRUE;

#if defined(WITH_NEW_PREVIEW)
    if (pData->desktopPattern)
    {
        ret &= (SetTextColor(hdc,
            pData->colors[COLOR_DESKTOP]) != CLR_INVALID);
        COLORREF previousColor = SetBkColor(hdc,
            pData->colors[COLOR_WINDOWTEXT]);

        ret &= (FillRect(hdc,
            &pPreview->rcDesktop, pPreview->brushes[COLOR_DESKTOP]) != 0);

        SetBkColor(hdc, previousColor);
    }
    else
#endif
    {
        ret &= (FillRect(hdc,
            &pPreview->rcDesktop, pPreview->brushes[COLOR_DESKTOP]) != 0);
    }

#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
        return ret;

    /* Icon labels */

    HGDIOBJ hPreviousFont = SelectObject(hdc, pPreview->hFontDesktop);

    ret &= DrawSchemeIconLabel(pPreview);

    if (hPreviousFont)
        SelectObject(hdc, hPreviousFont);
#endif

    return ret;
}

_Success_(return)
static
BOOL DrawCaptionBar(_In_ const PREVIEW_DATA *pPreview, RECT *prcCaption,
    TCHAR *caption, UINT uFlags, BOOL withMinMax, int buttonWidth,
    HFONT hFont)
{
    const HDC hdc = pPreview->hdcPreview;

    int totalButtonsWidth = buttonWidth;
    if (withMinMax)
        totalButtonsWidth *= 3;

    RECT rc;
    rc.top = prcCaption->top;
    rc.bottom = prcCaption->bottom;
    rc.left = prcCaption->left;
    rc.right = prcCaption->right - totalButtonsWidth - 2;

    BOOL ret = TRUE;

    ret &= DrawCaptionTemp(NULL, hdc, &rc, hFont,
#if defined(WITH_NEW_PREVIEW)
        (uFlags & DC_ICON ? pPreview->hCaptionIcon : NULL),
#else
        NULL,
#endif
        caption, uFlags);

    rc.left = rc.right;
    rc.right = prcCaption->right;
    ret &= DrawCaptionButtons(hdc, &rc, withMinMax, buttonWidth, uFlags);

    ret &= DrawBottomBorder(hdc, prcCaption);

    return ret;
}

_Success_(return)
static
BOOL DrawActiveClient(_Inout_ PREVIEW_DATA *pPreview)
{
    const HDC hdc = pPreview->hdcPreview;
    BOOL ret = TRUE;

    if (pPreview->rcActiveClient.top < 0)
        return ret;

    RECT rc = pPreview->rcActiveClient;
    ret &= MyDrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
    ret &= (FillRect(hdc, &rc, pPreview->brushes[COLOR_WINDOW]) != 0);

    /* Window Text */

    rc = pPreview->rcActiveClient;
#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
    {
#endif
        rc.left   += ScaleForDpi(4);
        rc.top    += ScaleForDpi(4);
#if defined(WITH_NEW_PREVIEW)
    }
    else
    {
        rc.left   += ScaleForDpi(6);
        rc.top    += ScaleForDpi(3);
    }
#endif
    rc.right  -= (pPreview->scrollBarWidth + 2);
    rc.bottom -= 2;

#if defined(WITH_NEW_PREVIEW)
    HGDIOBJ hPreviousFont = SelectObject(hdc, (pPreview->bClassicPreview ?
        pPreview->hFontCaption : pPreview->hFontWindow));
#else
    HGDIOBJ hPreviousFont = SelectObject(hdc, pPreview->hFontCaption);
#endif

#if defined(WITH_NEW_PREVIEW)
    const TCHAR *text = (pPreview->bClassicPreview ?
            pPreview->windowText :
            pPreview->normalText);
#else
    const TCHAR *text = pPreview->windowText;
#endif
    SIZE size;
    if (!GetTextExtentPoint32(pPreview->hdcPreview, text, lstrlen(text), &size))
        size.cy = ScaleForDpi(15);
    rc.bottom = min(rc.bottom, rc.top + size.cy);

    ret &= (SetTextColor(hdc,
        pPreview->scheme.colors[COLOR_WINDOWTEXT]) != CLR_INVALID);
    ret &= (DrawText(hdc, text, -1, &rc,
        DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_EDITCONTROL) > 0);

#if defined(WITH_NEW_PREVIEW)
    if (!pPreview->bClassicPreview)
    {
        if (!GetTextExtentPoint32(pPreview->hdcPreview,
            pPreview->selectedText, lstrlen(pPreview->selectedText), &size))
        {
            size.cx = ScaleForDpi(69);
            size.cy = ScaleForDpi(15);
        }
        pPreview->rcSelectedText.left   = rc.left;
        pPreview->rcSelectedText.right  = rc.left + size.cx;
        pPreview->rcSelectedText.top    = rc.bottom + 1;
        pPreview->rcSelectedText.bottom =
            min(pPreview->rcActiveClient.bottom - 2, rc.bottom + size.cy);
        if (pPreview->rcSelectedText.top < pPreview->rcSelectedText.bottom)
        {
            ret &= (FillRect(hdc, &pPreview->rcSelectedText,
                pPreview->brushes[COLOR_HIGHLIGHT]) != 0);
            ret &= (SetTextColor(hdc,
                pPreview->scheme.colors[COLOR_HIGHLIGHTTEXT]) != CLR_INVALID);
            ret &= (DrawText(hdc, pPreview->selectedText, -1,
                &pPreview->rcSelectedText,
                DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_EDITCONTROL) >
                0);
        }
        else
            UnsetRect(&pPreview->rcSelectedText);
    }
#endif

    if (hPreviousFont)
        SelectObject(hdc, hPreviousFont);

    /* Scrollbar */

    if (pPreview->rcActiveScrollBar.top < 0)
        return ret;

#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
    {
        ret &= DrawScrollbar(hdc, &pPreview->rcActiveScrollBar,
            pPreview->scrollButtonSize, NULL, TRUE);
    }
    else
    {
        ret &= DrawScrollbar(hdc, &pPreview->rcActiveScrollBar,
            pPreview->scrollButtonSize, pPreview->hbrScrollbarPattern, FALSE);
    }
#else
    ret &= DrawScrollbar(hdc, &pPreview->rcActiveScrollBar,
        pPreview->scrollButtonSize);
#endif

    return ret;
}

_Success_(return)
static
BOOL DrawMenuBarPreview(_Inout_ PREVIEW_DATA *pPreview, HWND hWnd)
{
    const HDC hdc = pPreview->hdcPreview;
    BOOL ret = TRUE;

    if (pPreview->rcMenuBar.top < 0)
        return ret;

    RECT rc = pPreview->rcMenuBar;
    ret &= (DrawMenuBarTemp(hWnd, hdc, &rc, pPreview->hMenu,
        pPreview->hFontMenu) == 0);
    rc.bottom--;
    ret &= DrawBottomBorder(hdc, &rc);

    return ret;
}

_Success_(return)
static
BOOL DrawWindows(_Inout_ PREVIEW_DATA *pPreview, HWND hWnd, UINT captionFlags)
{
    const HDC hdc = pPreview->hdcPreview;
    BOOL ret = TRUE;

    int borderSize = pPreview->borderSize + 1;
#if WINVER >= WINVER_VISTA
    borderSize += pPreview->paddedBorderSize;
#endif

#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview)
    {
        /* MDI window border and caption bar */
        if (pPreview->rcMdiFrame.top < 0)
            return ret;

        if (pPreview->rcMdiWorkspace.top > 0)
        {
            RECT rc = pPreview->rcMdiWorkspace;
            ret &= MyDrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
            ret &= (FillRect(hdc, &rc,
                pPreview->brushes[COLOR_APPWORKSPACE]) != 0);
        }
        ret &= DrawBorderedWindow(
            hdc, &pPreview->rcMdiFrame, borderSize, COLOR_ACTIVEBORDER);
        ret &= DrawCaptionBar(pPreview, &pPreview->rcMdiCaption,
            pPreview->mainCaption, captionFlags | DC_ACTIVE | DC_ICON, TRUE,
            pPreview->captionWidth, pPreview->hFontCaption);

        ret &= DrawMenuBarPreview(pPreview, hWnd);
    }

    if (pPreview->rcInactiveFrame.top < 0)
        return ret;

    /* Inactive client area */
    if (!pPreview->bClassicPreview && pPreview->rcInactiveClient.top >= 0)
    {
        RECT rc = pPreview->rcInactiveClient;
        ret &= MyDrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
        ret &= (FillRect(hdc, &rc, pPreview->brushes[COLOR_3DFACE]) != 0);

        /* Disabled Text */
        HGDIOBJ hPreviousFont = SelectObject(hdc, pPreview->hFontWindow);
        rc.left += ScaleForDpi(4);
        rc.top += ScaleForDpi(1);
        ret &= (SetTextColor(hdc, pPreview->scheme.colors[COLOR_GRAYTEXT]) !=
            CLR_INVALID);

        ret &= (DrawText(hdc, pPreview->disabledText, -1, &rc,
            DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_EDITCONTROL) > 0);

        if (hPreviousFont)
            SelectObject(hdc, hPreviousFont);

        /* Scrollbar */
        if (pPreview->rcInactiveScrollBar.top >= 0)
        {
            ret &= DrawScrollbar(hdc, &pPreview->rcInactiveScrollBar,
                pPreview->scrollButtonSize, pPreview->hbrScrollbarPattern,
                FALSE);
        }
    }
#endif  /* defined(WITH_NEW_PREVIEW_BIG) */

    /* Inactive border and caption bar */
    ret &= DrawBorderedWindow(
        hdc, &pPreview->rcInactiveFrame, borderSize, COLOR_INACTIVEBORDER);
    ret &= DrawCaptionBar(pPreview, &pPreview->rcInactiveCaption,
        pPreview->inactiveCaption, captionFlags
#if defined(WITH_NEW_PREVIEW)
        | (pPreview->bClassicPreview ? 0 : DC_ICON)
#endif
        , TRUE, pPreview->captionWidth, pPreview->hFontCaption);

    if (pPreview->rcActiveFrame.top < 0)
        return ret;

    /* Active client area */
    ret &= DrawActiveClient(pPreview);

    /* Active Window border and caption bar */
    ret &= DrawBorderedWindow(
        hdc, &pPreview->rcActiveFrame, borderSize, COLOR_ACTIVEBORDER);
    ret &= DrawCaptionBar(pPreview, &pPreview->rcActiveCaption,
        pPreview->activeCaption,
        captionFlags | DC_ACTIVE
#if defined(WITH_NEW_PREVIEW)
        | (pPreview->bClassicPreview ? 0 : DC_ICON)
#endif
        , TRUE, pPreview->captionWidth, pPreview->hFontCaption);

#if defined(WITH_NEW_PREVIEW_BIG)
    if (pPreview->bClassicPreview)
#endif
        ret &= DrawMenuBarPreview(pPreview, hWnd);

    return ret;
}

_Success_(return)
static
BOOL DrawMessageBox(_Inout_ PREVIEW_DATA *pPreview, UINT captionFlags)
{
    const HDC hdc = pPreview->hdcPreview;
    BOOL ret = TRUE;

    if (pPreview->rcMessageFrame.top < 0)
        return ret;

    ret &= MyDrawEdge(
        hdc, &pPreview->rcMessageFrame, EDGE_RAISED, BF_RECT | BF_MIDDLE);
#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
    {
#endif
        ret &= DrawCaptionBar(pPreview, &pPreview->rcMessageCaption,
            pPreview->messageCaption, captionFlags | DC_ACTIVE,
            FALSE, pPreview->captionWidth, pPreview->hFontCaption);
#if defined(WITH_NEW_PREVIEW)
    }
    else
    {
        ret &= DrawCaptionBar(pPreview, &pPreview->rcMessageCaption,
            pPreview->paletteCaption, captionFlags,
            FALSE, pPreview->smCaptionWidth, pPreview->hFontSmCaption);
    }
#endif

    /* Message text */

    RECT rc = pPreview->rcMessageClient;
    rc.left += ScaleForDpi(3);
    rc.top += ScaleForDpi(1);
    if (rc.top >= rc.bottom)
        return ret;

    HGDIOBJ hPreviousFont = SelectObject(hdc, pPreview->hFontMessage);
    ret &= (SetTextColor(hdc,
        pPreview->scheme.colors[COLOR_WINDOWTEXT]) != CLR_INVALID);
    ret &= (DrawText(hdc, pPreview->messageText, -1, &rc,
        DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS) > 0);

#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    if (!pPreview->bClassicPreview)
    {
        SIZE size;
        if (!GetTextExtentPoint32(pPreview->hdcPreview,
            pPreview->messageText, lstrlen(pPreview->messageText), &size))
        {
            size.cy = ScaleForDpi(15);
        }
        pPreview->rcHyperlink.top = pPreview->rcMessageClient.top + size.cy + 1;
    }
#endif

    if (hPreviousFont)
        SelectObject(hdc, hPreviousFont);

#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    if (!pPreview->bClassicPreview)
    {
        hPreviousFont = SelectObject(hdc, pPreview->hFontHyperlink);

        SIZE size;
        if (!GetTextExtentPoint32(pPreview->hdcPreview,
            pPreview->hyperlinkText, lstrlen(pPreview->hyperlinkText), &size))
        {
            size.cx = ScaleForDpi(62);
            size.cy = ScaleForDpi(16);
        }

        pPreview->rcHyperlink.left   = pPreview->rcMessageClient.left +
            ScaleForDpi(3);
        pPreview->rcHyperlink.right  = pPreview->rcHyperlink.left + size.cx;
        pPreview->rcHyperlink.bottom = min(pPreview->rcHyperlink.top + size.cy,
            pPreview->rcMessageClient.bottom);
        if (pPreview->rcHyperlink.top < pPreview->rcHyperlink.bottom)
        {
            ret &= (SetTextColor(hdc,
                pPreview->scheme.colors[COLOR_HOTLIGHT]) != CLR_INVALID);
            ret &= (DrawText(hdc, pPreview->hyperlinkText, -1,
                &pPreview->rcHyperlink,
                DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS) > 0);
        }

        if (hPreviousFont)
            SelectObject(hdc, hPreviousFont);
    }
#endif

    /* Button */
    if (pPreview->rcMessageButton.top < 0)
        return ret;

#if defined(WITH_NEW_PREVIEW)
    if (!pPreview->bClassicPreview)
    {
        /* Focus border */
        rc = pPreview->rcMessageButton;
        InflateRect(&rc, 1, 1);
        ret &= DrawBorder(hdc, &rc, 1, pPreview->brushes[COLOR_WINDOWFRAME]);
    }
#endif

    rc = pPreview->rcMessageButton;
    ret &= MyDrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONPUSH);
    rc.bottom--;
    ret &= (SetTextColor(hdc,
        pPreview->scheme.colors[COLOR_BTNTEXT]) != CLR_INVALID);
    hPreviousFont = SelectObject(hdc, pPreview->hFontMessage);
#if defined(WITH_NEW_PREVIEW)
    ret &= (DrawText(hdc,
        (pPreview->bClassicPreview ?
            pPreview->okText :
            pPreview->buttonText),
        -1, &rc,
#else
    ret &= (DrawText(hdc, pPreview->okText, -1, &rc,
#endif
        DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS) > 0);

    if (hPreviousFont)
        SelectObject(hdc, hPreviousFont);

    return ret;
}

#if defined(WITH_NEW_PREVIEW)
_Success_(return)
static
BOOL DrawToolTip(_Inout_ PREVIEW_DATA *pPreview)
{
    const HDC hdc = pPreview->hdcPreview;
    BOOL ret = TRUE;

    if (pPreview->bClassicPreview)
        return ret;

    if (pPreview->rcToolTip.left < 0)
        return ret;

    HGDIOBJ hPreviousFont = SelectObject(hdc, pPreview->hFontToolTip);

    SIZE size;
    if (!GetTextExtentPoint32(pPreview->hdcPreview,
        pPreview->toolTipText, lstrlen(pPreview->toolTipText), &size))
    {
        size.cx = ScaleForDpi(45);
        size.cy = ScaleForDpi(14);
    }

    pPreview->rcToolTip.top    = pPreview->rcToolTip.bottom - size.cy;
    pPreview->rcToolTip.right  = pPreview->rcToolTip.left   + size.cx;

    RECT rc;
    rc.left   = pPreview->rcToolTip.left   - 2;
    rc.top    = pPreview->rcToolTip.top    - 2;
    rc.right  = pPreview->rcToolTip.right  + 2;
    rc.bottom = pPreview->rcToolTip.bottom + 2;
    ret &= DrawBorder(hdc, &rc, 1, pPreview->brushes[COLOR_INFOTEXT]);

    InflateRect(&rc, -1, -1);
    ret &= (FillRect(hdc, &rc, pPreview->brushes[COLOR_INFOBK]) != 0);

    ret &= (SetTextColor(hdc,
        pPreview->scheme.colors[COLOR_INFOTEXT]) != CLR_INVALID);
    ret &= (DrawText(hdc, pPreview->toolTipText, -1,
        &pPreview->rcToolTip,
        DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS) > 0);

    if (hPreviousFont)
        SelectObject(hdc, hPreviousFont);

    return ret;
}
#endif

/* The rect of these text items, depending on the font are also calculated here:
 * Selected Text and ToolTips
 */
_Success_(return)
static
BOOL PaintPreview(HWND hWnd, _Inout_ PREVIEW_DATA *pPreview)
{
    BOOL ret = TRUE;

    SCHEME_DATA *pData = &pPreview->scheme;

    SetPreviewSettings(pData->colors, pPreview->brushes
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
        , pData->bFlatMenus
#endif
    );

#if defined(WITH_UNDOCUMENTED_API)
    /* Passing the brushes is necessary for Windows 9x */
    DWORD_PTR currentColors = SetSysColorsTemp(
        pData->colors, pPreview->brushes, NUM_COLORS);
#endif

    int previousBkMode = SetBkMode(pPreview->hdcPreview, TRANSPARENT);
    ret &= (previousBkMode != 0);

    UINT captionFlags = DC_TEXT;
#if WINVER >= WINVER_2K && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    if (pData->bGradientCaptions)
        captionFlags |= DC_GRADIENT;
#endif

    ret &= DrawDesktop(pPreview);
    ret &= DrawWindows(pPreview, hWnd, captionFlags);
    ret &= DrawMessageBox(pPreview, captionFlags);
#if defined(WITH_NEW_PREVIEW)
    ret &= DrawToolTip(pPreview);
#endif

    if (previousBkMode)
        SetBkMode(pPreview->hdcPreview, previousBkMode);

#if defined(WITH_UNDOCUMENTED_API)
    if (currentColors)
        SetSysColorsTemp(NULL, NULL, currentColors);
#endif

    return ret;
}

static
void PaintPreviewWindow(HWND hWnd, _Inout_ PREVIEW_DATA *pPreview)
{
    /* Workaround to paint only once instead of ~30 times */
    if (pPreview->bPainting)
        return;

    pPreview->bPainting = TRUE;

    /* Avoid refresh issues by using a buffer */

    PaintPreview(hWnd, pPreview);

    PAINTSTRUCT ps;
    HDC hdcPaint = BeginPaint(hWnd, &ps);
    if (!hdcPaint)
        return;

    BitBlt(hdcPaint, 0, 0, pPreview->rcDesktop.right, pPreview->rcDesktop.bottom,
        pPreview->hdcPreview, 0, 0, SRCCOPY);
    EndPaint(hWnd, &ps);

    pPreview->bPainting = FALSE;
}

static
UINT GetElementUnderCursor(POINT pt, _In_ const PREVIEW_DATA *pPreview)
{
#define InRect(_pvrect) PtInRect(&pPreview->_pvrect, pt)

#if defined(WITH_NEW_PREVIEW)
    if (InRect(rcToolTip))
        return ELEMENT_TOOLTIP;
#endif

    if (InRect(rcMessageButton))
        return ELEMENT_3DOBJECT;

#if defined(WITH_NEW_PREVIEW_BIG) && WINVER >= WINVER_2K
    if (!pPreview->bClassicPreview && InRect(rcHyperlink))
        return ELEMENT_HYPERLINK;
#endif

    if (InRect(rcMessageCaption))
#if defined(WITH_NEW_PREVIEW)
        return (pPreview->bClassicPreview ?
            ELEMENT_ACTIVECAPTION :
            ELEMENT_SMCAPTION);
#else
        return ELEMENT_ACTIVECAPTION;
#endif

    /* (Includes rcMessageClient) */
    if (InRect(rcMessageFrame))
        return ELEMENT_MSGBOX;

    if (InRect(rcMenuBar))
        return ELEMENT_MENU;

    if (InRect(rcActiveScrollBar))
        return ELEMENT_SCROLLBAR;

#if defined(WITH_NEW_PREVIEW)
    if (!pPreview->bClassicPreview)
    {
        if (InRect(rcSelectedText))
            return ELEMENT_SELECTEDITEM;
    }
#endif

    if (InRect(rcActiveCaption))
        return ELEMENT_ACTIVECAPTION;

    if (InRect(rcActiveClient))
        return ELEMENT_WINDOW;

    if (InRect(rcActiveFrame))
        return ELEMENT_ACTIVEBORDER;

#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview && InRect(rcInactiveScrollBar))
        return ELEMENT_SCROLLBAR;
#endif

    if (InRect(rcInactiveCaption))
        return ELEMENT_INACTIVECAPTION;

#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview && InRect(rcInactiveClient))
        return ELEMENT_DISABLEDITEM;
#endif

    if (InRect(rcInactiveFrame))
        return ELEMENT_INACTIVEBORDER;

#if defined(WITH_NEW_PREVIEW_BIG)
    if (!pPreview->bClassicPreview)
    {
        if (InRect(rcMdiCaption))
            return ELEMENT_ACTIVECAPTION;

        if (InRect(rcMdiWorkspace))
            return ELEMENT_APPBACKGROUND;

        if (InRect(rcMdiFrame))
            return ELEMENT_ACTIVEBORDER;
    }
#endif

    return ELEMENT_DESKTOP;

#undef InRect
}

static
void HandleClick(HWND hWnd, int xPos, int yPos,
    _In_ const PREVIEW_DATA *pPreview)
{
    POINT pt;
    pt.x = xPos;
    pt.y = yPos;

    UINT element = GetElementUnderCursor(pt, pPreview);

    SendMessage(GetParent(hWnd), WM_COMMAND,
        MAKEWPARAM(GetWindowLongPtr(hWnd, GWLP_ID), 0),
        (LPARAM)element);
}

#if defined(WITH_PREVIEW_CONTEXT_MENU)
static
void ShowContextMenu(HWND hWnd, int xPos, int yPos,
    _In_ const PREVIEW_DATA *pPreview)
{
#if !defined(WITH_NEW_PREVIEW)
    UNREFERENCED_PARAMETER(pPreview);
#endif

    HMENU hMenu = LoadMenu(
        g_propSheet.hInstance, MAKEINTRESOURCE(IDM_PREVIEW_CONTEXT_MENU));
    if (!hMenu)
        return;

    HMENU hSubMenu = GetSubMenu(hMenu, 0);
    if (!hSubMenu)
    {
        DestroyMenu(hMenu);
        return;
    }

    UINT uFlags = (GetSystemMetrics(SM_MENUDROPALIGNMENT) ?
        TPM_RIGHTALIGN : TPM_LEFTALIGN) |
        TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_HORIZONTAL;

#if defined(WITH_NEW_PREVIEW)
    if (pPreview->bClassicPreview)
    {
        MENUITEMINFO mii;
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_STATE;
        mii.fState = MFS_CHECKED;

        SetMenuItemInfo(hSubMenu, IDM_PREVIEW_CLASSIC, FALSE, &mii);
    }
#endif

    TrackPopupMenuEx(hSubMenu, uFlags, xPos, yPos, hWnd, NULL);

    DestroyMenu(hMenu);
}
#endif  /* defined(WITH_PREVIEW_CONTEXT_MENU) */

/* Create only the brushes needed for the preview. */
static
void CreateBrushes(_Out_ PREVIEW_DATA *pPreview)
{
    const SCHEME_DATA *pData = &pPreview->scheme;

#define CreateColorBrush(_iColor) \
    pPreview->brushes[_iColor] = CreateSolidBrush(pData->colors[_iColor])

    CreateColorBrush(COLOR_SCROLLBAR);
#if defined(WITH_NEW_PREVIEW)
    pPreview->hbrScrollbarPattern = CreateScrollbarPatternBrush();
#endif
    pPreview->brushes[COLOR_DESKTOP] = CreateDesktopBrush(pPreview);

    CreateColorBrush(COLOR_ACTIVECAPTION);
    CreateColorBrush(COLOR_INACTIVECAPTION);
    CreateColorBrush(COLOR_INACTIVECAPTION);
    CreateColorBrush(COLOR_MENU);
    CreateColorBrush(COLOR_WINDOW);
    CreateColorBrush(COLOR_WINDOWFRAME);
    CreateColorBrush(COLOR_ACTIVEBORDER);
    CreateColorBrush(COLOR_INACTIVEBORDER);
    CreateColorBrush(COLOR_APPWORKSPACE);
    CreateColorBrush(COLOR_HIGHLIGHT);
    CreateColorBrush(COLOR_3DFACE);
#if defined(WITH_UNDOCUMENTED_API) && (WINVER < WINVER_2K || defined(WINVER_IS_98))
    CreateColorBrush(COLOR_3DSHADOW);
#endif
#if defined(WITH_UNDOCUMENTED_API) && (WINVER < WINVER_2K || defined(WINVER_IS_98))
    CreateColorBrush(COLOR_3DHILIGHT);
    CreateColorBrush(COLOR_3DDKSHADOW);
    CreateColorBrush(COLOR_3DLIGHT);
#endif
    CreateColorBrush(COLOR_INFOTEXT);
    CreateColorBrush(COLOR_INFOBK);
#if WINVER >= WINVER_2K
    CreateColorBrush(COLOR_GRADIENTACTIVECAPTION);
    CreateColorBrush(COLOR_GRADIENTINACTIVECAPTION);
#endif
#if WINVER >= WINVER_XP
    CreateColorBrush(COLOR_MENUHILIGHT);
    CreateColorBrush(COLOR_MENUBAR);
#endif

#undef CreateColorBrush
}

static
void UpdateScheme(
    _Inout_ PREVIEW_DATA *pPreview, _Inout_ SCHEME_DATA *pData, WPARAM flags)
{
#if WINVER >= WINVER_XP
    if (flags & PV_UPDATE_EXPLORER_SETTINGS)
    {
        LoadExplorerSettings();
        return;
    }
#endif

#define HasColorChanged(_iColor) \
    pData->colors[_iColor] != pPreview->scheme.colors[_iColor]

#define RecreateColorBrush(_iColor) \
    if (HasColorChanged(_iColor) && pPreview->brushes[_iColor]) \
    { \
        DeleteObject(pPreview->brushes[_iColor]); \
        pPreview->brushes[_iColor] = CreateSolidBrush(pData->colors[_iColor]); \
    }

    if (!pPreview->brushes[0])
    {
        /* No brushes created yet */
        pPreview->scheme = *pData;
        CreateBrushes(pPreview);
    }
    else
    {
        /* Recreate only brushes of solid colors used for the preview */
        RecreateColorBrush(COLOR_SCROLLBAR);

        for (int iColor = COLOR_DESKTOP + 1; iColor < NUM_COLORS; iColor++)
        {
            RecreateColorBrush(iColor);
        }

        BOOL bRecreateDesktopBrush = ((HasColorChanged(COLOR_DESKTOP)
#if defined(WITH_NEW_PREVIEW)
            && (pPreview->bClassicPreview || !pData->desktopPattern))
            || pData->desktopPattern != pPreview->scheme.desktopPattern
#endif
        );

        pPreview->scheme = *pData;

        if (bRecreateDesktopBrush)
        {
            if (pPreview->brushes[COLOR_DESKTOP])
                DeleteObject(pPreview->brushes[COLOR_DESKTOP]);
            pPreview->brushes[COLOR_DESKTOP] = CreateDesktopBrush(pPreview);
        }
    }

#undef RecreateColorBrush
#undef HasColorChanged

    if (!flags)
    {
        RecreatePreviewFonts(pPreview);
        SetPvNcMetrics(&pPreview->scheme.ncMetrics, pPreview);
        CalculateItemsRects(pPreview);
    }
}

_Success_(return)
static
BOOL SetSize(int metric, int value, _Inout_ PREVIEW_DATA *pPreview)
{
    if (!SetPvNcMetric(metric, value, pPreview))
        return FALSE;

    CalculateItemsRects(pPreview);
    return TRUE;
}

_Success_(return)
static
BOOL SetFont(int iFont, _In_ LOGFONT *plfNewFont, _Inout_ PREVIEW_DATA *pPreview)
{
    HFONT *phFont = NULL;

    switch (iFont)
    {
    case FONT_CAPTION:
        phFont = &pPreview->hFontCaption;
        break;

    case FONT_MENU:
        phFont = &pPreview->hFontMenu;
        break;

    case FONT_MESSAGE:
        phFont = &pPreview->hFontMessage;
        break;

#if defined(WITH_NEW_PREVIEW)
    case FONT_SMCAPTION:
        phFont = &pPreview->hFontSmCaption;
        break;

    case FONT_TOOLTIP:
        phFont = &pPreview->hFontToolTip;
        break;

    case FONT_DESKTOP:
        phFont = &pPreview->hFontDesktop;
        break;
#endif

    default:
        return FALSE;
    }

    if (*phFont)
        DeleteObject(*phFont);
    *phFont = CreateFontIndirect(plfNewFont);

    return TRUE;
}

/* Called by ThemePreviewWndProc and SchemePreviewWndProc */
#define GetPreviewData() (PREVIEW_DATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA)

static
LRESULT CALLBACK PreviewWndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
    case WM_CREATE:
        return InitPreview(hWnd);

    case WM_DESTROY:
        DestroyPreview(GetPreviewData());
        return 0;

    case WM_SIZE:
        Resize(LOWORD(lParam), HIWORD(lParam), GetPreviewData());
        return 0;

    case WM_PAINT:
        PaintPreviewWindow(hWnd, GetPreviewData());
        return 0;

    case PVM_UPDATESCHEME:
        UpdateScheme(GetPreviewData(), (SCHEME_DATA *)lParam, wParam);
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;

    case PVM_SETSIZE:
        if (SetSize((int)wParam, (int)lParam, GetPreviewData()))
        {
            InvalidateRect(hWnd, NULL, FALSE);
            return TRUE;
        }

        return FALSE;

    case PVM_SETFONT:
        if (SetFont((int)wParam, (LOGFONT *)lParam, GetPreviewData()))
        {
            InvalidateRect(hWnd, NULL, FALSE);
            return TRUE;
        }

        return FALSE;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static
LRESULT CALLBACK SchemePreviewWndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        LRESULT result = PreviewWndProc(hWnd, uMsg, wParam, lParam);

#if defined(WITH_NEW_PREVIEW)
        PREVIEW_DATA *pPreview = GetPreviewData();
        LoadPreviewSettings(pPreview);
#endif

        return result;
    }

    case WM_DESTROY:
#if defined(WITH_NEW_PREVIEW)
        SavePreviewSettings(GetPreviewData());
#endif
        break;

    case WM_LBUTTONDOWN:
        HandleClick(hWnd, LOWORD(lParam), HIWORD(lParam), GetPreviewData());
        return 0;

#if defined(WITH_PREVIEW_CONTEXT_MENU)
    case WM_CONTEXTMENU:
        ShowContextMenu(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
            GetPreviewData());
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
#if defined(WITH_NEW_PREVIEW)
        case IDM_PREVIEW_CLASSIC:
        {
            PREVIEW_DATA *pPreview = GetPreviewData();
            pPreview->bClassicPreview = !pPreview->bClassicPreview;

            if (pPreview->brushes[COLOR_DESKTOP])
                DeleteObject(pPreview->brushes[COLOR_DESKTOP]);
            pPreview->brushes[COLOR_DESKTOP] = CreateDesktopBrush(pPreview);

            CalculateItemsRects(pPreview);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
#endif
        }
        return 0;
#endif  /* defined(WITH_PREVIEW_CONTEXT_MENU) */
    }

    return PreviewWndProc(hWnd, uMsg, wParam, lParam);
}


#undef GetPreviewData

_Success_(return)
BOOL RegisterSchemePreviewControl(void)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = SchemePreviewWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_propSheet.hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = schemePreviewWndClass;
    wc.hIconSm = NULL;

    return RegisterClassEx(&wc) != (ATOM)0;
}

void UnregisterSchemePreviewControl(void)
{
    UnregisterClass(schemePreviewWndClass, g_propSheet.hInstance);
}
