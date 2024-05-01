/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Handling of classic visual schemes
 *
 * PROGRAMMERS: Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 *              Ismael Ferreras Morezuelas (swyterzone+reactos@gmail.com)
 *              Timo Kreuzer (timo[dot]kreuzer[at]web[dot]de)
 *              Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "classtyl.h"
#include "reg.h"
#include "resource.h"
#include "mincrt.h"
#include "util.h"

#define XP_SCHEMENAME_MAX_SIZE 5

/* Registry name constants */

static const TCHAR szAppearanceKey[] =
    TEXT("Control Panel\\Appearance");

const TCHAR szSchemesKey[] =
    TEXT("Control Panel\\Appearance\\ClassicSchemes");

const TCHAR szColorsKey[] =
    TEXT("Control Panel\\Colors");

#if defined(WITH_MIGRATION)
static const TCHAR szLegacySchemesKey[] =
    TEXT("Control Panel\\Appearance\\Schemes");
#endif

/* For XP format */
#if defined(WITH_MIGRATION) && WINVER >= WINVER_XP && WINVER <= WINVER_7
static const TCHAR szXpSchemesKey[] =
    TEXT("Control Panel\\Appearance\\New Schemes");

static const TCHAR szDisplayNameValue[] = TEXT("DisplayName");
#endif

static const TCHAR szLegacySelectedSchemeValue[] = TEXT("Current");

static const TCHAR szXpSelectedSchemeValue[] = TEXT("SelectedStyle");
static const TCHAR szSelectedSchemeValue[]   = TEXT("SelectedScheme");
static const TCHAR szSelectedSizeValue[]     = TEXT("SelectedSize");

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#if WINVER >= WINVER_2K
static const TCHAR szGradientsValue[] = TEXT("Gradients");
#endif
#endif

#if WINVER >= WINVER_XP
const TCHAR szFlatMenusValue[] = TEXT("FlatMenus");
#endif

static SCHEME_DATA g_currentScheme;

SCHEMES_DATA *g_schemes = NULL;

/* Item names stored in the registry and theme files */

const TCHAR *g_colorNames[NUM_COLORS] = {
    TEXT("Scrollbar"),              /* 00 = COLOR_SCROLLBAR */
    TEXT("Background"),             /* 01 = COLOR_BACKGROUND */
                                    /*      COLOR_DESKTOP */
    TEXT("ActiveTitle"),            /* 02 = COLOR_ACTIVECAPTION */
    TEXT("InactiveTitle"),          /* 03 = COLOR_INACTIVECAPTION */
    TEXT("Menu"),                   /* 04 = COLOR_MENU */
    TEXT("Window"),                 /* 05 = COLOR_WINDOW */
    TEXT("WindowFrame"),            /* 06 = COLOR_WINDOWFRAME */
    TEXT("MenuText"),               /* 07 = COLOR_MENUTEXT */
    TEXT("WindowText"),             /* 08 = COLOR_WINDOWTEXT */
    TEXT("TitleText"),              /* 09 = COLOR_CAPTIONTEXT */
    TEXT("ActiveBorder"),           /* 10 = COLOR_ACTIVEBORDER */
    TEXT("InactiveBorder"),         /* 11 = COLOR_INACTIVEBORDER */
    TEXT("AppWorkSpace"),           /* 12 = COLOR_APPWORKSPACE */
    TEXT("Hilight"),                /* 13 = COLOR_HIGHLIGHT */
    TEXT("HilightText"),            /* 14 = COLOR_HIGHLIGHTTEXT */
    TEXT("ButtonFace"),             /* 15 = COLOR_BTNFACE */
                                    /*      COLOR_3DFACE */
    TEXT("ButtonShadow"),           /* 16 = COLOR_BTNSHADOW */
                                    /*      COLOR_3DSHADOW */
    TEXT("GrayText"),               /* 17 = COLOR_GRAYTEXT */
                                    /*      (Disabled menu item selection) */
    TEXT("ButtonText"),             /* 18 = COLOR_BTNTEXT */
    TEXT("InactiveTitleText"),      /* 19 = COLOR_INACTIVECAPTIONTEXT */
    TEXT("ButtonHilight"),          /* 20 = COLOR_BTNHIGHLIGHT */
                                    /*      COLOR_BTNHILIGHT */
                                    /*      COLOR_3DHIGHLIGHT */
                                    /*      COLOR_3DHILIGHT */
    TEXT("ButtonDkShadow"),         /* 21 = COLOR_3DDKSHADOW */
    TEXT("ButtonLight"),            /* 22 = COLOR_3DLIGHT */
    TEXT("InfoText"),               /* 23 = COLOR_INFOTEXT */
    TEXT("InfoWindow"),             /* 24 = COLOR_INFOBK */
#if WINVER >= WINVER_2K
    TEXT("ButtonAlternateFace"),    /* 25 = COLOR_ALTERNATEBTNFACE */
                                    /*      (unused, undefined by the SDK) */
    TEXT("HotTrackingColor"),       /* 26 = COLOR_HOTLIGHT (Hyperlink) */
    TEXT("GradientActiveTitle"),    /* 27 = COLOR_GRADIENTACTIVECAPTION */
    TEXT("GradientInactiveTitle"),  /* 28 = COLOR_GRADIENTINACTIVECAPTION */
#endif
#if WINVER >= WINVER_XP
    TEXT("MenuHilight"),            /* 29 = COLOR_MENUHILIGHT */
    TEXT("MenuBar")                 /* 30 = COLOR_MENUBAR */
#endif
};

const TCHAR *g_sizeNames[NUM_SIZES] = {
    TEXT("BorderWidth"),            /* 0 = SIZE_BORDER */
    TEXT("ScrollWidth"),            /* 1 = SIZE_SCROLLWIDTH */
    TEXT("ScrollHeight"),           /* 2 = SIZE_SCROLLHEIGHT */
    TEXT("CaptionWidth"),           /* 3 = SIZE_CAPTIONWIDTH */
    TEXT("CaptionHeight"),          /* 4 = SIZE_CAPTIONHEIGHT */
    TEXT("SmCaptionWidth"),         /* 5 = SIZE_SMCAPTIONWIDTH */
    TEXT("SmCaptionHeight"),        /* 6 = SIZE_SMCAPTIONHEIGHT */
    TEXT("MenuWidth"),              /* 7 = SIZE_MENUWIDTH */
    TEXT("MenuHeight"),             /* 8 = SIZE_MENUHEIGHT */
#if WINVER >= WINVER_VISTA
    TEXT("PaddedBorderWidth")       /* 9 = SIZE_PADDEDBORDER */
#endif
};

const TCHAR *g_fontNames[NUM_FONTS] = {
    TEXT("CaptionFont"),            /* 0 = FONT_CAPTION */
    TEXT("SmCaptionFont"),          /* 1 = FONT_SMCAPTION */
    TEXT("MenuFont"),               /* 2 = FONT_MENU */
    TEXT("StatusFont"),             /* 3 = FONT_TOOLTIP */
    TEXT("MessageFont"),            /* 4 = FONT_MESSAGE */
    TEXT("IconFont")                /* 5 = FONT_DESKTOP */
};

SIZE_RANGE g_sizeRanges[NUM_SIZES] =
{
    /* Index                     Min  Max  Current Orig.Max */
    /* SIZE_BORDER          */  {  1,  15,  1 },   /*  50 */
    /* SIZE_SCROLLWIDTH     */  {  8,  70, 16 },   /* 100 */
    /* SIZE_SCROLLHEIGHT    */  {  8,  70, 16 },   /* 100 */
    /* SIZE_CAPTIONWIDTH    */  { 12,  70, 18 },   /* 100 */
    /* SIZE_CAPTIONHEIGHT   */  { 12,  70, 18 },   /* 100 */
    /* SIZE_SMCAPTIONWIDTH  */  { 12,  70, 12 },   /* 100 */
    /* SIZE_SMCAPTIONHEIGHT */  { 12,  70, 15 },   /* 100 */
    /* SIZE_MENUWIDTH       */  { 12,  70, 18 },   /* 100 */
    /* SIZE_MENUHEIGHT      */  { 12,  70, 18 },   /* 100 */
#if WINVER >= WINVER_VISTA
    /* SIZE_PADDEDBORDER    */  {  0,  15,  0 }    /* 100 */
#endif
};

#if 0
/* Inserts a scheme in a list while keeping the list sorted.
 * In theory this should be needed because enumerating registry subkeys does
 * not guarantee them to be sorted, but in practice they are sorted.
 */
static
void InsertScheme(_Inout_ SCHEME **ppList, _Inout_ SCHEME *pScheme)
{
    if (!*ppList)
    {
        *ppList = pScheme;
        return;
    }

    SCHEME *pPrevious = *ppList;

    for (SCHEME *pCurrent = *ppList;
        pCurrent;
        pCurrent = pCurrent->next)
    {
        if (lstrcmpi(pCurrent->name, pScheme->name) <= 0)
        {
            pPrevious = pCurrent;
            continue;
        }

        /* Insert */
        pScheme->next = pCurrent;
        if (pCurrent == *ppList)
            *ppList = pScheme;
        else
            pPrevious->next = pScheme;

        return;
    }

    /* Last element reached */
    pPrevious->next = pScheme;
}
#endif

/* Returns a list of sizes from the registry key of a scheme */
_Ret_maybenull_
static
SCHEME *LoadSchemeSizes(HKEY hkScheme)
{
    HKEY hkSize;
    TCHAR sizeName[SIZENAME_MAX_SIZE];
    DWORD cbSizeName;
    SCHEME *pList = NULL;
    SCHEME *pCurrent = NULL;
    SCHEME *pPrevious;

    DWORD iScheme = 0;
    LSTATUS status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        cbSizeName = SIZENAME_MAX_SIZE;
        status = RegEnumKeyEx(
            hkScheme, iScheme, sizeName, &cbSizeName, NULL, NULL, NULL, NULL);
        iScheme++;
        if (status != ERROR_SUCCESS)
            continue;

        status = RegOpenKeyEx(hkScheme, sizeName, 0, READ_CONTROL, &hkSize);
        if (status != ERROR_SUCCESS)
            continue;

        pPrevious = pCurrent;
        pCurrent = CreateScheme(sizeName);
        if (pCurrent)
        {
            if (pList)
                pPrevious->next = pCurrent;
            else
                pList = pCurrent;
        }

        RegCloseKey(hkSize);
    }

    return pList;
}

_Ret_maybenull_
static
SCHEME *GetSchemesFromRegistry(void)
{
    HKEY hkSchemes;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSchemesKey, 0,
        KEY_ENUMERATE_SUB_KEYS, &hkSchemes);
    if (status != ERROR_SUCCESS)
        return NULL;

    HKEY hkScheme;
    TCHAR schemeName[SCHEMENAME_MAX_SIZE];
    DWORD cbSchemeName;
    SCHEME *pList = NULL;
    SCHEME *pCurrent = NULL;
    SCHEME *pPrevious;

    DWORD iScheme = 0;
    status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        cbSchemeName = SCHEMENAME_MAX_SIZE;
        status = RegEnumKeyEx(hkSchemes, iScheme, schemeName, &cbSchemeName,
            NULL, NULL, NULL, NULL);
        iScheme++;
        if (status != ERROR_SUCCESS)
            continue;

        status = RegOpenKeyEx(hkSchemes, schemeName, 0,
            KEY_ENUMERATE_SUB_KEYS, &hkScheme);
        if (status != ERROR_SUCCESS)
            continue;

        pPrevious = pCurrent;
        pCurrent = CreateScheme(schemeName);
        if (pCurrent)
        {
            pCurrent->sizes = LoadSchemeSizes(hkScheme);
            if (pCurrent->sizes)
            {
                if (pList)
                    pPrevious->next = pCurrent;
                else
                    pList = pCurrent;
            }
            else
                FreeSchemes(&pCurrent);
        }

        RegCloseKey(hkScheme);
    }

    RegCloseKey(hkSchemes);
    return pList;
}

_Success_(return)
static
BOOL DeleteKey(_In_ const TCHAR *szKey, _In_ const TCHAR *szSubKey)
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szKey, 0, DELETE, &hKey) !=
        ERROR_SUCCESS)
    {
        return FALSE;
    }

    /* Delete the key and all its subkeys (just in case there is junk) */
    BOOL ret = (SHDeleteKey(hKey, szSubKey) == ERROR_SUCCESS);

    RegCloseKey(hKey);
    return ret;
}

/* Note: Specify an empty sizeName to delete the whole scheme */
_Success_(return)
static
BOOL DeleteSchemeRegistry(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName)
{
    TCHAR szSchemeKey[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];
    if (wsprintf(szSchemeKey, TEXT("%s\\%s"), szSchemesKey, schemeName) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return FALSE;
    }

    return DeleteKey(szSchemeKey, sizeName);
}

/* When the scheme changes, remove the selection from system applets to avoid
 * confusing the user */
static
void DeleteSystemSelectedScheme(void)
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szAppearanceKey, 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return;
    }

    RegDeleteValue(hKey, szLegacySelectedSchemeValue);
    RegCloseKey(hKey);
}

static
void SaveSelectedSchemeName(_In_ const TCHAR *name)
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szSchemesKey, 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return;
    }

    RegSetValueEx(hKey, szSelectedSchemeValue, 0, REG_SZ, (BYTE *)name,
        (lstrlen(name) + 1) * sizeof(TCHAR));

    RegCloseKey(hKey);
}

static
void DeleteSelectedSchemeName(void)
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szSchemesKey, 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return;
    }

    RegDeleteValue(hKey, szSelectedSchemeValue);
    RegCloseKey(hKey);
}

static
void SaveSelectedSizeName(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName)
{
    TCHAR szSchemeKey[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];
    if (wsprintf(szSchemeKey, TEXT("%s\\%s"), szSchemesKey, schemeName) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return;
    }

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szSchemeKey, 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return;
    }

    RegSetValueEx(hKey, szSelectedSizeValue, 0, REG_SZ, (BYTE *)sizeName,
        (lstrlen(sizeName) + 1) * sizeof(TCHAR));
    RegCloseKey(hKey);
}

static
void DeleteSelectedSizeName(_In_ const TCHAR *schemeName)
{
    TCHAR szSchemeKey[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];
    if (wsprintf(szSchemeKey, TEXT("%s\\%s"), szSchemesKey, schemeName) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return;
    }

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szSchemeKey, 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return;
    }

    RegDeleteValue(hKey, szSelectedSizeValue);
    RegCloseKey(hKey);
}

/* Like GetCurrentScheme(), but for g_currentScheme */
_Ret_maybenull_
static
LOGFONT *GetCurrentSchemeFont(int metric)
{
    switch (metric)
    {
    case FONT_CAPTION:
        return &g_currentScheme.ncMetrics.lfCaptionFont;

    case FONT_SMCAPTION:
        return &g_currentScheme.ncMetrics.lfSmCaptionFont;

    case FONT_MENU:
        return &g_currentScheme.ncMetrics.lfMenuFont;

    case FONT_TOOLTIP:
        return &g_currentScheme.ncMetrics.lfStatusFont;

    case FONT_MESSAGE:
        return &g_currentScheme.ncMetrics.lfMessageFont;

    case FONT_DESKTOP:
        return &g_currentScheme.lfIconFont;
    }

    return NULL;
}

_Success_(return != CLR_INVALID)
static
COLORREF ReadSchemeColor(HKEY hKey, _In_ const TCHAR *valueName)
{
    COLORREF color;
    LSTATUS status = RegQueryDWord(hKey, valueName, &color);
    if (status != ERROR_SUCCESS)
        return CLR_INVALID;

    return color & 0x00FFFFFF;  /* Strip palette info from system schemes */
}

_Success_(return >= 0)
static
int ReadSchemeSize(HKEY hKey, int iSize)
{
    TCHAR valueName[6];
    if (wsprintf(valueName, TEXT("Size%d"), iSize) < 5)
        return -1;

    DWORD size;
    LSTATUS status = RegQueryDWord(hKey, valueName, &size);
    if (status != ERROR_SUCCESS)
        return -1;

    if (g_propSheet.dpi != USER_DEFAULT_SCREEN_DPI)
        return ScaleForDpi((int)size);

    return (int)size;
}

_Success_(return)
static
BOOL CopyDefaultFont(LOGFONT *pDstLogFont, int iFont)
{
    LOGFONT *pCurrentLogFont = GetCurrentSchemeFont(iFont);
    if (!pCurrentLogFont)
        return FALSE;

    memcpy(pDstLogFont, pCurrentLogFont, sizeof(LOGFONT));
    return TRUE;
}

_Success_(return)
static
BOOL ReadSchemeFont(HKEY hKey, _In_ const TCHAR *valueName, int iFont,
    _Out_ SCHEME_DATA *pData)
{
#if defined(_MSC_VER)
    /* Ignore unitialized memory warning; the contents will be overwritten */
#pragma warning(push)
#pragma warning(disable: 6001)
#endif
    LOGFONT *pLogFont = GetSchemeFont(pData, iFont);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
    if (!pLogFont)
        return FALSE;

    DWORD dwType;
    DWORD cbData = sizeof(LOGFONTW);
#if defined(UNICODE)
    LSTATUS status = RegQueryValueEx(hKey, valueName, NULL, &dwType,
        (BYTE *)pLogFont, &cbData);
#else
    /* Always read as Unicode to allow sharing schemes between 9x and NT */
    LOGFONTW logFontW;
    LSTATUS status = RegQueryValueEx(hKey, valueName, NULL, &dwType,
        (BYTE *)&logFontW, &cbData);
#endif

    if (!(status == ERROR_SUCCESS && dwType == REG_BINARY &&
        cbData == sizeof(LOGFONTW)))
    {
        CopyDefaultFont(pLogFont, iFont);
        return FALSE;
    }

#if !defined(UNICODE)
    memcpy(pLogFont, &logFontW, sizeof(LOGFONT) - sizeof(pLogFont->lfFaceName));
    WideCharToMultiByte(CP_ACP, 0, logFontW.lfFaceName, -1,
        pLogFont->lfFaceName, LF_FACESIZE, NULL, NULL);
#endif

    if (g_propSheet.dpi != USER_DEFAULT_SCREEN_DPI)
    {
        pLogFont->lfHeight = ScaleForDpi(pLogFont->lfHeight);
        pLogFont->lfWidth = ScaleForDpi(pLogFont->lfWidth);
    }

    return TRUE;
}

#if defined(WITH_MIGRATION)
#if WINVER >= WINVER_2K
static
void SetGradientCaptionsFromColors(_Inout_ SCHEME_DATA *pData)
{
    pData->bGradientCaptions = (
        (pData->colors[COLOR_ACTIVECAPTION] !=
            pData->colors[COLOR_GRADIENTACTIVECAPTION]) ||
        (pData->colors[COLOR_INACTIVECAPTION] !=
            pData->colors[COLOR_GRADIENTINACTIVECAPTION]));
}
#endif

static
int GetFontHeight(_In_ const LOGFONT *plfFont)
{
    int height = -1;

    HDC hdc = GetDC(NULL);
    if (!hdc)
        return height;

    HFONT hFont = CreateFontIndirect(plfFont);
    if (!hFont)
        goto CleanDC;

    HGDIOBJ hPreviousFont = SelectObject(hdc, hFont);
    if (!hPreviousFont)
        goto CleanFont;

    TEXTMETRIC tm;
    if (!GetTextMetrics(hdc, &tm))
        goto CleanSelFont;

    height = tm.tmHeight;

CleanSelFont:
    SelectObject(hdc, hPreviousFont);
CleanFont:
    DeleteObject(hFont);
CleanDC:
    ReleaseDC(NULL, hdc);
    return height;
}

static
void UpdateMinHeightsFromFonts(_In_ SCHEME_DATA *pData)
{
    LOGFONT *plfFont;
    plfFont = GetSchemeFont(pData, FONT_CAPTION);
    if (plfFont)
        UpdateMinHeightFromFont(plfFont, SIZE_CAPTIONHEIGHT);

    plfFont = GetSchemeFont(pData, FONT_SMCAPTION);
    if (plfFont)
        UpdateMinHeightFromFont(plfFont, SIZE_SMCAPTIONHEIGHT);

    plfFont = GetSchemeFont(pData, FONT_MENU);
    if (plfFont)
        UpdateMinHeightFromFont(plfFont, SIZE_MENUHEIGHT);
}

#if WINVER >= WINVER_XP && WINVER <= WINVER_7
static
void LoadXpSchemeColors(HKEY hkScheme, _Out_ SCHEME_DATA *pData)
{
    TCHAR colorValueName[10];
    COLORREF color;
    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
    {
        if (wsprintf(colorValueName, TEXT("Color #%d"), iColor) < 8)
            color = CLR_INVALID;
        else
            color = ReadSchemeColor(hkScheme, colorValueName);

        if (color == CLR_INVALID)
            color = g_currentScheme.colors[iColor];

        pData->colors[iColor] = color;
    }

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    DWORD dwData = 0;

    LSTATUS status = RegQueryDWord(hkScheme, TEXT("Flat Menus"), &dwData);
    if (status == ERROR_SUCCESS)
        pData->bFlatMenus = (BOOL)dwData;
    else
        pData->bFlatMenus = FALSE;

    SetGradientCaptionsFromColors(pData);
#endif
}

_Success_(return >= 0)
static
int ReadXpSchemeSize(HKEY hKey, int iSize)
{
    TCHAR valueName[8];
    if (wsprintf(valueName, TEXT("Size #%d"), iSize) < 7)
        return -1;

    UINT64 size;
    LSTATUS status = RegQueryQWord(hKey, valueName, &size);

    if (status != ERROR_SUCCESS)
        return -1;

    if (g_propSheet.dpi != USER_DEFAULT_SCREEN_DPI)
        return ScaleForDpi((int)size);

    return (int)size;
}

_Success_(return)
static
BOOL LoadXpScheme(_Out_ SCHEME_DATA *pData,
    _In_ const TCHAR *internalSchemeName,
    _In_ const TCHAR *internalSizeName,
    BOOL bLoadColors)
{
    TCHAR szSchemeKey[_countof(szXpSchemesKey) +
        XP_SCHEMENAME_MAX_SIZE * 2 + 8];

    if (wsprintf(szSchemeKey, TEXT("%s\\%s\\Sizes\\%s"),
        szXpSchemesKey, internalSchemeName, internalSizeName) <
        (int)(_countof(szXpSchemesKey) + 9))
    {
        return FALSE;
    }

    HKEY hkScheme;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSchemeKey, 0,
        KEY_QUERY_VALUE, &hkScheme);
    if (status != ERROR_SUCCESS)
        return FALSE;

    if (bLoadColors)
        LoadXpSchemeColors(hkScheme, pData);

    TCHAR valueName[8];
    for (int iFont = 0; iFont < NUM_FONTS; iFont++)
    {
        if (wsprintf(valueName, TEXT("Font #%d"), iFont) >= 7)
            ReadSchemeFont(hkScheme, valueName, iFont, pData);
        else
        {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 6001)
#endif
            LOGFONT *pLogFont = GetSchemeFont(pData, iFont);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
            if (pLogFont)
                CopyDefaultFont(pLogFont, iFont);
        }
    }

    UpdateMinHeightsFromFonts(pData);

    int size;
    for (int iSize = 0; iSize < NUM_SIZES; iSize++)
    {
        size = ReadXpSchemeSize(hkScheme, iSize);

        if (size < 0)
        {
#if WINVER >= WINVER_VISTA
            if (iSize == SIZE_PADDEDBORDER)
                size = 0;
            else
#endif
                size = GetSchemeMetric(&g_currentScheme.ncMetrics, iSize);
        }

        SetSchemeMetric(&pData->ncMetrics, iSize, size);
    }

    RegCloseKey(hkScheme);
    return TRUE;
}

_Success_(return)
static
BOOL ImportXpSchemeSize(HKEY hkSizes,
    _In_ const TCHAR *schemeInternalName,
    _In_ const TCHAR *schemeDisplayName,
    _In_       TCHAR *sizeInternalName,
    _In_ const TCHAR *selectedName,
    UINT sizeCount)
{
    HKEY hkSize;
    LSTATUS status = RegOpenKeyEx(
        hkSizes, sizeInternalName, 0, KEY_QUERY_VALUE, &hkSize);
    if (status != ERROR_SUCCESS)
        return FALSE;

    SCHEME_DATA schemeData;

    /* On XP, all sizes also have colors; just copy the colors of the
     * first size
     */
    BOOL bLoadColors = (lstrcmp(sizeInternalName, TEXT("0")) == 0);
    if (!LoadXpScheme(&schemeData, schemeInternalName, sizeInternalName,
        bLoadColors))
    {
        RegCloseKey(hkSize);
        return FALSE;
    }

    if (bLoadColors)
        SaveSchemeColors(&schemeData, schemeDisplayName);

    TCHAR rawName[MAX_PATH];
#if defined(UNICODE)
    TCHAR sizeDisplayName[SIZENAME_MAX_SIZE];
#endif
    TCHAR fullExportedName[SIZENAME_MAX_SIZE];
    TCHAR *exportedName;

    DWORD cbData = sizeof(rawName);
    status = RegQueryString(hkSize, szDisplayNameValue, rawName, &cbData);
    if (status != ERROR_SUCCESS)
        exportedName = sizeInternalName;
    else
    {
#if defined(UNICODE)
        if (SHLoadIndirectString(rawName, sizeDisplayName,
            _countof(sizeDisplayName), NULL) == S_OK)
        {
            exportedName = sizeDisplayName;
        }
        else
#endif
        {
            /* Limit length (truncate if it is too long) */
            rawName[SIZENAME_MAX_SIZE - 1] = TEXT('\0');
            exportedName = rawName;
        }
    }

    /* When there are multiple sizes, prefix a number to display them
     * sorted.
     * On XP, size 0=Normal
     * If there are 2 sizes:
     *   1=Large
     * If there are 3 sizes:
     *   1=Extra Large (why??)
     *   2=Large
     */
    if (sizeCount > 1)
    {
        unsigned int n = ttoui(sizeInternalName, NULL);
        switch (n)
        {
        case 0:  /* 1-Normal */
            n = 1;
            break;
        case 1:  /* 2-Large / 3-Extra Large */
            n = (sizeCount <= 2 ? 2 : 3);
            break;
        case 2:  /* 2-Large */
            break;
        default:
            /* Not present on default installations, but just in case */
            n++;
        }

        /* Force length limit */
        if (lstrlen(exportedName) > SIZENAME_MAX_SIZE - 2)
            exportedName[SIZENAME_MAX_SIZE - 3] = TEXT('\0');

        wsprintf(fullExportedName, TEXT("%u-%s"), n, exportedName);
        exportedName = fullExportedName;
    }

    BOOL ret = SaveSchemeSize(&schemeData, schemeDisplayName, exportedName);
    if (ret && (lstrcmp(sizeInternalName, selectedName) == 0))
        SaveSelectedSizeName(schemeDisplayName, exportedName);

    RegCloseKey(hkSize);
    return ret;
}

/* Returns TRUE if at least one size was saved */
_Success_(return)
static
BOOL ImportXpSchemeSizes(HKEY hkScheme,
    _In_ const TCHAR *schemeInternalName,
    _In_ const TCHAR *schemeDisplayName)
{
    HKEY hkSizes;
    LSTATUS status = RegOpenKeyEx(hkScheme, TEXT("Sizes"), 0,
        KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkSizes);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR selectedName[XP_SCHEMENAME_MAX_SIZE];
    DWORD cbData = sizeof(selectedName);
    status = RegQueryString(hkScheme, szSelectedSizeValue,
        selectedName, &cbData);
    if (status != ERROR_SUCCESS)
        selectedName[0] = TEXT('\0');

    /* Load the internal names ("0" to "2") in a list, in order to get the
     * amount of sizes. */
    typedef struct tagLIST
    {
        TCHAR internalName[XP_SCHEMENAME_MAX_SIZE];
        struct tagLIST *next;
    } LIST;
    LIST *pSizes = NULL;
    LIST *pSize;

    DWORD iSize = 0;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        pSize = (LIST *)Alloc(0, sizeof(LIST));
        if (!pSize)
            break;

        cbData = XP_SCHEMENAME_MAX_SIZE;
        status = RegEnumKeyEx(hkSizes, iSize, pSize->internalName, &cbData,
            NULL, NULL, NULL, NULL);
        iSize++;
        if (status != ERROR_SUCCESS)
        {
            Free(pSize);
            continue;
        }

        pSize->next = pSizes;
        pSizes = pSize;
    }
    /* Leave the size count (it increments one extra time when there are no
     * more items)
     */
    iSize--;

    /* Import the data for each size */
    BOOL ret = FALSE;
    for (pSize = pSizes; pSize; pSize = pSize->next)
    {
        ret |= ImportXpSchemeSize(hkSizes,
            schemeInternalName, schemeDisplayName, pSize->internalName,
            selectedName, (int)iSize);
    }

    /* Free the size list */
    pSize = pSizes;
    while (pSize)
    {
        pSizes = pSize->next;
        Free(pSize);
        pSize = pSizes;
    }

    return ret;
}

static
void ImportXpSchemes(void)
{
    HKEY hkSchemes;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szXpSchemesKey, 0,
        KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkSchemes);
    if (status != ERROR_SUCCESS)
        return;

    HKEY hkScheme;
    TCHAR internalName[XP_SCHEMENAME_MAX_SIZE];  /* 0 - 21 */
    TCHAR selectedName[XP_SCHEMENAME_MAX_SIZE];

    TCHAR rawName[MAX_PATH];  /* E.g. @themeui.dll,-9999 */
#if defined(UNICODE)
    TCHAR displayName[SCHEMENAME_MAX_SIZE];
#endif
    TCHAR *exportedName;

    DWORD cbData = sizeof(selectedName);
    status = RegQueryString(hkSchemes, szXpSelectedSchemeValue,
        selectedName, &cbData);
    if (status != ERROR_SUCCESS)
        selectedName[0] = TEXT('\0');

    DWORD iScheme = 0;
    status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        cbData = XP_SCHEMENAME_MAX_SIZE;
        status = RegEnumKeyEx(hkSchemes, iScheme, internalName, &cbData,
            NULL, NULL, NULL, NULL);
        iScheme++;
        if (status != ERROR_SUCCESS)
            continue;

        status = RegOpenKeyEx(hkSchemes, internalName, 0,
            KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &hkScheme);
        if (status != ERROR_SUCCESS)
            continue;

        cbData = sizeof(rawName);
        status = RegQueryString(hkScheme, szDisplayNameValue, rawName, &cbData);
        if (status != ERROR_SUCCESS)
            exportedName = internalName;
        else
        {
#if defined(UNICODE)
            if (SHLoadIndirectString(rawName, displayName,
                _countof(displayName), NULL) == S_OK)
            {
                exportedName = displayName;
            }
            else
#endif
            {
                rawName[SCHEMENAME_MAX_SIZE - 1] = TEXT('\0');
                exportedName = rawName;
            }
        }

        if (ImportXpSchemeSizes(hkScheme, internalName, exportedName) &&
            (lstrcmp(internalName, selectedName) == 0))
        {
            SaveSelectedSchemeName(exportedName);
        }

        RegCloseKey(hkScheme);
    }

    RegCloseKey(hkSchemes);
}
#endif  /* WINVER >= WINVER_XP && WINVER <= WINVER_7 */

static
void GetLegacySelectedName(_Out_writes_(MAX_PATH) TCHAR *selectedName)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szAppearanceKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
    {
        selectedName[0] = TEXT('\0');
        return;
    }

    DWORD cbData = MAX_PATH;
    status = RegQueryString(hKey, szLegacySelectedSchemeValue,
        selectedName, &cbData);

    if (status != ERROR_SUCCESS)
        selectedName[0] = TEXT('\0');

    RegCloseKey(hKey);
}

_Success_(return)
static
BOOL ImportLegacyScheme(
    _In_ const TCHAR *name, _In_ const BYTE *legacySchemeData)
{
    /* Format:
     * - WORD: Depends on the Windows version. 1 on 95, 4 on 98, 2 on 2000...
     * - WORD: Not present on 95; 0 otherwise
     * - NONCLIENTMETRICS (with varying cbSize on most versions for some reason)
     * - LOGFONT: Desktop icon font
     * - COLORREF array (little-endian)
     */

    SCHEME_DATA schemeData;

    /* Check version */
#if defined(UNICODE)
    /* NT */
    if (*legacySchemeData != 2)
        return FALSE;
#elif defined(WINVER_IS_98)
    if (*legacySchemeData != 4)
        return FALSE;
#elif defined(WINVER_IS_95)
    if (*legacySchemeData != 1)
        return FALSE;
#endif

#if defined(WINVER_IS_95)
    /* Win95 uses 16-bit instead of 32-bit for some of the values */
    legacySchemeData += sizeof(WORD);
#else
    legacySchemeData += sizeof(DWORD);
#endif

    /* Calculate the size for classic Windows manually, excluding
     * iPaddedBorderWidth and other possible future additions
     */
    const UINT cbSize = sizeof(UINT) + sizeof(int) * 9 + sizeof(LOGFONT) * 5;

    memcpy(&schemeData.ncMetrics, legacySchemeData, cbSize);
    legacySchemeData += cbSize;
    schemeData.ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
#if WINVER >= WINVER_VISTA
    schemeData.ncMetrics.iPaddedBorderWidth = 0;
#endif

#if defined(WINVER_IS_95)

#define LfWordToDWord(member) \
    schemeData.lfIconFont.member = \
        legacySchemeData[0] | (legacySchemeData[1] << 8); \
    legacySchemeData += 2

    LfWordToDWord(lfHeight);
    LfWordToDWord(lfWidth);
    LfWordToDWord(lfEscapement);
    LfWordToDWord(lfOrientation);
    LfWordToDWord(lfWeight);

#undef LfWordToDWord

    /* Copy the rest */
    memcpy((void *)((ULONG_PTR)&schemeData.lfIconFont + sizeof(LONG) * 5),
        legacySchemeData, sizeof(LOGFONT) - sizeof(LONG) * 5);
    legacySchemeData += (sizeof(LOGFONT) - sizeof(LONG) * 5);

#else  /* !defined(WINVER_IS_95) */
    memcpy(&schemeData.lfIconFont, legacySchemeData, sizeof(LOGFONT));
    legacySchemeData += sizeof(LOGFONT);
#endif

    memcpy(&schemeData.colors, legacySchemeData,
        sizeof(COLORREF) * NUM_COLORS_LEGACY);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#if WINVER >= WINVER_2K
    SetGradientCaptionsFromColors(&schemeData);
#endif
#if WINVER >= WINVER_XP
    schemeData.bFlatMenus = FALSE;
#endif
#endif

    if (!SaveSchemeColors(&schemeData, name))
        return FALSE;

    if (!SaveSchemeSize(&schemeData, name, TEXT("Normal")))
        return FALSE;

    return TRUE;
}

static
void ImportLegacySchemes(void)
{
    HKEY hkSchemes;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szLegacySchemesKey, 0,
        KEY_QUERY_VALUE, &hkSchemes);
    if (status != ERROR_SUCCESS)
        return;

    TCHAR selectedName[MAX_PATH];
    GetLegacySelectedName(selectedName);

    TCHAR rawName[MAX_PATH];
#if defined(UNICODE)
    TCHAR displayName[SCHEMENAME_MAX_SIZE];
#endif
    TCHAR *exportedName;
#if defined(UNICODE) && WINVER >= WINVER_7
    BOOL bMustFreeExportedName = FALSE;
#endif

#if defined(WINVER_IS_95)
#define LEGACY_DATA_SIZE sizeof(WORD) + \
    sizeof(UINT) + sizeof(int) * 9 + sizeof(LOGFONT) * 5 - sizeof(WORD) * 5 + \
    sizeof(LOGFONT) + sizeof(COLORREF) * NUM_COLORS_LEGACY
#else
#define LEGACY_DATA_SIZE sizeof(WORD) * 2 + \
    sizeof(UINT) + sizeof(int) * 9 + sizeof(LOGFONT) * 5 + \
    sizeof(LOGFONT) + sizeof(COLORREF) * NUM_COLORS_LEGACY
#endif

    BYTE legacySchemeData[LEGACY_DATA_SIZE];

    DWORD valueSize;
    DWORD dwType;
    DWORD cbData;
    DWORD iScheme = 0;
    status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        valueSize = MAX_PATH;
        cbData = LEGACY_DATA_SIZE;
        status = RegEnumValue(hkSchemes, iScheme,
            rawName, &valueSize,
            NULL, &dwType, legacySchemeData, &cbData);
        iScheme++;

        if (!(status == ERROR_SUCCESS && dwType == REG_BINARY &&
            cbData == LEGACY_DATA_SIZE))
        {
            continue;
        }

        rawName[MAX_PATH - 1] = TEXT('\0');
#if defined(UNICODE) && WINVER >= WINVER_2K
#if WINVER >= WINVER_7
        if (lstrcmpi(rawName, TEXT("@themeui.dll,-854")) == 0)
        {
            /* On Windows 7 it was renamed to "Windows Classic".
             * On Windows 8 and later, it is in the registry but not in the MUI
             * files.
             */
            exportedName = tcsdup(TEXT("Windows Standard"));
            bMustFreeExportedName = TRUE;
        }
        else
#endif
        if (SHLoadIndirectString(rawName, displayName,
            _countof(displayName), NULL) == S_OK)
        {
            exportedName = displayName;
        }
        else
#endif  /* defined(UNICODE) && WINVER >= WINVER_2K */
        {
            rawName[SCHEMENAME_MAX_SIZE - 1] = TEXT('\0');
            exportedName = rawName;
        }

        if (ImportLegacyScheme(exportedName, legacySchemeData) &&
            (lstrcmp(rawName, selectedName) == 0))
        {
            SaveSelectedSchemeName(exportedName);
        }

#if defined(UNICODE) && WINVER >= WINVER_7
        if (bMustFreeExportedName)
        {
            Free(exportedName);
            bMustFreeExportedName = FALSE;
        }
#endif
    }

#undef LEGACY_DATA_SIZE

    RegCloseKey(hkSchemes);
}

#endif  /* defined(WITH_MIGRATION) */

_Success_(return)
static
BOOL ApplySystemColors(_In_ const COLORREF *colors)
{
    int iColors[NUM_COLORS];
    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
        iColors[iColor] = iColor;

    return SetSysColors(NUM_COLORS, iColors, colors) != 0;
}

/* Save colors to registry.
 * The system does not do this when applying colors.
 * There are also colors saved in "Control Panel\\Desktop\\Colors", but they
 * are unused.
 */
_Success_(return)
static
BOOL SaveSystemColors(_In_ const COLORREF *colors)
{
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, szColorsKey, 0, NULL, 0,
        KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    BOOL ret = TRUE;
    TCHAR s[12];
    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
    {
        if (wsprintf(s,
            TEXT("%d %d %d"),
            GetRValue(colors[iColor]),
            GetGValue(colors[iColor]),
            GetBValue(colors[iColor])) < 5)
        {
            ret = FALSE;
            continue;
        }

        ret &= (RegSetValueEx(hKey, g_colorNames[iColor], 0, REG_SZ,
            (BYTE *)s, (lstrlen(s) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS);
    }

    RegCloseKey(hKey);
    return ret;
}

static
BOOL HaveColorsChanged(_In_ const SCHEME_DATA *pData)
{
    return memcmp(g_currentScheme.colors, pData->colors,
            sizeof(g_currentScheme.colors)) != 0;
}

static
BOOL HaveNcMetricsChanged(_In_ const SCHEME_DATA *pData)
{
    return memcmp(&g_currentScheme.ncMetrics, &pData->ncMetrics,
        sizeof(NONCLIENTMETRICS)) != 0;
}

static
BOOL HasIconFontChanged(_In_ const SCHEME_DATA *pData)
{
    return memcmp(&g_currentScheme.lfIconFont, &pData->lfIconFont,
        sizeof(LOGFONT)) != 0;
}

static
_Success_(return)
BOOL LoadCurrentColors(_Out_ SCHEME_DATA *pData)
{
    COLORREF color;
    BOOL ret = TRUE;

    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
    {
        color = (COLORREF)GetSysColor(iColor);
        if (color == CLR_INVALID)
            ret = FALSE;
        else
            pData->colors[iColor] = color;
    }

    return ret;
}

static
_Success_(return)
BOOL LoadCurrentNcMetrics(_Out_ SCHEME_DATA *pData)
{
    pData->ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);

    BOOL ret = SystemParametersInfo(
        SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),
        &pData->ncMetrics, 0);

    for (int iSize = 0; iSize < NUM_SIZES; iSize++)
        g_sizeRanges[iSize].current = GetSchemeMetric(&pData->ncMetrics, iSize);

    return ret;
}

static
_Success_(return)
BOOL LoadCurrentIconFont(_Out_ SCHEME_DATA *pData)
{
    return SystemParametersInfo(
        SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &pData->lfIconFont, 0);
}

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_2K
static
_Success_(return)
BOOL LoadCurrentGradientCaptionsSetting(_Out_ SCHEME_DATA *pData)
{
    return SystemParametersInfo(
        SPI_GETGRADIENTCAPTIONS, 0, &pData->bGradientCaptions, 0);
}
#endif

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_XP
static
_Success_(return)
BOOL LoadCurrentFlatMenusSetting(_Out_ SCHEME_DATA *pData)
{
    return SystemParametersInfo(SPI_GETFLATMENU, 0, &pData->bFlatMenus, 0);
}
#endif

/* Populates the passed scheme based on the current system settings.
 * Returns FALSE if there were any settings which could not be retrieved.
 */
_Success_(return)
BOOL LoadCurrentScheme(_Out_ SCHEME_DATA *pData)
{
    BOOL ret = TRUE;

    ret &= LoadCurrentColors(pData);
    ret &= LoadCurrentNcMetrics(pData);
    ret &= LoadCurrentIconFont(pData);
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_2K
    ret &= LoadCurrentGradientCaptionsSetting(pData);
#endif
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_XP
    ret &= LoadCurrentFlatMenusSetting(pData);
#endif

#if defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES)
    pData->desktopPattern = NULL;
#endif

    /* Set g_currentScheme values */
    memcpy(&g_currentScheme, pData, sizeof(SCHEME_DATA));

    return ret;
}

_Success_(return)
BOOL RefreshCurrentColors(void)
{
    return LoadCurrentColors(&g_currentScheme);
}

_Success_(return)
BOOL RefreshCurrentNcMetrics(void)
{
    return LoadCurrentNcMetrics(&g_currentScheme);
}

_Success_(return)
BOOL RefreshCurrentIconFont(void)
{
    return LoadCurrentIconFont(&g_currentScheme);
}

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_2K
_Success_(return)
BOOL RefreshCurrentGradientCaptionsSetting(void)
{
    return LoadCurrentGradientCaptionsSetting(&g_currentScheme);
}
#endif

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_XP
_Success_(return)
BOOL RefreshCurrentFlatMenusSetting(void)
{
    return LoadCurrentFlatMenusSetting(&g_currentScheme);
}
#endif

void LoadSchemesList(void)
{
    if (g_schemes->schemes)
        FreeSchemes(&g_schemes->schemes);

    g_schemes->schemes = GetSchemesFromRegistry();

#if defined(WITH_MIGRATION)
#if WINVER >= WINVER_XP && WINVER <= WINVER_7
    if (!g_schemes->schemes)
    {
        ImportXpSchemes();
        g_schemes->schemes = GetSchemesFromRegistry();
    }
#endif

    if (!g_schemes->schemes)
    {
        ImportLegacySchemes();
        g_schemes->schemes = GetSchemesFromRegistry();
    }
#endif  /* defined(WITH_MIGRATION) */

    if (!g_schemes->schemes)
        return;

    g_schemes->selected.color = NULL;
    g_schemes->selected.size = NULL;
}

_Ret_maybenull_
SCHEME *FindScheme(_In_ SCHEME *pSchemes, _In_ const TCHAR *name)
{
    for (SCHEME *pScheme = pSchemes; pScheme; pScheme = pScheme->next)
    {
        if (lstrcmpi(pScheme->name, name) == 0)
            return pScheme;
    }

    return NULL;
}

_Success_(return != NULL)
SCHEME *GetActiveSchemeSize(_In_ SCHEME *pScheme)
{
    TCHAR szSelectedSchemePath[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];
    if (wsprintf(
        szSelectedSchemePath, TEXT("%s\\%s"), szSchemesKey, pScheme->name) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return NULL;  /* Unselected */
    }

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSelectedSchemePath, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return pScheme->sizes;

    TCHAR szSelectedSize[SIZENAME_MAX_SIZE];

    DWORD cbData = sizeof(szSelectedSize);
    status = RegQueryString(hKey, szSelectedSizeValue, szSelectedSize, &cbData);
    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS)
        return NULL;

    SCHEME *pSchemeSize = FindScheme(pScheme->sizes, szSelectedSize);
    if (!pSchemeSize)
        return NULL;

    return pSchemeSize;
}

/* Gets the active classic scheme and populates selection with entries from the
 * list of loaded schemes
 */
_Success_(return)
BOOL GetActiveScheme(_In_ SCHEME *pSchemes, _Out_ SCHEME_SELECTION *pSelection)
{
    pSelection->color = NULL;
    pSelection->size = NULL;

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, szSchemesKey, 0,
        KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    TCHAR szSelectedScheme[SCHEMENAME_MAX_SIZE];
    DWORD cbData = sizeof(szSelectedScheme);
    LSTATUS status = RegQueryString(hKey, szSelectedSchemeValue,
        szSelectedScheme, &cbData);
    if (status != ERROR_SUCCESS)
    {
        /* Migrate the previous value name */

        cbData = sizeof(szSelectedScheme);
        status = RegQueryString(hKey, szXpSelectedSchemeValue,
            szSelectedScheme, &cbData);
        if (status != ERROR_SUCCESS)
            return FALSE;

        status = RegSetValueEx(hKey, szSelectedSchemeValue, 0, REG_SZ,
            (BYTE *)szSelectedScheme,
            (lstrlen(szSelectedScheme) + 1) * sizeof(TCHAR));
        if (status == ERROR_SUCCESS)
            RegDeleteValue(hKey, szXpSelectedSchemeValue);
    }

    RegCloseKey(hKey);

    SCHEME *pCurrentScheme = FindScheme(pSchemes, szSelectedScheme);
    if (!pCurrentScheme)
        return FALSE;

    pSelection->color = pCurrentScheme;

    pSelection->size = GetActiveSchemeSize(pCurrentScheme);
    return TRUE;
}

/* Populates the passed scheme with values retrieved from registry */
_Success_(return)
BOOL LoadScheme(
    _Out_ SCHEME_DATA *pData, _In_ const SCHEME_SELECTION *pSelection)
{
    if (!pSelection->color)
    {
        /* This should not happen */
        return FALSE;
    }

    TCHAR szSchemeKey[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];

    if (wsprintf(szSchemeKey, TEXT("%s\\%s"), szSchemesKey,
        pSelection->color->name) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return FALSE;
    }

    HKEY hkScheme;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSchemeKey, 0,
        KEY_QUERY_VALUE, &hkScheme);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR colorValueName[8];
    COLORREF color;
    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
    {
        if (wsprintf(colorValueName, TEXT("Color%d"), iColor) < 6)
            color = CLR_INVALID;
        else
            color = ReadSchemeColor(hkScheme, colorValueName);

        /* Default to current color */
        if (color == CLR_INVALID)
            color = g_currentScheme.colors[iColor];

        pData->colors[iColor] = color;
    }

#if WINVER >= WINVER_2K && defined(WITH_CLASSIC_ADVANCED_SETTINGS)

#if WINVER >= WINVER_XP
    pData->bFlatMenus = RegQueryBool(hkScheme, szFlatMenusValue,
        g_currentScheme.bFlatMenus);
#endif

    pData->bGradientCaptions = RegQueryBool(hkScheme, szGradientsValue,
        g_currentScheme.bGradientCaptions);

#endif

    RegCloseKey(hkScheme);

    /* If this returns FALSE, it means the size is modified. Load it anyways. */
    LoadSchemeSize(pData, pSelection);

    return TRUE;
}

/* Populates the passed scheme size with values retrieved from registry */
_Success_(return)
BOOL LoadSchemeSize(
    _Out_ SCHEME_DATA *pData, _In_ const SCHEME_SELECTION *pSelection)
{
    if (!pSelection->size)
        return FALSE;

    TCHAR szSizeKey[_countof(szSchemesKey) +
        SCHEMENAME_MAX_SIZE + SIZENAME_MAX_SIZE];
    HKEY hkSize = NULL;

    if (wsprintf(szSizeKey, TEXT("%s\\%s\\%s"),
        szSchemesKey, pSelection->color->name, pSelection->size->name) <
        (int)(_countof(szSchemesKey) + 3))
    {
        return FALSE;
    }

    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szSizeKey, 0,
        KEY_QUERY_VALUE, &hkSize);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL ret = TRUE;
    TCHAR valueName[8];
    for (int iFont = 0; iFont < NUM_FONTS; iFont++)
    {
        if (wsprintf(valueName, TEXT("Font%d"), iFont) >= 5)
            ret &= ReadSchemeFont(hkSize, valueName, iFont, pData);
        else
        {
#if defined(_MSC_VER)
            /* Ignore unitialized memory warning; the contents will be overwritten */
#pragma warning(push)
#pragma warning(disable: 6001)
#endif
            LOGFONT *pLogFont = GetSchemeFont(pData, iFont);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
            if (pLogFont)
                CopyDefaultFont(pLogFont, iFont);
        }
    }

    UpdateMinHeightsFromFonts(pData);

    int size;
    for (int iSize = 0; iSize < NUM_SIZES; iSize++)
    {
        size = ReadSchemeSize(hkSize, iSize);

        /* Default to current size */
        if (size < 0)
            size = GetSchemeMetric(&g_currentScheme.ncMetrics, iSize);

        SetSchemeMetric(&pData->ncMetrics, iSize, size);
    }

    RegCloseKey(hkSize);
    return ret;
}

_Success_(return >= 0)
int GetSchemeMetric(_In_ const NONCLIENTMETRICS *ncMetrics, int metric)
{
    switch (metric)
    {
    case SIZE_BORDER:
        return ncMetrics->iBorderWidth;

    case SIZE_SCROLLWIDTH:
        return ncMetrics->iScrollWidth;

    case SIZE_SCROLLHEIGHT:
        return ncMetrics->iScrollHeight;

    case SIZE_CAPTIONWIDTH:
        return ncMetrics->iCaptionWidth;

    case SIZE_CAPTIONHEIGHT:
        return ncMetrics->iCaptionHeight;

    case SIZE_SMCAPTIONWIDTH:
        return ncMetrics->iSmCaptionWidth;

    case SIZE_SMCAPTIONHEIGHT:
        return ncMetrics->iSmCaptionHeight;

    case SIZE_MENUWIDTH:
        return ncMetrics->iMenuWidth;

    case SIZE_MENUHEIGHT:
        return ncMetrics->iMenuHeight;

#if WINVER >= WINVER_VISTA
    case SIZE_PADDEDBORDER:
        return ncMetrics->iPaddedBorderWidth;
#endif
    }

    return -1;
}

/* Returns the value which was really set after adjusting it */
_Success_(return >= 0)
int SetSchemeMetric(_Out_ NONCLIENTMETRICS *ncMetrics, int metric, int value)
{
    if (value < g_sizeRanges[metric].min)
        value = g_sizeRanges[metric].min;
    else if (value > g_sizeRanges[metric].max)
        value = g_sizeRanges[metric].max;

    switch (metric)
    {
    case SIZE_BORDER:
        ncMetrics->iBorderWidth = value;
        break;

    case SIZE_SCROLLWIDTH:
        ncMetrics->iScrollWidth = value;
        break;

    case SIZE_SCROLLHEIGHT:
        ncMetrics->iScrollHeight = value;
        break;

    case SIZE_CAPTIONWIDTH:
        ncMetrics->iCaptionWidth = value;
        break;

    case SIZE_CAPTIONHEIGHT:
        ncMetrics->iCaptionHeight = value;
        break;

    case SIZE_SMCAPTIONWIDTH:
        ncMetrics->iSmCaptionWidth = value;
        break;

    case SIZE_SMCAPTIONHEIGHT:
        ncMetrics->iSmCaptionHeight = value;
        break;

    case SIZE_MENUWIDTH:
        ncMetrics->iMenuWidth = value;
        break;

    case SIZE_MENUHEIGHT:
        ncMetrics->iMenuHeight = value;
        break;

#if WINVER >= WINVER_VISTA
    case SIZE_PADDEDBORDER:
        ncMetrics->iPaddedBorderWidth = value;
        break;
#endif

    default:
        return -1;
    }

    g_sizeRanges[metric].current = value;
    return value;
}

_Ret_maybenull_
LOGFONT *GetSchemeFont(_In_ SCHEME_DATA *pData, int metric)
{
    switch (metric)
    {
    case FONT_CAPTION:
        return &pData->ncMetrics.lfCaptionFont;

    case FONT_SMCAPTION:
        return &pData->ncMetrics.lfSmCaptionFont;

    case FONT_MENU:
        return &pData->ncMetrics.lfMenuFont;

    case FONT_TOOLTIP:
        return &pData->ncMetrics.lfStatusFont;

    case FONT_MESSAGE:
        return &pData->ncMetrics.lfMessageFont;

    case FONT_DESKTOP:
        return &pData->lfIconFont;
    }

    return NULL;
}

void UpdateMinHeightFromFont(_In_ const LOGFONT *plfFont, int iSize)
{
    int fontHeight = GetFontHeight(plfFont);
    if (fontHeight <= 0)
        return;

    g_sizeRanges[iSize].min = fontHeight + 2;

    /* Do not set the minimum value above the maximum */
    if (g_sizeRanges[iSize].min > g_sizeRanges[iSize].max)
        g_sizeRanges[iSize].min = g_sizeRanges[iSize].max;
}

/* Applies the selected scheme and stores its name in the registry.
 */
void ApplyScheme(
    _Inout_ SCHEME_DATA *pData, _In_opt_ const SCHEME_SELECTION *pSelection)
{
    BOOL changed = FALSE;

    if (HaveColorsChanged(pData))
    {
        ApplySystemColors(pData->colors);
        SaveSystemColors(pData->colors);
        changed = TRUE;
    }

    if (HaveNcMetricsChanged(pData))
    {
        /* Applying non-client metrics can be slow.
         * They are saved to "HKCU\Control Panel\Desktop\WindowMetrics".
         * This can hang with certain programs opened... closing them resumes
         * the operation. Passing no fWinIni flags changes nothing.
         */
        SystemParametersInfo(SPI_SETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),
            &pData->ncMetrics, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        changed = TRUE;
    }

    if (HasIconFontChanged(pData))
    {
        SystemParametersInfo(SPI_SETICONTITLELOGFONT, sizeof(LOGFONT),
            &pData->lfIconFont, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        changed = TRUE;
    }

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#if WINVER >= WINVER_2K
    if (pData->bGradientCaptions != g_currentScheme.bGradientCaptions)
    {
        SystemParametersInfo(
            SPI_SETGRADIENTCAPTIONS, 0, IntToPtr(pData->bGradientCaptions),
            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        g_currentScheme.bGradientCaptions = pData->bGradientCaptions;
        changed = TRUE;
    }
#endif

#if WINVER >= WINVER_XP
    if (pData->bFlatMenus != g_currentScheme.bFlatMenus)
    {
        SystemParametersInfo(
            SPI_SETFLATMENU, 0, IntToPtr(pData->bFlatMenus),
            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        g_currentScheme.bFlatMenus = pData->bFlatMenus;
        changed = TRUE;
    }
#endif
#endif

    if (changed)
        DeleteSystemSelectedScheme();

    if (!pSelection || !pSelection->color)
    {
        DeleteSelectedSchemeName();
        return;
    }

    SaveSelectedSchemeName(pSelection->color->name);

    if (pSelection->size)
    {
        SaveSelectedSizeName(
            pSelection->color->name, pSelection->size->name);
    }
    else
        DeleteSelectedSizeName(pSelection->color->name);
}

_Ret_maybenull_
SCHEME *CreateScheme(_In_ const TCHAR *name)
{
    SCHEME *pScheme = NULL;

    pScheme = (SCHEME *)Alloc(0, sizeof(SCHEME));
    if (!pScheme)
        return NULL;

    pScheme->name = tcsdup(name);
    if (!pScheme->name)
    {
        Free((void *)pScheme);
        pScheme = NULL;
        return NULL;
    }

    pScheme->sizes = NULL;
    pScheme->next = NULL;

    return pScheme;
}

_Success_(return)
BOOL SaveSchemeColors(_In_ SCHEME_DATA *pData, _In_ const TCHAR *schemeName)
{
    TCHAR szSchemeKey[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];

    if (wsprintf(szSchemeKey, TEXT("%s\\%s"), szSchemesKey, schemeName) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return FALSE;
    }

    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, szSchemeKey, 0, NULL, 0,
        KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    TCHAR valueName[8];

    BOOL ret = TRUE;

    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
    {
        if (wsprintf(valueName, TEXT("Color%d"), iColor) < 6)
            continue;

        ret &= (RegSetValueEx(
            hKey, valueName, 0, REG_DWORD, (BYTE *)&pData->colors[iColor],
            sizeof(COLORREF)) == ERROR_SUCCESS);
    }

#if WINVER >= WINVER_2K && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    DWORD dwData;

#if WINVER >= WINVER_XP
    dwData = (DWORD)pData->bFlatMenus;
    ret &= (RegSetValueEx(hKey, szFlatMenusValue, 0, REG_DWORD,
        (BYTE *)&dwData, sizeof(DWORD)) == ERROR_SUCCESS);
#endif

    dwData = (DWORD)pData->bGradientCaptions;
    ret &= (RegSetValueEx(hKey, szGradientsValue, 0, REG_DWORD,
        (BYTE *)&dwData, sizeof(DWORD)) == ERROR_SUCCESS);
#endif

    RegCloseKey(hKey);

    return ret;
}

_Success_(return)
BOOL SaveSchemeSize(_In_ SCHEME_DATA *pData,
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName)
{
    HKEY hKey;

    TCHAR szSizeKey[_countof(szSchemesKey) +
        SCHEMENAME_MAX_SIZE + SIZENAME_MAX_SIZE];
    if (wsprintf(
        szSizeKey, TEXT("%s\\%s\\%s"), szSchemesKey, schemeName, sizeName) <
        (int)(_countof(szSchemesKey) + 3))
    {
        return FALSE;
    }

    if (RegCreateKeyEx(HKEY_CURRENT_USER, szSizeKey, 0, NULL, 0,
        KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    TCHAR valueName[7];
    BOOL ret = TRUE;

    for (int iFont = 0; iFont < NUM_FONTS; iFont++)
    {
        if (wsprintf(valueName, TEXT("Font%d"), iFont) < 5)
        {
            ret = FALSE;
            continue;
        }

        LOGFONT *pLogFont = GetSchemeFont(pData, iFont);
        if (!pLogFont)
        {
            ret = FALSE;
            continue;
        }

        LONG previousFontHeight = pLogFont->lfHeight;
        LONG previousFontWidth  = pLogFont->lfWidth;

        if (g_propSheet.dpi != USER_DEFAULT_SCREEN_DPI)
        {
            pLogFont->lfHeight = UnscaleForDpi(pLogFont->lfHeight);
            pLogFont->lfWidth = UnscaleForDpi(pLogFont->lfWidth);
        }

        /* Zero out the empty space of the string to avoid storing junk in
         * the registry
         */
        int len = lstrlen(pLogFont->lfFaceName) + 1;
        if (len < LF_FACESIZE)
        {
            memset(&pLogFont->lfFaceName[len], 0,
                (LF_FACESIZE - len) * sizeof(TCHAR));
        }

        /* Always save as Unicode */
#if defined(UNICODE)
        RegSetValueEx(
            hKey, valueName, 0, REG_BINARY, (BYTE *)pLogFont, sizeof(LOGFONT));
#else
        LOGFONTW logFontW;

        /* Keep the analyzers happy */
        memset(&logFontW, 0, sizeof(LOGFONTW));

        /* Copy everything except the last part, lfFaceName */
        memcpy(&logFontW, pLogFont,
            sizeof(LOGFONT) - LF_FACESIZE * sizeof(TCHAR));

        if (MultiByteToWideChar(CP_ACP, 0, pLogFont->lfFaceName, -1,
            logFontW.lfFaceName, LF_FACESIZE))
        {
            ret &= (RegSetValueEx(hKey, valueName, 0, REG_BINARY,
                (BYTE *)&logFontW, sizeof(LOGFONTW)) == ERROR_SUCCESS);
        }
        else
            ret = FALSE;
#endif

        if (g_propSheet.dpi != USER_DEFAULT_SCREEN_DPI)
        {
            pLogFont->lfHeight = previousFontHeight;
            pLogFont->lfWidth = previousFontWidth;
        }
    }

    int size;
    DWORD dwData;
    for (int iSize = 0; iSize < NUM_SIZES; iSize++)
    {
        if (wsprintf(valueName, TEXT("Size%d"), iSize) < 5)
        {
            ret = FALSE;
            continue;
        }

        size = GetSchemeMetric(&pData->ncMetrics, iSize);
        if (g_propSheet.dpi != USER_DEFAULT_SCREEN_DPI)
            size = UnscaleForDpi(size);

        dwData = (DWORD)size;
        ret &= (RegSetValueEx(hKey, valueName, 0, REG_DWORD, (BYTE *)&dwData,
            sizeof(DWORD)) == ERROR_SUCCESS);
    }

    RegCloseKey(hKey);

    return ret;
}

void SaveSchemeSelection(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName)
{
    SaveSelectedSchemeName(schemeName);
    SaveSelectedSizeName(schemeName, sizeName);
}

_Success_(return)
BOOL RenameScheme(_Inout_ SCHEME *pScheme, _In_ const TCHAR *newName)
{
#if WINVER >= WINVER_VISTA && defined(UNICODE)
    /* Rename the key */

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(
        HKEY_CURRENT_USER, szSchemesKey, 0, READ_CONTROL, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegRenameKey(hKey, pScheme->name, newName);

    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS)
        return FALSE;

#else
    /* Save the scheme with the new name and delete the old one */

    /* Save all the sizes with the new scheme name */
    BOOL ret = TRUE;

    SCHEME_SELECTION selection;
    selection.color = pScheme;
    selection.size = pScheme->sizes;

    SCHEME_DATA data;
    if (!LoadScheme(&data, &selection))
        return FALSE;

    ret &= SaveSchemeColors(&data, newName);

    while (selection.size)
    {
        if (LoadSchemeSize(&data, &selection))
            ret &= SaveSchemeSize(&data, newName, selection.size->name);
        else
            ret = FALSE;

        selection.size = selection.size->next;
    }

    if (!ret)
        return FALSE;

    /* If all the sizes were saved, delete the old scheme */
    if (!DeleteSchemeRegistry(pScheme->name, TEXT("")))
        return FALSE;

#endif  /* WINVER < WINVER_VISTA */

    Free(pScheme->name);
    pScheme->name = tcsdup(newName);

    return TRUE;
}

_Success_(return)
BOOL RenameSchemeSize(
    _In_ SCHEME *pScheme, _Inout_ SCHEME *pSize, _In_ const TCHAR *newSizeName)
{
#if WINVER >= WINVER_VISTA && defined(UNICODE)
    /* Rename the key */

    TCHAR szSizeKey[_countof(szSchemesKey) + SCHEMENAME_MAX_SIZE];

    if (wsprintf(szSizeKey, TEXT("%s\\%s"), szSchemesKey, pScheme->name) <
        (int)(_countof(szSchemesKey) + 1))
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(
        HKEY_CURRENT_USER, szSizeKey, 0, READ_CONTROL, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegRenameKey(hKey, pSize->name, newSizeName);
    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS)
        return FALSE;

#else
    /* Save the scheme size with the new name and delete the old one */

    SCHEME_SELECTION selection;
    selection.color = pScheme;
    selection.size = pSize;

    SCHEME_DATA data;
    if (!LoadSchemeSize(&data, &selection))
        return FALSE;

    if (!SaveSchemeSize(&data, pScheme->name, newSizeName))
        return FALSE;

    if (!DeleteSchemeRegistry(pScheme->name, pSize->name))
        return FALSE;

#endif  /* WINVER < WINVER_VISTA */

    Free(pSize->name);
    pSize->name = tcsdup(newSizeName);

    return TRUE;
}

_Success_(return)
BOOL DeleteSchemeFromList(_Inout_ SCHEME **ppList, _Inout_ SCHEME **ppScheme)
{
    SCHEME *node = *ppList;

    if (node == *ppScheme)
    {
        /* First element */
        *ppList = (*ppScheme)->next;
        FreeScheme(ppScheme);
        return TRUE;
    }

    /* Find previous scheme */
    while (node && node->next != *ppScheme)
        node = node->next;

    if (!node)
        return FALSE;

    /* Found */
    node->next = (*ppScheme)->next;
    FreeScheme(ppScheme);
    return TRUE;
}

_Success_(return)
BOOL DeleteScheme(_Inout_ SCHEME **ppScheme)
{
    if (!DeleteSchemeRegistry((*ppScheme)->name, TEXT("")))
        return FALSE;

    return DeleteSchemeFromList(&g_schemes->schemes, ppScheme);
}

_Success_(return)
BOOL DeleteSize(_Inout_ SCHEME **ppScheme, _Inout_ SCHEME **ppSize)
{
    if (!DeleteSchemeRegistry((*ppScheme)->name, (*ppSize)->name))
        return FALSE;

    return DeleteSchemeFromList(&(*ppScheme)->sizes, ppSize);
}

void FreeScheme(_Inout_ SCHEME **ppScheme)
{
    if ((*ppScheme)->sizes)
        FreeSchemes(&(*ppScheme)->sizes);

    if ((*ppScheme)->name)
        Free((*ppScheme)->name);

    Free(*ppScheme);
    *ppScheme = NULL;
}

void FreeSchemes(_Inout_ SCHEME **ppSchemes)
{
    SCHEME *pScheme = *ppSchemes;
    SCHEME *pSchemeOld;

    while (pScheme)
    {
        pSchemeOld = pScheme;
        pScheme = pScheme->next;
        FreeScheme(&pSchemeOld);
    }

    *ppSchemes = NULL;
}

#if defined(WITH_CMDLINE) || defined(WITH_THEMES)
void SetWin3Style(_Inout_ SCHEME_DATA *pData)
{
    pData->colors[COLOR_3DDKSHADOW] = RGB(0, 0, 0);
    pData->colors[COLOR_3DLIGHT] = pData->colors[COLOR_WINDOWFRAME];
    pData->colors[COLOR_INFOTEXT] =  pData->colors[COLOR_WINDOWTEXT];
    pData->colors[COLOR_INFOBK] = pData->colors[COLOR_WINDOW];
#if WINVER >= WINVER_2K
    pData->colors[COLOR_ALTERNATEBTNFACE] = pData->colors[COLOR_3DFACE];
    pData->colors[COLOR_HOTLIGHT] = RGB(0, 0, 255);
    pData->colors[COLOR_GRADIENTACTIVECAPTION] =
        pData->colors[COLOR_ACTIVECAPTION];
    pData->colors[COLOR_GRADIENTINACTIVECAPTION] =
        pData->colors[COLOR_INACTIVECAPTION];
#endif
#if WINVER >= WINVER_XP
    pData->colors[COLOR_MENUHILIGHT] = pData->colors[COLOR_HIGHLIGHT];
    pData->colors[COLOR_MENUBAR] = pData->colors[COLOR_MENU];
#endif

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#if WINVER >= WINVER_2K
    pData->bGradientCaptions = FALSE;
#endif
#if WINVER >= WINVER_XP
    pData->bFlatMenus = TRUE;
#endif
#endif
}

void SetWin16Metrics(_Out_ SCHEME_DATA *pData)
{
    pData->lfIconFont.lfHeight = FontPtToLog(10);
    pData->lfIconFont.lfWidth = 0;
    pData->lfIconFont.lfEscapement = 0;
    pData->lfIconFont.lfOrientation = 0;
    pData->lfIconFont.lfWeight = FW_DONTCARE;
    pData->lfIconFont.lfItalic = 0;
    pData->lfIconFont.lfUnderline = 0;
    pData->lfIconFont.lfStrikeOut = 0;
    pData->lfIconFont.lfCharSet = ANSI_CHARSET;
    pData->lfIconFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    pData->lfIconFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    pData->lfIconFont.lfQuality = DEFAULT_QUALITY;
    pData->lfIconFont.lfPitchAndFamily = DEFAULT_PITCH;
    lstrcpy(pData->lfIconFont.lfFaceName, TEXT("System"));

    pData->ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
    pData->ncMetrics.iBorderWidth = 2;
    pData->ncMetrics.iScrollWidth = 17;
    pData->ncMetrics.iScrollHeight = 17;
    pData->ncMetrics.iCaptionWidth = 20;
    pData->ncMetrics.iCaptionHeight = 20;
    pData->ncMetrics.lfCaptionFont = pData->lfIconFont;
    pData->ncMetrics.iSmCaptionWidth = 20;
    pData->ncMetrics.iSmCaptionHeight = 20;
    pData->ncMetrics.lfSmCaptionFont = pData->lfIconFont;
    pData->ncMetrics.iMenuWidth = 19;
    pData->ncMetrics.iMenuHeight = 19;
    pData->ncMetrics.lfMenuFont = pData->lfIconFont;
    pData->ncMetrics.lfStatusFont = pData->lfIconFont;
    pData->ncMetrics.lfMessageFont = pData->lfIconFont;
#if WINVER >= WINVER_VISTA
    pData->ncMetrics.iPaddedBorderWidth = 0;
#endif
}
#endif  /* #if defined(WITH_CMDLINE) || defined(WITH_THEMES) */
