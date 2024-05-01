/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Classic Visual Style property page
 *
 * PROGRAMMERS: Timo Kreuzer (timo[dot]kreuzer[at]web[dot]de)
 *              Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "bg.h"
#include "clasdraw.h"
#include "classtyl.h"
#include "colorbtn.h"
#include "draw.h"
#include "mincrt.h"
#include "preview.h"
#include "resource.h"
#include "util.h"
#include "wndclas.h"

#include <CommCtrl.h>

#define MIN_FONT_SIZE  6
#define MAX_FONT_SIZE 24

static HWND g_hDlg;

/* Flag to determine if a size change was made by the user or internally */
static BOOL g_bRefreshingSizes = TRUE;

static HBITMAP g_hbmpColors[3];
static RECT    g_rcColorButton;

static HFONT g_hBoldFont;
static HFONT g_hItalicFont;
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static HFONT g_hUnderlineFont;
#endif

static int g_currentElement;
static BOOL g_bInitializing;

static HWND g_hToolTipColors[3];
static HWND g_hToolTipSizes[2];
static HWND g_hToolTipFont;

/* Implemented in dlgcsave.c */
INT_PTR CALLBACK SaveSchemeDlgProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Implemented in dlgcren.c */
INT_PTR CALLBACK RenameSchemeDlgProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#if WINVER < WINVER_2K

#if defined(COLOR_GRADIENTACTIVECAPTION)
#undef COLOR_GRADIENTACTIVECAPTION
#endif
#define COLOR_GRADIENTACTIVECAPTION    -1

#if defined(COLOR_GRADIENTINACTIVECAPTION)
#undef COLOR_GRADIENTINACTIVECAPTION
#endif
#define COLOR_GRADIENTINACTIVECAPTION  -1

#endif

enum colorButtons
{
    COLORBUTTON_1,
    COLORBUTTON_2,
    COLORBUTTON_FONT,
    NUM_COLORBUTTONS
};

typedef struct tagASSIGNMENT
{
    int size1;
    int size2;
    int color1;
    int color2;
    int font;
    int fontColor;
} ASSIGNMENT;

/* Assign the color and metric numbers to each element of the combo box */
/*       Size 1                Size 2               Color 1              Color 2                     Font           Fontcolor           */
static const ASSIGNMENT g_assignment[NUM_ELEMENTS] =
{
/* 0*/ { -1,                   -1,                  COLOR_DESKTOP,       -1,                         FONT_DESKTOP,  -1                   },
/* 1*/ { -1,                   -1,                  COLOR_APPWORKSPACE,  -1,                         -1,            -1                   },
/* 2*/ { -1,                   -1,                  COLOR_WINDOW,        COLOR_WINDOWFRAME,          -1,            COLOR_WINDOWTEXT     },
/* 3*/ { SIZE_MENUHEIGHT,      SIZE_MENUWIDTH,      COLOR_MENU,          -1,                         FONT_MENU,     COLOR_MENUTEXT       },
/* 4*/ { SIZE_CAPTIONHEIGHT,   SIZE_CAPTIONWIDTH,   COLOR_ACTIVECAPTION, COLOR_GRADIENTACTIVECAPTION,FONT_CAPTION,  COLOR_CAPTIONTEXT    },
/* 5*/ { SIZE_CAPTIONHEIGHT,SIZE_CAPTIONWIDTH,COLOR_INACTIVECAPTION,COLOR_GRADIENTINACTIVECAPTION,FONT_CAPTION,COLOR_INACTIVECAPTIONTEXT },
/* 6*/ { SIZE_SMCAPTIONHEIGHT, SIZE_SMCAPTIONWIDTH, -1,                  -1,                         FONT_SMCAPTION,-1                   },
/* 7*/ { SIZE_BORDER,          -1,                  COLOR_ACTIVEBORDER,  -1,                         -1,            -1                   },
/* 8*/ { SIZE_BORDER,          -1,                  COLOR_INACTIVEBORDER,-1,                         -1,            -1                   },
/* 9*/ { SIZE_SCROLLWIDTH,     SIZE_SCROLLHEIGHT,   COLOR_SCROLLBAR,     -1,                         -1,            -1                   },
/*10*/ { -1,                   -1,                  COLOR_3DFACE,        -1,                         -1,            COLOR_BTNTEXT        },
/*11*/ { -1,                   -1,                  COLOR_3DSHADOW,      COLOR_3DDKSHADOW,           -1,            -1                   },
/*12*/ { -1,                   -1,                  COLOR_3DHILIGHT,     COLOR_3DLIGHT,              -1,            -1                   },
/*13*/ { -1,                   -1,                  COLOR_HIGHLIGHT,     -1,                         -1,            COLOR_HIGHLIGHTTEXT  },
/*14*/ { -1,                   -1,                  -1,                  -1,                         -1,            COLOR_GRAYTEXT       },
/*15*/ { -1,                   -1,                  COLOR_INFOBK,        -1,                         FONT_TOOLTIP,  COLOR_INFOTEXT       },
/*16*/ { -1,                   -1,                  -1,                  -1,                         FONT_MESSAGE,  COLOR_WINDOWTEXT     },
#if WINVER >= WINVER_2K
/*17*/ { -1,                   -1,                  -1,                  -1,                         -1,            COLOR_HOTLIGHT       },
#endif
#if WINVER >= WINVER_XP
/*18*/ { -1,                   -1,                  COLOR_MENUBAR,       COLOR_MENUHILIGHT,          -1,            -1                   },
#endif
#if WINVER >= WINVER_VISTA
/*19*/ { SIZE_PADDEDBORDER,    -1,                  -1,                  -1,                         -1,            -1                   }
#endif
};

static
void EnableDeleteButtons(BOOL bEnable)
{
#define EnableControl(iControl) \
    EnableWindow(GetDlgItem(g_hDlg, iControl), bEnable)

    EnableControl(IDC_CLASSIC_RENAME);
    EnableControl(IDC_CLASSIC_DELETE);

#undef EnableControl
}

static
void UpdateColorButton(int iControl, int iButton, int iColor)
{
    HWND hColorBtn = GetDlgItem(g_hDlg, iControl);
    if (!hColorBtn)
        return;

    if (iColor >= 0)
    {
        HBRUSH hBrush = CreateSolidBrush(g_schemes->scheme.colors[iColor]);
        FillColorButton(
            hColorBtn, &g_rcColorButton, g_hbmpColors[iButton], hBrush);
        DeleteObject(hBrush);

        EnableWindow(hColorBtn, TRUE);

        /* Note: The LTEXT control id must precede immediatly the color button */
        EnableWindow(GetDlgItem(g_hDlg, iControl - 1), TRUE);

        return;
    }

    DisableColorButton(hColorBtn);
    EnableWindow(GetDlgItem(g_hDlg, iControl - 1), FALSE);
}

static
void UpdateColorButtonsBorder(void)
{
    /* Add the border back temporarily */
    InflateRect(&g_rcColorButton, 1, 1);

    HWND hColorBtn;
    HBRUSH hBrush = CreateSolidBrush(g_schemes->scheme.colors[COLOR_BTNTEXT]);

#define DrawBtnBorder(iControl, iButton) \
    hColorBtn = GetDlgItem(g_hDlg, iControl); \
    if (hColorBtn && IsWindowEnabled(hColorBtn)) \
        DrawColorButtonBorder(hColorBtn, &g_rcColorButton, \
            g_hbmpColors[iButton], hBrush)

    DrawBtnBorder(IDC_CLASSIC_COLOR1, COLORBUTTON_1);
    DrawBtnBorder(IDC_CLASSIC_COLOR2, COLORBUTTON_2);
    DrawBtnBorder(IDC_CLASSIC_FONTCOLOR, COLORBUTTON_FONT);
#undef DrawBtnBorder

    DeleteObject(hBrush);

    InflateRect(&g_rcColorButton, -1, -1);
}

static
void UpdateColorButtons(void)
{
    ASSIGNMENT assignment = g_assignment[g_currentElement];

    /* Update the color of the color buttons */
    UpdateColorButton(IDC_CLASSIC_COLOR1, 0, assignment.color1);
    UpdateColorButton(IDC_CLASSIC_COLOR2, 1, assignment.color2);
    UpdateColorButton(IDC_CLASSIC_FONTCOLOR, 2, assignment.fontColor);
}

static
void UpdateSizeControl(int metric, int textControl, int udControl)
{
    if (metric < 0)
    {
        SetDlgItemText(g_hDlg, textControl, TEXT(""));
        return;
    }

    SendDlgItemMessage(g_hDlg, udControl, UDM_SETRANGE, 0L,
        MAKELONG(g_sizeRanges[metric].max, g_sizeRanges[metric].min));

    int value = GetSchemeMetric(&g_schemes->scheme.ncMetrics, metric);
    SetDlgItemInt(g_hDlg, textControl, value, FALSE);
}

static
void UpdateSize1Control(void)
{
    UpdateSizeControl(g_assignment[g_currentElement].size1,
        IDC_CLASSIC_SIZE1_E, IDC_CLASSIC_SIZE1_UD);
}

static
void UpdateSize2Control(void)
{
    UpdateSizeControl(g_assignment[g_currentElement].size2,
        IDC_CLASSIC_SIZE2_E, IDC_CLASSIC_SIZE2_UD);
}

static
void UnselectSchemeSize(void)
{
    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_SCHEMESIZE,
        CB_SETCURSEL, (WPARAM)-1, 0L);
}

static
void UnselectScheme(void)
{
    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_SCHEME,
        CB_SETCURSEL, (WPARAM)-1, 0L);

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_SCHEMESIZE,
        CB_RESETCONTENT, 0L, 0L);
}

static
void UpdateSize(int value, int metric, int textControl)
{
    int previousValue = GetSchemeMetric(&g_schemes->scheme.ncMetrics, metric);

    value = SetSchemeMetric(&g_schemes->scheme.ncMetrics, metric, value);
    if (value < 0 || value == previousValue)
        return;

    /* Update the second size proportionally */
    int size2metric;
    switch (metric)
    {
    case SIZE_SCROLLWIDTH:
        size2metric = SIZE_SCROLLHEIGHT;
        break;

    case SIZE_CAPTIONHEIGHT:
        size2metric = SIZE_CAPTIONWIDTH;
        break;

    case SIZE_SMCAPTIONHEIGHT:
        size2metric = SIZE_SMCAPTIONWIDTH;
        break;

    case SIZE_MENUHEIGHT:
        size2metric = SIZE_MENUWIDTH;
        break;

    default:
        size2metric = 0;
    }

    if (size2metric > 0)
    {
        int size2 = GetSchemeMetric(&g_schemes->scheme.ncMetrics, size2metric);
        size2 = MulDiv(size2, value, previousValue);
        SetSchemeMetric(&g_schemes->scheme.ncMetrics, size2metric, size2);

        g_bRefreshingSizes = TRUE;
        UpdateSize2Control();
        g_bRefreshingSizes = FALSE;
    }

    HWND hPreview = GetDlgItem(g_hDlg, IDC_CLASSIC_PREVIEW);
    if (hPreview)
    {
        SendMessage(hPreview, PVM_SETSIZE, (WPARAM)textControl, LOWORD(value));
        SendMessage(hPreview, PVM_UPDATESCHEME, 0L, (LPARAM)&g_schemes->scheme);
    }

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void DisableFontControls(void)
{
    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTNAME, CB_SETCURSEL, (WPARAM)-1, 0L);
    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTSIZE, CB_SETCURSEL, (WPARAM)-1, 0L);
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTSMOOTHING, CB_SETCURSEL, (WPARAM)-1, 0L);
#endif

    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTBOLD, BM_SETCHECK, 0L, 0L);
    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTITALIC, BM_SETCHECK, 0L, 0L);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTUNDERLINE, BM_SETCHECK, 0L, 0L);
#endif

    SetDlgItemText(g_hDlg, IDC_CLASSIC_FONTWIDTH_E, TEXT(""));
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SetDlgItemText(g_hDlg, IDC_CLASSIC_FONTANGLE_E, TEXT(""));
#endif
}

#ifdef SelectFont
#  undef SelectFont
#endif

static
void SelectFont(_In_ const LOGFONT *plfFont)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTNAME);
    if (!hCombo)
        return;

    int comboIndex = (int)SendMessage(
        hCombo, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)plfFont->lfFaceName);

    if (comboIndex < -1)
        comboIndex = -1;

    SendMessage(hCombo, CB_SETCURSEL, (WPARAM)comboIndex, 0L);
}

static
void SelectFontSize(_In_ const LOGFONT *plfFont)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSIZE);
    if (!hCombo)
        return;

    int size = FontLogToPt(plfFont->lfHeight);
    int comboIndex;

    TCHAR szSize[4];
    if (wsprintf(szSize, TEXT("%d"), size) < 1)
    {
        /* Sane default */
        szSize[0] = TEXT('8');
        szSize[1] = TEXT('\0');
    }

    if (size < MIN_FONT_SIZE)
        comboIndex = 0;
    else if (size > MAX_FONT_SIZE)
        comboIndex = MAX_FONT_SIZE - MIN_FONT_SIZE;
    else
    {
        comboIndex = (int)SendMessage(
            hCombo, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szSize);
        if (comboIndex < 0)
            comboIndex = 2;  /* "8" */
    }

    SendMessage(hCombo, CB_SETCURSEL, (WPARAM)comboIndex, 0L);

#if defined(WITH_EDITABLE_FONTSIZE)
    SetWindowText(hCombo, szSize);
#endif
}

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static
void SelectFontSmoothing(_In_ const LOGFONT *plfFont)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSMOOTHING);
    if (!hCombo)
        return;

    if (!SelectComboByItemData(hCombo, (LRESULT)plfFont->lfQuality))
        SendMessage(hCombo, CB_SETCURSEL, 0L, 0L);
}
#endif

static
void UpdateFontControls(void)
{
    int iFont = g_assignment[g_currentElement].font;

    if (iFont < 0)
    {
        DisableFontControls();
        return;
    }

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    SelectFont(plfFont);
    SelectFontSize(plfFont);

    SetDlgItemInt(g_hDlg, IDC_CLASSIC_FONTWIDTH_E,
        FontLogToPt(plfFont->lfWidth), FALSE);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SetDlgItemInt(g_hDlg, IDC_CLASSIC_FONTANGLE_E,
        plfFont->lfEscapement / 10, FALSE);
#endif

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FONTBOLD, BM_SETCHECK,
        (WPARAM)(plfFont->lfWeight >= FW_BOLD), 0L);

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FONTITALIC, BM_SETCHECK,
        (WPARAM)plfFont->lfItalic, 0L);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FONTUNDERLINE, BM_SETCHECK,
        (WPARAM)plfFont->lfUnderline, 0L);
#endif

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SelectFontSmoothing(plfFont);
#endif
}

/* Set control values according to the selected element and scheme size */
static
void UpdateSizeControls(void)
{
    g_bRefreshingSizes = TRUE;

    UpdateSize1Control();
    UpdateSize2Control();
    UpdateFontControls();

    g_bRefreshingSizes = FALSE;
}

static
void UpdateToolTips(void)
{
    TCHAR toolTipText[17];
    int i;

#define UpdTT(part, iControl, hToolTip, namesArray) \
    i = g_assignment[g_currentElement].part; \
    if ((i >= 0) && \
        (wsprintf(toolTipText, TEXT("%d - %s"), i, namesArray[i]) >= 5)) \
            UpdateToolTipText(g_hDlg, iControl, hToolTip, toolTipText)

    UpdTT(color1,    IDC_CLASSIC_COLOR1,    g_hToolTipColors[0], g_colorNames);
    UpdTT(color2,    IDC_CLASSIC_COLOR2,    g_hToolTipColors[1], g_colorNames);
    UpdTT(fontColor, IDC_CLASSIC_FONTCOLOR, g_hToolTipColors[2], g_colorNames);
    UpdTT(size1,     IDC_CLASSIC_SIZE1_E,   g_hToolTipSizes[0],  g_sizeNames);
    UpdTT(size2,     IDC_CLASSIC_SIZE2_E,   g_hToolTipSizes[1],  g_sizeNames);
    UpdTT(font,      IDC_CLASSIC_FONTNAME,  g_hToolTipFont,      g_fontNames);

#undef UpdTT
}

/* Enable or disable controls according to the selected element */
static
void UpdateElementControls(void)
{
    ASSIGNMENT assignment = g_assignment[g_currentElement];
    BOOL bEnable;

#define EnableControl(iControl) \
    EnableWindow(GetDlgItem(g_hDlg, iControl), bEnable)

    /* Note: Requires that the ids of the combo box, or the edit and up-down
     * controls follow immediately the LTEXT control.
     */

#define EnableComboControl(iControl) \
    EnableControl(iControl); \
    EnableControl(iControl + 1)

#define EnableNumberControl(iControl) \
    EnableComboControl(iControl); \
    EnableControl(iControl + 2)

    bEnable = (assignment.size1 >= 0);
    EnableNumberControl(IDC_CLASSIC_SIZE1_T);

    bEnable = (assignment.size2 >= 0);
    EnableNumberControl(IDC_CLASSIC_SIZE2_T);

    bEnable = (assignment.font >= 0);
    EnableComboControl(IDC_CLASSIC_FONTNAME_T);
    EnableComboControl(IDC_CLASSIC_FONTSIZE_T);
    EnableNumberControl(IDC_CLASSIC_FONTWIDTH_T);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    EnableNumberControl(IDC_CLASSIC_FONTANGLE_T);
#endif
    EnableNumberControl(IDC_CLASSIC_FONTSTYLE_T);
    EnableControl(IDC_CLASSIC_FONTSTYLE_T);
    EnableControl(IDC_CLASSIC_FONTBOLD);
    EnableControl(IDC_CLASSIC_FONTITALIC);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    EnableControl(IDC_CLASSIC_FONTUNDERLINE);
#endif
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    EnableComboControl(IDC_CLASSIC_FONTSMOOTHING_T);
#endif

#undef EnableNumberControl
#undef EnableComboControl
#undef EnableControl

    UpdateColorButtons();
    UpdateSizeControls();
    UpdateToolTips();
}

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static
void SetSchemeCheckBoxes(void)
{
#if WINVER >= WINVER_2K
    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_GRADIENTCAPTIONS, BM_SETCHECK,
        (WPARAM)g_schemes->scheme.bGradientCaptions, 0L);
#endif

#if WINVER >= WINVER_XP
    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_FLATMENUS, BM_SETCHECK,
        (WPARAM)g_schemes->scheme.bFlatMenus, 0L);
#endif
}
#endif

#if WINVER >= WINVER_2K
/* Returns if the second color was changed */
static
BOOL HandleCaptionColorChange(
    int iColor, int iGradientColor, COLORREF previousColor)
{
    /* Conditions: */

    /* 1. Gradient title bars are disabled */
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    if (g_schemes->scheme.bGradientCaptions)
        return FALSE;
#endif

    /* 2. The two colors were the same */
    if (previousColor != g_schemes->scheme.colors[iGradientColor])
        return FALSE;

    g_schemes->scheme.colors[iGradientColor] = g_schemes->scheme.colors[iColor];
    UpdateColorButton(IDC_CLASSIC_COLOR2, COLORBUTTON_2, iGradientColor);

    return TRUE;
}
#endif

static
void HandleColorButtonPress(int iButton)
{
    int iControl = 0;
    int iColor = 0;

    /* Get the color index from the element index and button number */
    switch (iButton)
    {
    case COLORBUTTON_1:
        iColor = g_assignment[g_currentElement].color1;
        iControl = IDC_CLASSIC_COLOR1;
        break;

    case COLORBUTTON_2:
        iColor = g_assignment[g_currentElement].color2;
        iControl = IDC_CLASSIC_COLOR2;
        break;

    case COLORBUTTON_FONT:
        iColor = g_assignment[g_currentElement].fontColor;
        iControl = IDC_CLASSIC_FONTCOLOR;
        break;
    }

    COLORREF previousColor = g_schemes->scheme.colors[iColor];
    if (!ChooseCustomColor(&g_schemes->scheme.colors[iColor], g_hDlg))
        return;

    BOOL changed = FALSE;

    /* Special cases */
    switch (iColor)
    {
    case COLOR_SCROLLBAR:
        /* Respect the scrollbar background color */
        if (g_schemes->scheme.colors[iColor] !=
            g_schemes->scheme.colors[COLOR_3DHILIGHT])
        {
            g_schemes->scheme.colors[COLOR_3DHILIGHT] =
                g_schemes->scheme.colors[iColor];
            changed = TRUE;
        }
        break;

#if WINVER >= WINVER_2K
    case COLOR_ACTIVECAPTION:
        changed = HandleCaptionColorChange(
            iColor, COLOR_GRADIENTACTIVECAPTION, previousColor);
        break;

    case COLOR_INACTIVECAPTION:
        changed = HandleCaptionColorChange(
            iColor, COLOR_GRADIENTINACTIVECAPTION, previousColor);
        break;
#endif
    }

    if ((g_schemes->scheme.colors[iColor] != previousColor))
    {
        UpdateColorButton(iControl, iButton, iColor);
        changed = TRUE;
    }

    if (!changed)
        return;

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
        PV_UPDATE_ONLY_COLORS, (LPARAM)&g_schemes->scheme);

    UnselectScheme();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void FixControlsHeight(void)
{
    /* Get combobox height */
    RECT rc;
    if (!GetWindowRect(GetDlgItem(g_hDlg, IDC_CLASSIC_ELEMENT), &rc))
        return;
    int height = rc.bottom - rc.top;

    if (!GetWindowRect(g_hDlg, &rc))
        return;
    int xOffset = -rc.left;
    int yOffset = -rc.top;

    HWND hControl;

    /* Leave in the same position, but with the height of combo boxes */
#define Resize(iControl) \
    hControl = GetDlgItem(g_hDlg, iControl); \
    if (GetWindowRect(hControl, &rc)) \
        SetWindowPos(hControl, NULL, 0, 0, rc.right - rc.left, height, \
            SWP_NOMOVE | \
            SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOCOPYBITS | \
            SWP_NOOWNERZORDER)

    /* Like Resize(), but also extend one pixel to the top and bottom */
#define Move(iControl) \
    hControl = GetDlgItem(g_hDlg, iControl); \
    if (GetWindowRect(hControl, &rc)) { \
        OffsetRect(&rc, xOffset, yOffset); \
        SetWindowPos(hControl, NULL, rc.left, rc.top - 1, \
            rc.right - rc.left, height + 2, \
            SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOCOPYBITS | \
            SWP_NOOWNERZORDER); \
    }

    Move(IDC_CLASSIC_SAVE);
    Move(IDC_CLASSIC_RENAME);
    Move(IDC_CLASSIC_DELETE);

    Resize(IDC_CLASSIC_SIZE1_E);
    Resize(IDC_CLASSIC_SIZE1_UD);
    Resize(IDC_CLASSIC_SIZE2_E);
    Resize(IDC_CLASSIC_SIZE2_UD);
    Resize(IDC_CLASSIC_FONTWIDTH_E);
    Resize(IDC_CLASSIC_FONTWIDTH_UD);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    Resize(IDC_CLASSIC_FONTANGLE_E);
    Resize(IDC_CLASSIC_FONTANGLE_UD);
#endif

    Move(IDC_CLASSIC_COLOR1);
    Move(IDC_CLASSIC_COLOR2);
    Move(IDC_CLASSIC_FONTCOLOR);
    Move(IDC_CLASSIC_FONTBOLD);
    Move(IDC_CLASSIC_FONTITALIC);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    Move(IDC_CLASSIC_FONTUNDERLINE);
#endif

#undef Resize
#undef Move
}

#if defined(WITH_EDITABLE_FONTSIZE) && WINVER >= WINVER_2K
static
void RightAlignCombo(HWND hCombo)
{
    COMBOBOXINFO cbi;
    cbi.cbSize = sizeof(COMBOBOXINFO);
    if (!GetComboBoxInfo(hCombo, &cbi))
        return;

    LONG style = GetWindowLong(cbi.hwndItem, GWL_STYLE);
    style |= ES_RIGHT;
    SetWindowLong(cbi.hwndItem, GWL_STYLE, style);

    style = GetWindowLong(cbi.hwndList, GWL_EXSTYLE);
    style |= WS_EX_RIGHT;
    SetWindowLong(cbi.hwndList, GWL_EXSTYLE, style);
}

static
void RightAlignFontSizeCombo(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSIZE);
    if (hCombo)
        RightAlignCombo(hCombo);
}
#endif  /* defined(WITH_EDITABLE_FONTSIZE) && WINVER >= WINVER_2K */

static
void SetButtonFont(_Inout_ HFONT *pGlobalFont, LOGFONT *pLf, WORD iControl)
{
    HFONT hFont = CreateFontIndirect(pLf);
    if (!hFont)
        return;

    if (*pGlobalFont)
        DeleteObject(*pGlobalFont);

    *pGlobalFont = hFont;
    SendDlgItemMessage(
        g_hDlg, iControl, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
}

static
void SetButtonsFont(void)
{
    HFONT hFont = (HFONT)SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTBOLD, WM_GETFONT, 0L, 0L);
    LOGFONT lf;
    GetObject(hFont, sizeof(LOGFONT), &lf);

    lf.lfWeight = FW_BOLD;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    SetButtonFont(&g_hBoldFont, &lf, IDC_CLASSIC_FONTBOLD);

    lf.lfWeight = FW_NORMAL;
    lf.lfItalic = TRUE;
    SetButtonFont(&g_hItalicFont, &lf, IDC_CLASSIC_FONTITALIC);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    lf.lfItalic = FALSE;
    lf.lfUnderline = TRUE;
    SetButtonFont(&g_hUnderlineFont, &lf, IDC_CLASSIC_FONTUNDERLINE);
#endif
}

/* Draw bitmaps for the colorbuttons */
static
void InitColorButtons(void)
{
    /* Note: This assumes all the color buttons are the same size */
    if (!(GetWindowRect(
        GetDlgItem(g_hDlg, IDC_CLASSIC_COLOR1), &g_rcColorButton) &&
        OffsetRect(&g_rcColorButton,
            -g_rcColorButton.left, -g_rcColorButton.top)))
    {
        /* Fallback */
        g_rcColorButton.left = 0;
        g_rcColorButton.top = 0;
        g_rcColorButton.right = ScaleForDpi(46);
        g_rcColorButton.bottom = ScaleForDpi(23);
    }

    g_rcColorButton.right -= ScaleForDpi(13);
    g_rcColorButton.bottom -= ScaleForDpi(11);

    HWND hColorBtn;

#define InitBtn(iControl, iButton) \
    hColorBtn = GetDlgItem(g_hDlg, iControl); \
    if (hColorBtn) g_hbmpColors[iButton] = \
        CreateColorButtonBitmap(hColorBtn, &g_rcColorButton)

    InitBtn(IDC_CLASSIC_COLOR1, COLORBUTTON_1);
    InitBtn(IDC_CLASSIC_COLOR2, COLORBUTTON_2);
    InitBtn(IDC_CLASSIC_FONTCOLOR, COLORBUTTON_FONT);
#undef InitBtn

    /* Take out the border */
    InflateRect(&g_rcColorButton, -1, -1);
}

static
void SetIcon(int controlId, int iconId)
{
    HICON hIcon = (HICON)LoadImage(
        g_propSheet.hInstance, MAKEINTRESOURCE(iconId), IMAGE_ICON, 0, 0,
        LR_DEFAULTCOLOR);
    if (!hIcon)
        return;

    SendDlgItemMessage(
        g_hDlg, controlId, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
}

static
void SetButtonIcons(void)
{
    SetIcon(IDC_CLASSIC_SAVE,   IDI_SAVEICON);
    SetIcon(IDC_CLASSIC_RENAME, IDI_RENAMEICON);
    SetIcon(IDC_CLASSIC_DELETE, IDI_DELICON);
}

static
void CreateToolTips(void)
{
    CreateToolTip(g_hDlg, IDC_CLASSIC_SAVE,   IDS_SAVESCHEME);
    CreateToolTip(g_hDlg, IDC_CLASSIC_RENAME, IDS_RENAMESCHEME);
    CreateToolTip(g_hDlg, IDC_CLASSIC_DELETE, IDS_DELETESCHEME);

    CreateToolTip(g_hDlg, IDC_CLASSIC_FONTBOLD,      IDS_BOLD);
    CreateToolTip(g_hDlg, IDC_CLASSIC_FONTITALIC,    IDS_ITALIC);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    CreateToolTip(g_hDlg, IDC_CLASSIC_FONTUNDERLINE, IDS_UNDERLINE);
#endif

    g_hToolTipColors[0] = CreateToolTip(g_hDlg, IDC_CLASSIC_COLOR1,    0);
    g_hToolTipColors[1] = CreateToolTip(g_hDlg, IDC_CLASSIC_COLOR2,    0);
    g_hToolTipColors[2] = CreateToolTip(g_hDlg, IDC_CLASSIC_FONTCOLOR, 0);
    g_hToolTipSizes[0]  = CreateToolTip(g_hDlg, IDC_CLASSIC_SIZE1_E,   0);
    g_hToolTipSizes[1]  = CreateToolTip(g_hDlg, IDC_CLASSIC_SIZE2_E,   0);
    g_hToolTipFont      = CreateToolTip(g_hDlg, IDC_CLASSIC_FONTNAME,  0);
}

static
void LoadElementCombo(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_ELEMENT);
    if (!hCombo)
        return;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    INT_PTR listIndex;
    TCHAR text[80];
    for (int element = 0; element < NUM_ELEMENTS; element++)
    {
        if (LoadString(
            g_propSheet.hInstance, IDS_ELEMENT_0 + element, text, 79) <= 0)
        {
            continue;
        }

        listIndex = SendMessage(hCombo, CB_ADDSTRING, 0L, (LPARAM)&text);
        if (listIndex < 0)
            continue;

        SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)listIndex, (LPARAM)element);

        /* Select "Desktop" */
        if (element == ELEMENT_DESKTOP)
            SendMessage(hCombo, CB_SETCURSEL, (WPARAM)listIndex, 0L);
    }

    g_currentElement = ELEMENT_DESKTOP;
}

/* Callback function to add the installed fonts to the font combo */
static
int CALLBACK EnumFontFamExProc(
    _In_ const ENUMLOGFONTEX *lpelfe, _In_ const NEWTEXTMETRICEX *lpntme,
    DWORD dwFontType, LPARAM lParam
)
{
    UNREFERENCED_PARAMETER(lpntme);
    UNREFERENCED_PARAMETER(dwFontType);

    /* Do not add fonts starting with @ */
    if (lpelfe->elfLogFont.lfFaceName[0] == TEXT('@'))
        return 2;

    SendMessage((HWND)lParam, CB_ADDSTRING, 0,
        (LPARAM) & (lpelfe->elfLogFont.lfFaceName));
    return 1;
}

static
void LoadFontCombo(HWND hCombo)
{
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen)
        return;

    LOGFONT lfFont;

    /* Contrary to what the documentation says,
     * DEFAULT_CHARSET may enumerate multiple fonts with the same name
     */
    lfFont.lfCharSet = ANSI_CHARSET;
    lfFont.lfFaceName[0] = TEXT('\0');
    lfFont.lfPitchAndFamily = DEFAULT_PITCH;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    EnumFontFamiliesEx(
        hdcScreen, &lfFont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)hCombo, 0);

    ReleaseDC(NULL, hdcScreen);
}

static
void LoadFontSizeCombo(HWND hCombo)
{
    TCHAR szSize[3];
    int index;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    for (int size = MIN_FONT_SIZE; size <= MAX_FONT_SIZE; size++)
    {
        if (wsprintf(szSize, TEXT("%d"), size) < 1)
            continue;

        index = (int)SendMessage(hCombo, CB_ADDSTRING, 0L, (LPARAM)&szSize);
        if (index < 0)
            continue;

        SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)index, (LPARAM)size);
    }
}

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static
void LoadFontSmoothingCombo(HWND hCombo)
{
    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    TCHAR text[20];
    int index;

#define AddSmoothingCombo(ids, data) \
    if (LoadString(g_propSheet.hInstance, ids, text, 19) > 0) \
    { \
        index = (int)SendMessage(hCombo, CB_ADDSTRING, 0L, (LPARAM)text); \
        if (index >= 0) \
            SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)index, (LPARAM)data); \
    }

    AddSmoothingCombo(IDS_FONTSMOOTHING_DEFAULT, DEFAULT_QUALITY);
    AddSmoothingCombo(IDS_FONTSMOOTHING_OFF, NONANTIALIASED_QUALITY);
    AddSmoothingCombo(IDS_FONTSMOOTHING_CT, CLEARTYPE_QUALITY);
    AddSmoothingCombo(IDS_FONTSMOOTHING_CTN, CLEARTYPE_NATURAL_QUALITY);

#undef AddSmoothingCombo
}
#endif

static
void LoadFontCombos(void)
{
    HWND hCombo;

    hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTNAME);
    if (hCombo)
        LoadFontCombo(hCombo);

    hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSIZE);
    if (hCombo)
        LoadFontSizeCombo(hCombo);

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSMOOTHING);
    if (hCombo)
        LoadFontSmoothingCombo(hCombo);
#endif
}

static
void LoadColorSchemeCombo(void)
{
    if (!g_schemes->schemes)
        return;

    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEME);
    if (!hCombo)
        return;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    TCHAR *szCurrent;
    if (AllocAndLoadString(&szCurrent, IDS_CURRENT_ELEMENT) > 0)
    {
        int listIndex = (int)SendMessage(
            hCombo, CB_ADDSTRING, 0L, (LPARAM)szCurrent);
        if (listIndex >= 0)
        {
            SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)listIndex, (LPARAM)NULL);
            if (!g_schemes->selected.color)
                SendMessage(hCombo, CB_SETCURSEL, (WPARAM)listIndex, 0L);
        }
        Free((HLOCAL)szCurrent);
    }

    AddSchemesToCombo(hCombo, g_schemes->schemes, g_schemes->selected.color);
}

static
void LoadSizeCombo(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEMESIZE);
    if (!hCombo)
        return;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    if (!g_schemes->selected.color)
    {
        EnableDeleteButtons(FALSE);
        return;
    }

    BOOL bSelected = AddSchemesToCombo(
        hCombo, g_schemes->selected.color->sizes, g_schemes->selected.size);

    EnableDeleteButtons(bSelected);
}

static
void InitPage(void)
{
    g_schemes = (SCHEMES_DATA *)Alloc(HEAP_ZERO_MEMORY, sizeof(SCHEMES_DATA));
    if (!g_schemes)
    {
        ShowMessageFromResource(NULL, IDS_ERROR_MEM, IDS_ERROR, MB_OK);
        DestroyWindow(g_hDlg);
        return;
    }

    g_bInitializing = TRUE;
    FixControlsHeight();

#if defined(WITH_EDITABLE_FONTSIZE) && WINVER >= WINVER_2K
    RightAlignFontSizeCombo();
#endif

    SetButtonsFont();
    SetButtonIcons();
    CreateToolTips();
    InitColorButtons();

    LoadCurrentScheme(&g_schemes->scheme);
#if defined(WITH_NEW_PREVIEW)
    g_schemes->scheme.desktopPattern = LoadCurrentDesktopPattern();
#endif

    LoadElementCombo();
    LoadFontCombos();

    LoadSchemesList();

    GetActiveScheme(g_schemes->schemes, &g_schemes->selected);
    LoadColorSchemeCombo();
    LoadSizeCombo();

    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTWIDTH_UD, UDM_SETRANGE, 0L, MAKELONG(24, 0));

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTANGLE_UD, UDM_SETRANGE, 0L, MAKELONG(359, 0));
#endif

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
        0L, (LPARAM)&g_schemes->scheme);

    UpdateElementControls();
    UpdateSizeControls();
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SetSchemeCheckBoxes();
#endif

    g_bInitializing = FALSE;
}

static
void DestroyPage(void)
{
    if (g_hBoldFont)
        DeleteObject(g_hBoldFont);
    if (g_hItalicFont)
        DeleteObject(g_hItalicFont);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    if (g_hUnderlineFont)
        DeleteObject(g_hUnderlineFont);
#endif

    for (int iButton = 0; iButton < NUM_COLORBUTTONS; iButton++)
        if (g_hbmpColors[iButton])
            DeleteObject(g_hbmpColors[iButton]);

    if (!g_schemes)
        return;

    g_schemes->selected.color = NULL;
    g_schemes->selected.size = NULL;

    FreeSchemes(&g_schemes->schemes);

#if defined(WITH_NEW_PREVIEW)
    if (g_schemes->scheme.desktopPattern)
        Free(g_schemes->scheme.desktopPattern);
#endif

    Free(g_schemes);
    g_schemes = NULL;
}

static
void ChangeScheme(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEME);
    if (!hCombo)
        return;

    INT_PTR sel = SendMessage(hCombo, CB_GETCURSEL, 0L, 0L);
    if (sel < 0)
    {
        SendDlgItemMessage(g_hDlg, IDC_CLASSIC_SCHEMESIZE,
            CB_RESETCONTENT, 0L, 0L);
        return;
    }

    g_schemes->selected.color =
        (SCHEME *)SendMessage(hCombo, CB_GETITEMDATA, (WPARAM)sel, 0L);
    if (g_schemes->selected.color)
    {
        g_schemes->selected.size = GetActiveSchemeSize(g_schemes->selected.color);
        if (!g_schemes->selected.size)
        {
            /* Select the first one */
            g_schemes->selected.size = g_schemes->selected.color->sizes;
        }

        if (!LoadScheme(&g_schemes->scheme, &g_schemes->selected))
        {
            /* Deleted from the registry */
            DeleteSchemeFromList(
                &g_schemes->schemes, &g_schemes->selected.color);
            SendMessage(hCombo, CB_DELETESTRING, (WPARAM)sel, 0L);
            SendMessage(hCombo, CB_SETCURSEL, 0L, 0L);
            return;
        }
    }
    else
    {
        g_schemes->selected.size = NULL;
        LoadCurrentScheme(&g_schemes->scheme);
#if defined(WITH_NEW_PREVIEW)
        g_schemes->scheme.desktopPattern = LoadCurrentDesktopPattern();
#endif
    }

    LoadSizeCombo();

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME, 0L,
        (LPARAM)&g_schemes->scheme);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    SetSchemeCheckBoxes();
#endif
    UpdateColorButtons();
    UpdateSizeControls();

    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void ChangeSchemeSize(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEMESIZE);
    if (hCombo)
        g_schemes->selected.size = (SCHEME *)GetComboBoxSelectedData(hCombo);

    /* Only the size changed; do not reload the colors */
    if (!LoadSchemeSize(&g_schemes->scheme, &g_schemes->selected))
        return;

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
        0L, (LPARAM)&g_schemes->scheme);

    UpdateSizeControls();

    EnableDeleteButtons(TRUE);
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void ChangeElement(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_ELEMENT);
    if (!hCombo)
        return;

    g_currentElement = (int)GetComboBoxSelectedData(hCombo);
    UpdateElementControls();
}

static
void ChangeFontName(void)
{
    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    GetDlgItemText(g_hDlg, IDC_CLASSIC_FONTNAME,
        plfFont->lfFaceName, LF_FACESIZE);

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_SETFONT,
        (WPARAM)iFont, (LPARAM)plfFont);

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

/* For the given font, updates the minimum height and, if the current value was
 * changed (to the new minimum), updates the width proportionally
 */
static
void UpdateSizesFromFont(_In_ const LOGFONT *plfFont,
    int iSizeH, _Inout_ int *height,
    int iSizeW, _Inout_ int *width)
{
    int previousHeight = *height;

    UpdateMinHeightFromFont(plfFont, iSizeH);

    /* Do not go below the current value */
    if (*height > g_sizeRanges[iSizeH].current)
        *height = g_sizeRanges[iSizeH].current;

    /* But do not go below the minimum */
    if (*height < g_sizeRanges[iSizeH].min)
        *height = g_sizeRanges[iSizeH].min;

    *width = MulDiv(*width, *height, previousHeight);
    if (*width > g_sizeRanges[iSizeW].max)
        *width = g_sizeRanges[iSizeW].max;
}

static
void ChangeFontSize(void)
{
    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSIZE);
    if (!hCombo)
        return;

#if defined(WITH_EDITABLE_FONTSIZE)
    TCHAR szSize[4];

    GetWindowText(hCombo, szSize, 4);
    unsigned int size = (int)ttoui(szSize, NULL);
#else
    unsigned int size = (int)GetComboBoxSelectedData(hCombo);
#endif

    plfFont->lfHeight = FontPtToLog(size);

    BOOL sizeChanged = TRUE;

    /* Change the item size according to the font height */
    switch (iFont)
    {
    case FONT_CAPTION:
        UpdateSizesFromFont(plfFont,
            SIZE_CAPTIONHEIGHT, &g_schemes->scheme.ncMetrics.iCaptionHeight,
            SIZE_CAPTIONWIDTH,  &g_schemes->scheme.ncMetrics.iCaptionWidth);
        break;

    case FONT_SMCAPTION:
        UpdateSizesFromFont(plfFont,
            SIZE_SMCAPTIONHEIGHT, &g_schemes->scheme.ncMetrics.iSmCaptionHeight,
            SIZE_SMCAPTIONWIDTH,  &g_schemes->scheme.ncMetrics.iSmCaptionWidth);
        break;

    case FONT_MENU:
        UpdateSizesFromFont(plfFont,
            SIZE_MENUHEIGHT, &g_schemes->scheme.ncMetrics.iMenuHeight,
            SIZE_MENUWIDTH,  &g_schemes->scheme.ncMetrics.iMenuWidth);
        break;

    default:
        sizeChanged = FALSE;
    }

    if (sizeChanged)
    {
        g_bRefreshingSizes = TRUE;
        UpdateSize1Control();
        UpdateSize2Control();
        g_bRefreshingSizes = FALSE;
    }

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW,
        PVM_UPDATESCHEME, 0L, (LPARAM)&g_schemes->scheme);

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static
void ChangeFontSmoothing(void)
{
    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_FONTSMOOTHING);
    if (!hCombo)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    plfFont->lfQuality = (BYTE)GetComboBoxSelectedData(hCombo);

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_SETFONT,
        (WPARAM)iFont, (LPARAM)plfFont);

    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}
#endif

static
void ToggleFontBold(void)
{
    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    int value = (int)SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTBOLD, BM_GETCHECK, 0L, 0L);

    plfFont->lfWeight = (value == BST_CHECKED) ? FW_BOLD : FW_NORMAL;
    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_SETFONT,
        (WPARAM)iFont, (LPARAM)plfFont);

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void ToggleFontItalic(void)
{
    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    int value = (int)SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTITALIC, BM_GETCHECK, 0L, 0L);

    plfFont->lfItalic = (value == BST_CHECKED);

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_SETFONT,
        (WPARAM)iFont, (LPARAM)plfFont);

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
static
void ToggleFontUnderline(void)
{
    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    int value = (int)SendDlgItemMessage(
        g_hDlg, IDC_CLASSIC_FONTUNDERLINE, BM_GETCHECK, 0L, 0L);

    plfFont->lfUnderline = (value == BST_CHECKED);

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_SETFONT,
        (WPARAM)iFont, (LPARAM)plfFont);

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}
#endif

static
void DeleteSelectedScheme(void)
{
    if (!g_schemes->selected.color || !g_schemes->selected.size)
        return;

    HWND hSchemeCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEME);
    if (!hSchemeCombo)
        return;

    HWND hSizeCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEMESIZE);
    if (!hSizeCombo)
        return;

    int iScheme = (int)SendMessage(hSchemeCombo, CB_GETCURSEL, 0L, 0L);
    if (iScheme < 0)
        return;

    int sizeCount = (int)SendMessage(hSizeCombo, CB_GETCOUNT, 0L, 0L);

    int iString;
    if (sizeCount > 1)
        iString = IDS_DELETESCHEME_CONFIRM_SIZE;
    else
        iString = IDS_DELETESCHEME_CONFIRM_SCHEME;
    if (ShowMessageFromResource(
        g_hDlg, iString, IDS_DELETE_CONFIRM_TITLE, MB_YESNO | MB_ICONWARNING)
        != IDYES)
    {
        return;
    }

    if (sizeCount > 1)
    {
        if (!DeleteSize(&g_schemes->selected.color, &g_schemes->selected.size))
        {
            ShowMessageFromResource(g_hDlg,
                IDS_DELETESCHEME_ERROR_SIZE, IDS_ERROR, MB_OK | MB_ICONERROR);
            return;
        }

        /* Delete size item */
        int iSize = (int)SendMessage(hSizeCombo, CB_GETCURSEL, 0L, 0L);

        sizeCount = (int)SendMessage(
            hSizeCombo, CB_DELETESTRING, (WPARAM)iSize, 0L);

#if 0
        /* Select the following item, or the last one */
        SendMessage(
            hSizeCombo, CB_SETCURSEL, (WPARAM)min(iSize, sizeCount - 1), 0L);
#else
        EnableDeleteButtons(FALSE);
#endif

        ChangeSchemeSize();
    }
    else  /* sizeCount <= 1 */
    {
        if (!DeleteScheme(&g_schemes->selected.color))
        {
            ShowMessageFromResource(g_hDlg,
                IDS_DELETESCHEME_ERROR_SCHEME, IDS_ERROR, MB_OK | MB_ICONERROR);
            return;
        }

        /* The last size was removed; remove scheme item */
#if 0
        int schemeCount = (int)
#endif
        SendMessage(
            hSchemeCombo, CB_DELETESTRING, (WPARAM)iScheme, 0L);

        EnableDeleteButtons(FALSE);
        SendMessage(hSchemeCombo, CB_SETCURSEL, 0L, 0L);
        SendMessage(hSizeCombo, CB_RESETCONTENT, 0L, 0L);

#if 0
        if (schemeCount > 0)
        {
            SendMessage(
                hSchemeCombo, CB_SETCURSEL, (WPARAM)min(iScheme, schemeCount - 1), 0L);
            ChangeScheme();
        }
        else
        {
            /* No schemes left */
            SendMessage(hSchemeCombo, CB_RESETCONTENT, 0L, 0L);
            SendMessage(hSizeCombo, CB_RESETCONTENT, 0L, 0L);

            SetFocus(hSchemeCombo);
        }
#endif
    }
}

static
int AddSchemeToCombo(HWND hCombo, _In_ const SCHEME *pScheme)
{
    int listIndex = (int)SendMessage(
        hCombo, CB_ADDSTRING, 0L, (LPARAM)pScheme->name);

    if (listIndex >= 0)
        SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)listIndex, (LPARAM)pScheme);

    return listIndex;
}

static
void SelectNewScheme(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEME);
    if (!hCombo)
        return;

    if (SelectComboByItemData(hCombo, (LRESULT)g_schemes->selected.color))
    {
        LoadSizeCombo();
    }
    else
    {
        int index = AddSchemeToCombo(hCombo, g_schemes->selected.color);
        SendMessage(hCombo, CB_SETCURSEL, (WPARAM)index, 0L);
    }

    hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_SCHEMESIZE);
    if (!hCombo)
        return;

    if (!SelectComboByItemData(hCombo, (LRESULT)g_schemes->selected.size))
    {
        int index = AddSchemeToCombo(hCombo, g_schemes->selected.size);
        SendMessage(hCombo, CB_SETCURSEL, (WPARAM)index, 0L);
        EnableDeleteButtons(TRUE);
    }
}

_Success_(return)
static
BOOL RefreshSelectedSchemeName(int iControl)
{
    HWND hCombo = GetDlgItem(g_hDlg, iControl);
    if (!hCombo)
        return FALSE;

    int listIndex = (int)SendMessage(hCombo, CB_GETCURSEL, 0L, 0L);
    if (listIndex < 0)
        return FALSE;

    SCHEME *pScheme = (SCHEME *)SendMessage(
        hCombo, CB_GETITEMDATA, (WPARAM)listIndex, 0L);
    if (!pScheme)
        return FALSE;

    if (SendMessage(hCombo, CB_DELETESTRING, (WPARAM)listIndex, 0L) == CB_ERR)
        return FALSE;

    listIndex = (int)SendMessage(
        hCombo, CB_ADDSTRING, 0L, (LPARAM)pScheme->name);
    if (listIndex < 0)
        return FALSE;

    SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)listIndex, (LPARAM)pScheme);
    SendMessage(hCombo, CB_SETCURSEL, (WPARAM)listIndex, 0L);

    return TRUE;
}

static
void HandleButtonPress(WORD iControl)
{
    switch (iControl)
    {
    case IDC_CLASSIC_SAVE:
        if (DialogBoxParam(
            g_propSheet.hInstance, MAKEINTRESOURCE(IDD_CLASSIC_SAVE),
            g_hDlg, SaveSchemeDlgProc, 0L))
        {
            SelectNewScheme();
        }
        break;

    case IDC_CLASSIC_RENAME:
        if (DialogBoxParam(
            g_propSheet.hInstance, MAKEINTRESOURCE(IDD_CLASSIC_RENAME),
            g_hDlg, RenameSchemeDlgProc, 0L))
        {
            RefreshSelectedSchemeName(IDC_CLASSIC_SCHEME);
            RefreshSelectedSchemeName(IDC_CLASSIC_SCHEMESIZE);
        }
        break;

    case IDC_CLASSIC_DELETE:
        DeleteSelectedScheme();
        break;

    case IDC_CLASSIC_FONTBOLD:
        ToggleFontBold();
        break;

    case IDC_CLASSIC_FONTITALIC:
        ToggleFontItalic();
        break;

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    case IDC_CLASSIC_FONTUNDERLINE:
        ToggleFontUnderline();
        break;
#endif

    case IDC_CLASSIC_COLOR1:
        HandleColorButtonPress(COLORBUTTON_1);
        break;

    case IDC_CLASSIC_COLOR2:
        HandleColorButtonPress(COLORBUTTON_2);
        break;

    case IDC_CLASSIC_FONTCOLOR:
        HandleColorButtonPress(COLORBUTTON_FONT);
        break;

#if WINVER >= WINVER_2K && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    case IDC_CLASSIC_GRADIENTCAPTIONS:
        g_schemes->scheme.bGradientCaptions = (SendDlgItemMessage(
            g_hDlg, iControl, BM_GETCHECK, 0L, 0L) == BST_CHECKED);

        SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
            PV_UPDATE_ONLY_COLORS, (LPARAM)&g_schemes->scheme);

        UnselectScheme();
        PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
        break;
#endif

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    case IDC_CLASSIC_FLATMENUS:
        g_schemes->scheme.bFlatMenus = (SendDlgItemMessage(
            g_hDlg, iControl, BM_GETCHECK, 0L, 0L) == BST_CHECKED);

        SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
            PV_UPDATE_ONLY_COLORS, (LPARAM)&g_schemes->scheme);

        UnselectScheme();
        PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
        break;
#endif
    }
}

static
void HandleComboBoxSelChange(WORD iControl)
{
    switch (iControl)
    {
    case IDC_CLASSIC_SCHEME:
        ChangeScheme();
        break;

    case IDC_CLASSIC_SCHEMESIZE:
        ChangeSchemeSize();
        break;

    case IDC_CLASSIC_ELEMENT:
        ChangeElement();
        break;

    case IDC_CLASSIC_FONTNAME:
        ChangeFontName();
        break;

    case IDC_CLASSIC_FONTSIZE:
        ChangeFontSize();
        break;

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    case IDC_CLASSIC_FONTSMOOTHING:
        ChangeFontSmoothing();
        break;
#endif
    }
}

#if defined(WITH_EDITABLE_FONTSIZE)
static
void HandleComboBoxEdit(WORD iControl)
{
    switch (iControl)
    {
    case IDC_CLASSIC_FONTSIZE:
        ChangeFontSize();
        break;
    }
}
#endif

static
void HandleSizeCommand(WORD command, int metric, int textControl, int udControl)
{
    switch (command)
    {
    case EN_CHANGE:
        if (metric >= 0 && !g_bRefreshingSizes)
        {
            int value = (int)LOWORD(SendDlgItemMessage(
                g_hDlg, udControl, UDM_GETPOS, 0L, 0L));
            UpdateSize(value, metric, textControl);
        }
        break;

    case EN_KILLFOCUS:
        SetUpDownOnRange(GetDlgItem(g_hDlg, udControl));
        break;
    }
}

static
void HandleFontUdChange(int udControl)
{
    if (g_bRefreshingSizes)
        return;

    int iFont = g_assignment[g_currentElement].font;
    if (iFont < 0)
        return;

    LOGFONT *plfFont = GetSchemeFont(&g_schemes->scheme, iFont);
    if (!plfFont)
        return;

    WORD value = LOWORD(SendDlgItemMessage(
        g_hDlg, udControl, UDM_GETPOS, 0L, 0L));

    switch (udControl)
    {
    case IDC_CLASSIC_FONTWIDTH_UD:
        plfFont->lfWidth = FontPtToLog(value);
        break;

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    case IDC_CLASSIC_FONTANGLE_UD:
        plfFont->lfEscapement = value * 10;  /* tenth of degrees */
        plfFont->lfOrientation = plfFont->lfEscapement;
        break;
#endif

    default:
        return;
    }

    SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_SETFONT,
        (WPARAM)iFont, (LPARAM)plfFont);

    UnselectSchemeSize();
    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void HandleFontUdCommand(WORD command, int udControl)
{
    switch (command)
    {
    case EN_CHANGE:
        HandleFontUdChange(udControl);
        break;

    case EN_KILLFOCUS:
        SetUpDownOnRange(GetDlgItem(g_hDlg, udControl));
        break;
    }
}

/* Returns if an item was selected */
BOOL AddSchemesToCombo(
    HWND hCombo, _In_ const SCHEME *pSchemes, _In_opt_ const SCHEME *pSelected)
{
    BOOL ret = FALSE;

    for (const SCHEME *pCurrent = pSchemes; pCurrent; pCurrent = pCurrent->next)
    {
        int listIndex = AddSchemeToCombo(hCombo, pCurrent);
        if (listIndex >= 0 && pCurrent == pSelected)
        {
            SendMessage(hCombo, CB_SETCURSEL, (WPARAM)listIndex, 0L);
            ret = TRUE;
        }
    }

    return ret;
}

INT_PTR CALLBACK ClassicStylePageProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hDlg = hWnd;
        InitPage();
        return 0;

    case WM_DESTROY:
        DestroyPage();
        return 0;

    case WM_COMMAND:
        if (!g_schemes || g_bInitializing)
            return 0;

        switch (LOWORD(wParam))
        {
        case IDC_CLASSIC_PREVIEW:
            if (SelectComboByItemData(
                GetDlgItem(hWnd, IDC_CLASSIC_ELEMENT), (LRESULT)lParam))
            {
                g_currentElement = (int)lParam;
                UpdateElementControls();
            }
            break;

        case IDC_CLASSIC_SIZE1_E:
            HandleSizeCommand(
                HIWORD(wParam), g_assignment[g_currentElement].size1,
                IDC_CLASSIC_SIZE1_E, IDC_CLASSIC_SIZE1_UD);
            break;

        case IDC_CLASSIC_SIZE2_E:
            HandleSizeCommand(
                HIWORD(wParam), g_assignment[g_currentElement].size2,
                IDC_CLASSIC_SIZE2_E, IDC_CLASSIC_SIZE2_UD);
            break;

        case IDC_CLASSIC_FONTWIDTH_E:
            HandleFontUdCommand(HIWORD(wParam), IDC_CLASSIC_FONTWIDTH_UD);
            break;

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
        case IDC_CLASSIC_FONTANGLE_E:
            HandleFontUdCommand(HIWORD(wParam), IDC_CLASSIC_FONTANGLE_UD);
            break;
#endif
        }

        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            HandleButtonPress(LOWORD(wParam));
            break;

        case CBN_SELCHANGE:
            HandleComboBoxSelChange(LOWORD(wParam));
            break;

#if defined(WITH_EDITABLE_FONTSIZE)
        case CBN_EDITCHANGE:
            HandleComboBoxEdit(LOWORD(wParam));
            break;
#endif

        }

        return 0;

    case WM_NOTIFY:
        switch (((NMHDR *)lParam)->code)
        {
        case PSN_APPLY:
        {
            /* The selection is kept for the default saved names, but if the
             * scheme was modified the name should be removed from the registry
             */
            SCHEME_SELECTION selected;

            if (SendDlgItemMessage(g_hDlg, IDC_CLASSIC_SCHEME,
                CB_GETCURSEL, 0L, 0L) >= 0)
            {
                selected.color = g_schemes->selected.color;
            }
            else
                selected.color = NULL;

            if (SendDlgItemMessage(g_hDlg, IDC_CLASSIC_SCHEMESIZE,
                CB_GETCURSEL, 0L, 0L) >= 0)
            {
                selected.size = g_schemes->selected.size;
            }
            else
                selected.size = NULL;

            ApplyScheme(&g_schemes->scheme, &selected);
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG_PTR)PSNRET_NOERROR);
            return TRUE;
        }

        case PSN_KILLACTIVE:
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG_PTR)FALSE);
            return TRUE;
        }

        return 0;

    case WM_SYSCOLORCHANGE:
        UpdateColorButtonsBorder();

        /* Refresh colors to detect if they are different from the selected
         * scheme.
         */
        RefreshCurrentColors();

        return 0;

    case WM_SETTINGCHANGE:
        switch (wParam)
        {
        case SPI_SETNONCLIENTMETRICS:
            RefreshCurrentNcMetrics();
            break;

        case SPI_SETICONTITLELOGFONT:
            RefreshCurrentIconFont();
            break;

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_2K
        case SPI_SETGRADIENTCAPTIONS:
            RefreshCurrentGradientCaptionsSetting();
            break;
#endif

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_XP
        case SPI_SETFLATMENU:
            RefreshCurrentFlatMenusSetting();
            break;
#endif

#if WINVER >= WINVER_2K
        case SPI_SETKEYBOARDCUES:
            SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
                PV_UPDATE_ONLY_COLORS, (LPARAM)&g_schemes->scheme);
            break;
#endif

#if 0  /* defined(WITH_NEW_PREVIEW) */
        case SPI_SETDESKPATTERN:  /* Never received */
            if (g_schemes->scheme.desktopPattern)
                Free(g_schemes->scheme.desktopPattern);
            g_schemes->scheme.desktopPattern = LoadCurrentDesktopPattern();
            SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
                PV_UPDATE_ONLY_COLORS, (LPARAM)&g_schemes->scheme);
#endif

#if WINVER >= WINVER_XP
        case 0:
            SendDlgItemMessage(g_hDlg, IDC_CLASSIC_PREVIEW, PVM_UPDATESCHEME,
                PV_UPDATE_EXPLORER_SETTINGS, (LPARAM)&g_schemes->scheme);
            break;
#endif
        }

        return 0;

    case WM_QUERYENDSESSION:
        /* Contrary to what the documentation says, without this shutdown is
         * prevented, at least from W95 to 7...
         */
        return 0;

    case WM_CTLCOLORDLG:
        /* Returning any other value paints the property sheet pages with an
         * incorrect background color.
         */
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
