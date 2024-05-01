/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Themes handling
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if defined(WITH_THEMES)

/* This is only useful from Windows 8, as until Windows 7 the system applies
 * themes fine, and it supports both classic and modern styles.
 * This only supports the classic styles, as undocumented functions for applying
 * modern styles do not work anymore.
 */

#include "bg.h"
#include "classtyl.h"
#include "mincrt.h"
#include "reg.h"
#include "ssave.h"
#include "util.h"
#include "uxtundoc.h"

#include <shellapi.h>
#include <Uxtheme.h>

#define STRICT_TYPED_ITEMIDS
#include <ShlObj.h>

#define UNSET_VALUE 666

/* Registry key and value names for allowing changing specific elements */

static const TCHAR szThemesKey[] =
    TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes");

static const TCHAR szChangeStyleValue[] =
    TEXT("ThemeChangesStyle");

static const TCHAR szChangeMetricsValue[] =
    TEXT("ThemeChangesMetrics");

static const TCHAR szChangeBackgroundValue[] =
    TEXT("ThemeChangesBackground");

static const TCHAR szChangeIconsValue[] =
    TEXT("ThemeChangesDesktopIcons");

static const TCHAR szChangePointersValue[] =
    TEXT("ThemeChangesMousePointers");

static const TCHAR szChangeSoundsValue[] =
    TEXT("ThemeChangesSounds");

static const TCHAR szChangeScreenSaverValue[] =
    TEXT("ThemeChangesScreenSaver");

/* Key and value names in .Theme files and the registry */

static const TCHAR szMtsKey[] =
    TEXT("MasterThemeSelector");

static const TCHAR szMtsValue[] =
    TEXT("MTSM");

#if WINVER >= WINVER_2K
static const TCHAR szHighContrastValue[] =
    TEXT("HighContrast");
#endif

#if WINVER >= WINVER_XP
static const TCHAR szStyleKey[] =
    TEXT("VisualStyles");

static const TCHAR szStylePathValue[] =
    TEXT("Path");

static const TCHAR szStyleColorValue[] =
    TEXT("ColorStyle");

static const TCHAR szStyleSizeValue[] =
    TEXT("Size");
#endif

#if WINVER >= WINVER_VISTA
static const TCHAR szDwmValue[] =
    TEXT("Composition");
#endif

#if WINVER >= WINVER_7
static const TCHAR szDwmColorValue[] =
    TEXT("ColorizationColor");
#endif

#if WINVER >= WINVER_8
static const TCHAR szDwmAutoColorValue[] =
    TEXT("AutoColorization");
#endif

#if WINVER >= WINVER_10
static const TCHAR szTransparencyValue[] =
    TEXT("Transparency");

static const TCHAR szSystemModeValue[] =
    TEXT("SystemMode");

static const TCHAR szAppModeValue[] =
    TEXT("AppMode");

static const TCHAR szLightMode[] =
    TEXT("Light");

static const TCHAR szDarkMode[] =
    TEXT("Dark");

static const TCHAR szPersonalizeRegKey[] =
    TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");

static const TCHAR szTransparencyRegValue[] =
    TEXT("EnableTransparency");

static const TCHAR szSystemLightModeRegValue[] =
    TEXT("SystemUsesLightTheme");

static const TCHAR szAppLightModeRegValue[] =
    TEXT("AppsUseLightTheme");
#endif

static const TCHAR szWinIniDesktopSection[] =
    TEXT("Desktop");

static const TCHAR szClassicStyleKey[] =
    TEXT("ClassicStyle");

static const TCHAR szMetricsKey[] =
    TEXT("Metrics");

static const TCHAR szNcMetricsValue[] =
    TEXT("NonclientMetrics");

static const TCHAR szPicturePositionValue[] =
    TEXT("PicturePosition");

static const TCHAR szMultimonBgValue[] =
    TEXT("MultimonBackgrounds");

static const TCHAR szIconMetricsValue[] =
    TEXT("IconMetrics");

static const TCHAR szSlideshowKey[] =
    TEXT("Slideshow");

static const TCHAR szSlideshowDirValue[] =
    TEXT("ImagesRootPath");

static const TCHAR szSlideshowIntervalValue[] =
    TEXT("Interval");

static const TCHAR szSlideshowShuffleValue[] =
    TEXT("Shuffle");

static const TCHAR szDefaultValue[] =
    TEXT("DefaultValue");

static const TCHAR szRecycleBinEmptyValue[] =
    TEXT("Empty");

static const TCHAR szRecycleBinFullValue[] =
    TEXT("Full");

static const TCHAR szPointersKey[] =
    TEXT("Control Panel\\Cursors");

static const TCHAR szScreenSaverSection[] =
    TEXT("boot");

/* Theme icon UUIDs */

static const TCHAR szComputerIconUuid[] =
    TEXT("20D04FE0-3AEA-1069-A2D8-08002B30309D");

static const TCHAR szUserFilesIconUuid[] =
    TEXT("59031A47-3F72-44A7-89C5-5595FE6B30EE");

/* My Documents (legacy) */
static const TCHAR szUserFilesIconUuidOld[] =
    TEXT("450D8FBA-AD25-11D0-98A8-0800361B1103");

static const TCHAR szNetworkIconUuid[] =
    TEXT("F02C1A0D-BE21-4350-88B0-7367FC96EF3C");

/* My Network Places (legacy) */
static const TCHAR szNetworkIconUuidOld[] =
    TEXT("208D2C60-3AEA-1069-A2D7-08002B30309D");

static const TCHAR szRecycleBinIconUuid[] =
    TEXT("645FF040-5081-101B-9F08-00AA002F954E");

typedef struct tagTHEME_CHANGES_ALLOWED
{
    BOOL style;
    BOOL metrics;
    BOOL background;
    BOOL icons;
    BOOL pointers;
    BOOL sounds;
    BOOL screenSaver;
} THEME_CHANGES_ALLOWED;

static
const TCHAR *g_pointerNames[] = {
    TEXT("Arrow"),
    TEXT("Help"),
    TEXT("AppStarting"),
    TEXT("Wait"),
    TEXT("Crosshair"),
    TEXT("IBeam"),
    TEXT("NWPen"),
    TEXT("No"),
    TEXT("SizeNS"),
    TEXT("SizeWE"),
    TEXT("SizeNWSE"),
    TEXT("SizeNESW"),
    TEXT("SizeAll"),
    TEXT("UpArrow"),
    TEXT("Hand")
};

static THEME_CHANGES_ALLOWED g_themeChangesAllowed;

static
BOOL ReadThemeChangesAllowed(void)
{
    /* By default, allow changing everything */
    g_themeChangesAllowed.style       = TRUE;
    g_themeChangesAllowed.metrics     = TRUE;
    g_themeChangesAllowed.background  = TRUE;
    g_themeChangesAllowed.icons       = TRUE;
    g_themeChangesAllowed.pointers    = TRUE;
    g_themeChangesAllowed.sounds      = TRUE;
    g_themeChangesAllowed.screenSaver = TRUE;

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szThemesKey, 0,
        KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    /* Only ThemeChangesDesktopIcons and ThemeChangesMousePointers are saved
     * and respected by Windows (even though the GUI option to configure if
     * themes change pointers or not was removed for some reason).
     * The rest are custom values.
     * Only if the values are present and set to 0 the system does not set the
     * icons and pointers in theme files.
     */

    /* Also create the values if they do not exist, to indicate the user which
     * values can be changed.
     */
    DWORD dwValue;
    DWORD dwAllow = 1;
#define QueryDisallowed(value, member) \
    status = RegQueryDWord(hKey, value, &dwValue); \
    if (status == ERROR_SUCCESS) { \
        if (dwValue == 0) g_themeChangesAllowed.member = FALSE; \
    } else \
        RegSetValueEx(hKey, value, 0, REG_DWORD, \
            (BYTE *)&dwAllow, sizeof(DWORD))

    QueryDisallowed(szChangeStyleValue,       style);
    QueryDisallowed(szChangeMetricsValue,     metrics);
    QueryDisallowed(szChangeBackgroundValue,  background);
    QueryDisallowed(szChangeIconsValue,       icons);
    QueryDisallowed(szChangePointersValue,    pointers);
    QueryDisallowed(szChangeSoundsValue,      sounds);
    QueryDisallowed(szChangeScreenSaverValue, screenSaver);

#undef QueryDisallowed

    RegCloseKey(hKey);
    return TRUE;
}

_Success_(return)
static
BOOL ReadThemePath(
    _In_ const TCHAR *themePath,
    _In_ const TCHAR *szSection,
    _In_ const TCHAR *szKey,
    _Out_writes_(MAX_PATH) TCHAR *path)
{
    if (GetPrivateProfileString(szSection, szKey, NULL,
        path, MAX_PATH, themePath) < 4)
    {
        return FALSE;
    }

    GetAbsolutePath(themePath, path, MAX_PATH);
    return PathFileExists(path);
}

/* If a resourceId pointer is given, the path can end with ,ID (eg ,0 or ,-1).
 * Returns FALSE if no path was found, or if the path does not exist.
 */
_Success_(return)
BOOL ReadThemeExpandedPath(
    _In_ const TCHAR *themePath,
    _In_ const TCHAR *szSection,
    _In_ const TCHAR *szKey,
    _Out_writes_(MAX_PATH) TCHAR *path,
    _Out_writes_opt_(MAX_PATH - 2) TCHAR *realPath,
    _Out_opt_ int *resourceId)
{
    TCHAR origPath[MAX_PATH];

    if (GetPrivateProfileString(szSection, szKey, NULL,
        origPath, MAX_PATH, themePath) < 4)
    {
        return FALSE;
    }

    DWORD len;
#if defined(UNICODE)
    len = ExpandEnvironmentStrings(origPath, path, MAX_PATH);
    if (len == 0 || len > MAX_PATH)
        return FALSE;
#else
    /* -1 required by ExpandEnvironmentStringsA() */
    len = ExpandEnvironmentStrings(origPath, path, MAX_PATH - 1);
    if (len == 0 || len > MAX_PATH - 1)
        return FALSE;
#endif

    GetAbsolutePath(themePath, path, MAX_PATH);

    if (PathFileExists(path))
    {
        if (realPath)
            lstrcpy(realPath, path);
        if (resourceId)
            *resourceId = 0;
        return TRUE;
    }
    else if (!resourceId)
        return FALSE;

    /* Check without ",ID" */
    for (UINT c = lstrlen(path) - 2; c > 0; c--)
    {
        if (path[c] == TEXT(','))
        {
            path[c] = '\0';
            if (!PathFileExists(path))
                return FALSE;
            if (realPath)
                lstrcpy(realPath, path);
            path[c] = TEXT(',');

            *resourceId = ttoui(&path[c + 1], NULL);
            return TRUE;
        }

        if (path[c] >= TEXT('0') && path[c] <= TEXT('9'))
            continue;

        if (path[c] == TEXT('-') && path[c - 1] == TEXT(','))
            continue;

        return FALSE;
    }

    return FALSE;
}

/* Paths inside theme files can include:
 * - %ResourceDir% (XP and later); does not include a trailing backslash
 * - %ThemeDir% (Plus! and later); does include a trailing backslash on Plus!
 * Neither one is a normal environment variable, so create them for this
 * process.
 */
_Success_(return)
static
BOOL SetEnvironmentVariables(void)
{
    BOOL ret = FALSE;
    TCHAR path[MAX_PATH];

    UINT len = GetWindowsDirectory(path, MAX_PATH - 11);
    if (len >= 2 && len < MAX_PATH - 10 &&
        lstrcpy(&path[len], TEXT("\\Resources")))
    {
        ret &= SetEnvironmentVariable(TEXT("ResourceDir"), path);
    }

#if 0
    TCHAR *pfPath;
    HRESULT hr = SHGetKnownFolderPath(
        &FOLDERID_ProgramFilesX86, 0, NULL, &pfPath);
    if (hr == S_OK && lstrlen(pfPath) < MAX_PATH - 14)
    {
        if (wsprintf(path, TEXT("%s\\Plus!\\Themes\\"), pfPath) >= 16)
            ret &= SetEnvironmentVariable(TEXT("ThemeDir"), path);
    }

    CoTaskMemFree(pfPath);
#else
    /* Create empty so that paths relative to the .Theme file are used. */
    ret &= SetEnvironmentVariable(TEXT("ThemeDir"), TEXT(""));
#endif

    return ret;
}

#if WINVER >= WINVER_XP
_Success_(return)
static
BOOL RunRegCommand(_In_ const TCHAR *szRegValue)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Classic"),
        0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR command[MAX_PATH];
    DWORD cbData = sizeof(command);
    status = RegQueryString(hKey, szRegValue, command, &cbData);

    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS)
        return FALSE;

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_NORMAL;

    PROCESS_INFORMATION pi;
    BOOL ret = CreateProcess(NULL, command,
        NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL,
        &si, &pi);
    if (!ret)
        return FALSE;

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return TRUE;
}

_Success_(return)
static
BOOL EnableVisualStyles(BOOL bEnable)
{
    if (bEnable == IsThemeActive())
        return TRUE;

#if WINVER <= WINVER_7
    EnableTheming(bEnable);
#else
    if (bEnable)
        return RunRegCommand(TEXT("DisableClassicStyleCommand"));
    else
        return RunRegCommand(TEXT("EnableClassicStyleCommand"));
#endif
}
#endif  /* WINVER >= WINVER_XP */

#if WINVER >= WINVER_VISTA
_Success_(return)
static
BOOL EnableDwm(BOOL bEnable)
{
    if (bEnable == IsCompositionActive())
        return TRUE;

#if WINVER <= WINVER_7
    if (bEnable)
        return DwmEnableComposition(DWM_EC_ENABLECOMPOSITION) == S_OK;
    else
        return DwmEnableComposition(DWM_EC_DISABLECOMPOSITION) == S_OK;
#else
    if (bEnable)
        return RunRegCommand(TEXT("EnableDwmCommand"));
    else
        return RunRegCommand(TEXT("DisableDwmCommand"));
#endif
}
#endif  /* WINVER >= WINVER_VISTA */

#if WINVER >= WINVER_XP && defined(UNICODE)
/* Note that this returns FALSE if multiple consecutive backslashes are in the
 * file path, or if empty values are given to apply the default color and/or
 * size.
 */
static
BOOL IsCurrentVisualStyle(
    _In_ const WCHAR *newFile,
    _In_ const WCHAR *newColor,
    _In_ const WCHAR *newSize)
{
    WCHAR currentFile[MAX_PATH];
    WCHAR currentColor[50];
    WCHAR currentSize[50];
    HRESULT hr = GetCurrentThemeName(
        currentFile,  _countof(currentFile),
        currentColor, _countof(currentColor),
        currentSize,  _countof(currentSize));

    return hr == S_OK &&
        lstrcmpiW(currentFile, newFile) == 0 &&
        lstrcmpiW(currentColor, newColor) == 0 &&
        lstrcmpiW(currentSize,  newSize)  == 0;
}
#endif

_Success_(return)
static
BOOL ApplyClassicScheme(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName,
    _Inout_ SCHEME_DATA *pSchemeData)
{
    if (schemeName[0] == TEXT('\0') || sizeName[0] == TEXT('\0'))
        return FALSE;

    if (lstrlen(schemeName) > SCHEMENAME_MAX_SIZE - 1)
        return FALSE;

    if (lstrlen(sizeName) > SIZENAME_MAX_SIZE - 1)
        return FALSE;

    SCHEME_SELECTION selection;
    selection.color = CreateScheme(schemeName);
    if (!selection.color)
        return FALSE;

    selection.size = CreateScheme(sizeName);
    if (!selection.size)
        return FALSE;

    if (!LoadScheme(pSchemeData, &selection))
    {
        FreeScheme(&selection.color);
        FreeScheme(&selection.size);
        return FALSE;
    }

    FreeScheme(&selection.color);
    FreeScheme(&selection.size);
    return TRUE;
}

_Success_(return)
static
BOOL ApplyThemeStyle(
    _In_ const TCHAR *themePath, _Inout_opt_ SCHEME_DATA *pSchemeData)
{
#if WINVER >= WINVER_XP && defined(UNICODE)
    TCHAR mssPath[MAX_PATH];
    BOOL bStyle = ReadThemeExpandedPath(themePath,
        szStyleKey, szStylePathValue, mssPath, NULL, NULL);

    BOOL ret = EnableVisualStyles(bStyle);
#endif

    TCHAR szColor[50];
    if (GetPrivateProfileString(szStyleKey, szStyleColorValue, NULL,
        szColor, _countof(szColor), themePath) < 1)
    {
        szColor[0] = TEXT('\0');
    }

    TCHAR szSize[50];
    if (GetPrivateProfileString(szStyleKey, szStyleSizeValue, NULL,
        szSize, _countof(szSize), themePath) < 1)
    {
        szSize[0] = TEXT('\0');
    }

#if WINVER >= WINVER_XP && defined(UNICODE)
    if (bStyle)
    {
        /* Compare with current values to avoid unnecessary flashing if the
         * style will not change
         */
        if (!IsCurrentVisualStyle(mssPath, szColor, szSize))
        {
            SetVisualStyle(mssPath, szColor, szSize);

            /* Message sent by Windows when changing themes. */
            SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE,
                0L, (LPARAM)TEXT("WindowsThemeElement"));
        }
    }
    else
#endif
    if (pSchemeData)
        ApplyClassicScheme(szColor, szSize, pSchemeData);

#if WINVER >= WINVER_VISTA
    UINT value = GetPrivateProfileInt(szStyleKey, szDwmValue,
        UNSET_VALUE, themePath);
    if (value != UNSET_VALUE)
        EnableDwm(value > 0);
#endif

    return ret;
}

#if WINVER >= WINVER_10
_Success_(return)
static
BOOL ApplyImmersiveSettings(_In_ const TCHAR *themePath)
{
    UINT value = GetPrivateProfileInt(szStyleKey, szTransparencyValue,
        UNSET_VALUE, themePath);
    DWORD transparency = (DWORD)value;
    if (value != UNSET_VALUE && value > 1)
        transparency = 1;

    TCHAR szColorMode[6];

    DWORD lightSystemMode = UNSET_VALUE;
    DWORD len = GetPrivateProfileString(szStyleKey, szSystemModeValue, NULL,
        szColorMode, _countof(szColorMode), themePath);
    if (len >= 4)
    {
        if (lstrcmpi(szColorMode, szLightMode) == 0)
            lightSystemMode = 1;
        else if (lstrcmpi(szColorMode, szDarkMode) == 0)
            lightSystemMode = 0;
    }

    DWORD lightAppMode = UNSET_VALUE;
    len = GetPrivateProfileString(szStyleKey, szAppModeValue, NULL,
        szColorMode, _countof(szColorMode), themePath);
    if (len >= 4)
    {
        if (lstrcmpi(szColorMode, szLightMode) == 0)
            lightAppMode = 1;
        else if (lstrcmpi(szColorMode, szDarkMode) == 0)
            lightAppMode = 0;
    }

    if (transparency == UNSET_VALUE &&
        lightSystemMode == UNSET_VALUE &&
        lightAppMode == UNSET_VALUE)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegCreateKeyEx(HKEY_CURRENT_USER, szPersonalizeRegKey,
        0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, NULL);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL changed = FALSE;

    if (transparency != UNSET_VALUE)
    {
        changed |= RegSetValueIfDifferent(hKey, szTransparencyRegValue,
            REG_DWORD, (BYTE *)&transparency, sizeof(DWORD));
    }

    if (lightSystemMode != UNSET_VALUE)
    {
        changed |= RegSetValueIfDifferent(hKey, szSystemLightModeRegValue,
            REG_DWORD, (BYTE *)&lightSystemMode, sizeof(DWORD));
    }

    if (lightAppMode != UNSET_VALUE)
    {
        changed |= RegSetValueIfDifferent(hKey, szAppLightModeRegValue,
            REG_DWORD, (BYTE *)&lightAppMode, sizeof(DWORD));
    }

    RegCloseKey(hKey);

    if (changed)
    {
        SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE,
            0L, (LPARAM)TEXT("ImmersiveColorSet"));
    }

    return changed;
}
#endif

static
COLORREF InvertColor(COLORREF crOrig)
{
    return RGB(
        0xFF - GetRValue(crOrig),
        0xFF - GetGValue(crOrig),
        0xFF - GetBValue(crOrig));
}

_Success_(return)
static
BOOL ReadThemeColors(_In_ const TCHAR *themePath, _In_ const TCHAR *keyName,
    _Out_writes_(NUM_COLORS) COLORREF *pColors, BOOL bWinIni)
{
    TCHAR szColorData[(3 + 1) * 3];
    BOOL bReadColor = FALSE;

    for (int iColor = 0; iColor < NUM_COLORS; iColor++)
    {
        if (GetPrivateProfileString(keyName, g_colorNames[iColor], NULL,
            szColorData, _countof(szColorData), themePath) >= 2 * 3 - 1)
        {
            COLORREF color = ColorStringToColor(szColorData);
            if (color != CLR_INVALID)
            {
                pColors[iColor] = color;
                bReadColor = TRUE;
                continue;
            }
        }

        if (!bWinIni)
        {
            /* No color specified; keep current one */
            continue;
        }

        /* WIN.INI unsupported color */

        switch (iColor)
        {
        /* From 1:
         * COLOR_SCROLLBAR
         * COLOR_DESKTOP
         * COLOR_ACTIVECAPTION
         * COLOR_INACTIVECAPTION
         * COLOR_MENU
         * COLOR_WINDOW
         * COLOR_WINDOWFRAME
         * COLOR_MENUTEXT
         * COLOR_WINDOWTEXT
         * COLOR_CAPTIONTEXT
         */

#define CaseColor(_iColor, _color) \
    case _iColor: \
        pColors[iColor] = _color; \
        bReadColor = TRUE; \
        break

        /* Introduced in 2; use 2 default colors */
        CaseColor(COLOR_ACTIVEBORDER, RGB(127, 127, 127));
        CaseColor(COLOR_INACTIVEBORDER, RGB(127, 127, 127));
        CaseColor(COLOR_APPWORKSPACE, RGB(0, 128, 255));

        /* Introduced in 3 */
        CaseColor(COLOR_HIGHLIGHT, InvertColor(pColors[COLOR_WINDOW]));
        CaseColor(COLOR_HIGHLIGHTTEXT, InvertColor(pColors[COLOR_WINDOWTEXT]));
        CaseColor(COLOR_3DFACE, pColors[COLOR_WINDOW]);
        CaseColor(COLOR_3DSHADOW, RGB(128, 128, 128));  /* 3 default */
        CaseColor(COLOR_GRAYTEXT, RGB(128, 128, 128));  /* 3 16-bit default */
        CaseColor(COLOR_BTNTEXT, pColors[COLOR_WINDOWTEXT]);

        /* Make it readable, since on Windows 1/2 it always displays the caption
         * bar text over a black rectangle.
         */
        CaseColor(COLOR_INACTIVECAPTIONTEXT,
            InvertColor(pColors[COLOR_INACTIVECAPTION]));

        CaseColor(COLOR_3DHILIGHT, RGB(255, 255, 255));  /* 3 default */

        /* Colors introduced in 95 are handled by SetWin3Style() */

#undef CaseColor
        }

        bReadColor = TRUE;
    }

    return bReadColor;
}

/* Returns TRUE if the file seems like a Windows 1 - 3 WIN.INI file. */
_Success_(return)
static
BOOL ReadWinIniColors(_In_ const TCHAR *iniPath,
    _Out_writes_(NUM_COLORS) COLORREF *pColors, BOOL *pisWin3)
{
    static const TCHAR szWinIniColorsValue[] = TEXT("Colors");
    TCHAR szColor[(3 + 1) * 3];

    if (GetPrivateProfileString(szWinIniColorsValue,
        g_colorNames[COLOR_DESKTOP],
        NULL, szColor, _countof(szColor), iniPath) < 2 * 3 - 1)
    {
        return FALSE;
    }

    ReadThemeColors(iniPath, szWinIniColorsValue, pColors, TRUE);

    TCHAR szColorData[(3 + 1) * 3];
    *pisWin3 = (GetPrivateProfileString(
        szWinIniColorsValue, g_colorNames[COLOR_HIGHLIGHT],
        NULL, szColorData, _countof(szColorData), iniPath) >= 2 * 3 - 1);

    return TRUE;
}

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_XP
static
void ReadFlatMenuSetting(_In_ const TCHAR *themePath, _Inout_ BOOL *pbFlatMenus)
{
    UINT value = GetPrivateProfileInt(
        szClassicStyleKey, szFlatMenusValue, *pbFlatMenus, themePath);

    *pbFlatMenus = (value > 0);
}
#endif

_Success_(return)
static
BOOL ReadThemeNcMetrics(
    _In_ const TCHAR *themePath, _Out_ NONCLIENTMETRICS *pNcMetrics)
{
    /* The metrics are always stored in ANSI format, and they do not include
     * the window padded border size, even on Vista/7.
     */
    const UINT cbNcMetricsSize = sizeof(UINT) +
        sizeof(int) * 9 + sizeof(LOGFONTA) * 5;
    const DWORD stringSize = (3 + 1) * cbNcMetricsSize;

    /* Allocate the string in the heap to avoid the CRT */
    TCHAR *szNcMetrics = Alloc(0, stringSize * sizeof(TCHAR));
    if (!szNcMetrics)
        return FALSE;

    if (GetPrivateProfileString(szMetricsKey, szNcMetricsValue, NULL,
        szNcMetrics, stringSize, themePath) < 2 * cbNcMetricsSize - 1)
    {
        Free(szNcMetrics);
        return FALSE;
    }

    NONCLIENTMETRICSA ncMetricsA;
    DWORD copiedBytes = StringDataToBytes(szNcMetrics, (BYTE *)&ncMetricsA,
        cbNcMetricsSize);
    Free(szNcMetrics);
    szNcMetrics = NULL;

    if (copiedBytes != cbNcMetricsSize)
        return FALSE;

#if defined(UNICODE)
    /* Copy from NONCLIENTMETRICSA to NONCLIENTMETRICSW */

#define CopySize(iMetric) \
    pNcMetrics->iMetric = ncMetricsA.iMetric
#define CopyLf(iMetric) \
    memcpy(&pNcMetrics->iMetric, &ncMetricsA.iMetric, \
        sizeof(LOGFONT) - LF_FACESIZE * sizeof(TCHAR)); \
    MultiByteToWideChar(CP_ACP, 0, ncMetricsA.iMetric.lfFaceName, -1, \
        pNcMetrics->iMetric.lfFaceName, LF_FACESIZE)

    /* Keep the current cbSize */
    CopySize(iBorderWidth);
    CopySize(iScrollWidth);
    CopySize(iScrollHeight);
    CopySize(iCaptionWidth);
    CopySize(iCaptionHeight);
    CopyLf(lfCaptionFont);
    CopySize(iSmCaptionWidth);
    CopySize(iSmCaptionHeight);
    CopyLf(lfSmCaptionFont);
    CopySize(iMenuWidth);
    CopySize(iMenuHeight);
    CopyLf(lfMenuFont);
    CopyLf(lfStatusFont);
    CopyLf(lfMessageFont);
    /* Keep the current iPaddedBorderWidth */

#undef CopySize
#undef CopyLf

#else  /* !defined(UNICODE) */
    memcpy(pNcMetrics, &ncMetricsA, cbNcMetricsSize);
#endif

    return TRUE;
}

/* From ICONMETRICSA, only the font is used.
 * The other values (spacing and wrapping) are stored but not read by system
 * applets, so I do not either, to avoid applying huge spacing with system
 * themes.
 * The wrapping setting seems to be a left-over from Windows 3.
 */
_Success_(return)
static
BOOL ReadThemeIconLogFont(_In_ const TCHAR *themePath, _Out_ LOGFONT *plfIcon)
{
#define stringSize (3 + 1) * sizeof(ICONMETRICSA)

    TCHAR *szIconMetrics = Alloc(0, stringSize * sizeof(TCHAR));
    if (!szIconMetrics)
        return FALSE;

    if (GetPrivateProfileString(szMetricsKey, szIconMetricsValue, NULL,
        szIconMetrics, stringSize, themePath) < 2 * sizeof(ICONMETRICSA) - 1)
    {
        Free(szIconMetrics);
        return FALSE;
    }
#undef stringSize

    ICONMETRICSA iconMetricsA;
    DWORD copiedBytes = StringDataToBytes(szIconMetrics, (BYTE *)&iconMetricsA,
        sizeof(ICONMETRICSA));
    Free(szIconMetrics);

    if (copiedBytes != sizeof(ICONMETRICSA))
        return FALSE;

#if defined(UNICODE)
    /* Copy from LOGFONTA to LOGFONTW */
    memcpy(plfIcon, &iconMetricsA.lfFont,
        sizeof(LOGFONT) - LF_FACESIZE * sizeof(TCHAR));
    MultiByteToWideChar(CP_ACP, 0, iconMetricsA.lfFont.lfFaceName, -1,
        plfIcon->lfFaceName, LF_FACESIZE);
#else
    memcpy(plfIcon, &iconMetricsA.lfFont, sizeof(LOGFONT));
#endif

    return TRUE;
}

#if WINVER >= WINVER_2K
static
BOOL ReadThemeHighContrast(_In_ const TCHAR *themePath)
{
    UINT value = GetPrivateProfileInt(
        szStyleKey, szHighContrastValue, 0, themePath);

    return value > 0;
}

#if 0
_Success_(return)
static
BOOL SetLastNonHighContrastTheme(_In_ const TCHAR *themePath)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        TEXT("Microsoft\\Windows\\CurrentVersion\\Themes\\HighContrast"), 0,
        KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, TEXT("Pre-High Contrast Scheme"), 0, REG_SZ,
        (BYTE *)themePath, (lstrlen(themePath) + 1) * sizeof(TCHAR));

    return status == ERROR_SUCCESS;
}
#endif

_Success_(return)
static
BOOL SetLastHighContrastTheme(_In_ const TCHAR *themePath)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szThemesKey, 0,
        KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, TEXT("LastHighContrastTheme"), 0, REG_SZ,
        (BYTE *)themePath, (lstrlen(themePath) + 1) * sizeof(TCHAR));

    return status == ERROR_SUCCESS;
}

_Success_(return)
static
BOOL ApplyThemeHighContrast(_In_ const TCHAR *themePath)
{
    BOOL isHc = ReadThemeHighContrast(themePath);

    HIGHCONTRAST hc;
    hc.cbSize = sizeof(HIGHCONTRAST);

    if (!SystemParametersInfo(
        SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0))
    {
        return FALSE;
    }

    if (isHc)
    {
        SetLastHighContrastTheme(themePath);
        hc.dwFlags |= HCF_HIGHCONTRASTON;
    }
    else
    {
        if ((hc.dwFlags & HCF_HIGHCONTRASTON) == 0)
        {
            /* Keep disabled */
            return TRUE;
        }

        /* This does not work as it should; the the default theme is applied */
#if 0
        SetLastNonHighContrastTheme(themePath);
#endif
        hc.dwFlags &= ~HCF_HIGHCONTRASTON;
    }

    BOOL ret = SystemParametersInfo(SPI_SETHIGHCONTRAST, sizeof(HIGHCONTRAST),
        &hc, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

    /* Wait a bit that the theme finishes applying */
    Sleep(2000);

    return ret;
}
#endif  /* WINVER >= WINVER_2K */

#if WINVER >= WINVER_7
/* Returns if a different color should be applied */
static
BOOL ReadThemeDwmColor(
    _In_ const TCHAR *themePath, _Out_ COLORREF *color)
{
    TCHAR szDwmColor[11];
    if (GetPrivateProfileString(szStyleKey, szDwmColorValue, NULL,
        szDwmColor, _countof(szDwmColor), themePath) !=
        _countof(szDwmColor) - 1)
    {
       return FALSE;
    }

    unsigned long dwmColor = ttoul(szDwmColor, NULL);
    *color = BGR2RGB(dwmColor);

    return *color != GetAccentColor();
}
#endif

#if WINVER >= WINVER_8
static
char ReadThemeDwmAutoColor(_In_ const TCHAR *themePath)
{
    UINT value = GetPrivateProfileInt(
        szStyleKey, szDwmAutoColorValue, UNSET_VALUE, themePath);
    if (value == UNSET_VALUE)
    {
        /* Do not change */
        return -1;
    }

    if (value > 0)
        return 1;
    return 0;
}
#endif

/* Returns the Windows 8 style number, or -1 if no valid settings were found. */
_Success_(return >= 0)
static
char ReadThemeLegacyBackgroundStyle(_In_ const TCHAR *themePath)
{
    UINT value = GetPrivateProfileInt(szDesktopKey, szTileValue,
        UNSET_VALUE, themePath);
    if (value > 0 && value != UNSET_VALUE)
        return 1;  /* Tile */

    value = GetPrivateProfileInt(
        szDesktopKey, szWallpaperStyleValue, UNSET_VALUE, themePath);
    switch (value)
    {
    case BG_POSREG_CENTER:
        return BG_POS_CENTER;
    case BG_POSREG_STRETCH:
        return BG_POS_STRETCH;
    case BG_POSREG_FIT:
        return BG_POS_FIT;
    case BG_POSREG_CROP:
        return BG_POS_CROP;
    case BG_POSREG_SPAN:
        return BG_POS_SPAN;
    }

    /* Plus! JPEG transcoding setting */
    value = GetPrivateProfileInt(szMtsKey, TEXT("Stretch"),
        UNSET_VALUE, themePath);
    if (value == UNSET_VALUE)
        return -1;
    if (value > 0)
        return BG_POS_STRETCH;
    return BG_POS_CENTER;
}

_Success_(return >= 0)
static
char ReadWinIniTiled(_In_ const TCHAR *iniPath)
{
    UINT value = GetPrivateProfileInt(
        szWinIniDesktopSection, szTileValue, 1, iniPath);

    if (value > 0)
        return BG_POS_TILE;
    else
        return BG_POS_CENTER;
}


/* Windows is inconsistent: it unsets the wallpaper if the value is unset or
 * (None), but keeps it if the path is invalid.
 * I unset it in the latter case (by passing a null string).
 */
_Success_(return >= 0)
static
char ReadThemeBackgroundStyle(_In_ const TCHAR *themePath)
{
    UINT value = GetPrivateProfileInt(
        szDesktopKey, szPicturePositionValue, UNSET_VALUE, themePath);
    if (value <= 5)
        return (char)value;
    else
    {
        /* No valid PicturePosition setting, try the legacy values */
        return ReadThemeLegacyBackgroundStyle(themePath);
    }
}

_Success_(return)
static
BOOL ApplyWinIniWallpaper(_In_ const TCHAR *iniPath)
{
    TCHAR imagePath[MAX_PATH];
    if (!ReadThemePath(iniPath, szWinIniDesktopSection, szWallpaperValue,
        imagePath))
    {
        imagePath[0] = TEXT('\0');
    }

    return ApplyWallpaper(imagePath);
}

_Success_(return)
static
BOOL ApplyThemeWallpapers(_In_ const TCHAR *themePath)
{
    TCHAR imagePath[MAX_PATH];
    if (!ReadThemeExpandedPath(themePath, szDesktopKey, szWallpaperValue,
        imagePath, NULL, NULL))
    {
        imagePath[0] = TEXT('\0');
    }

    BOOL ret = ApplyWallpaper(imagePath);

#if WINVER >= WINVER_8 && defined(UNICODE)
    UINT value = GetPrivateProfileInt(szDesktopKey, szMultimonBgValue, 0,
        themePath);
    if (value < 1)
        return ret;

    UINT screenCount = GetScreenCount();
    if (screenCount > 98)
        screenCount = 98;

    TCHAR szWallpaperKey[12];
    for (UINT iScreen = 0; iScreen < screenCount; iScreen++)
    {
        if (wsprintf(szWallpaperKey, TEXT("Wallpaper%u"), iScreen + 1) < 10)
        {
            ret = FALSE;
            continue;
        }

        if (!ReadThemeExpandedPath(themePath, szDesktopKey, szWallpaperKey,
            imagePath, NULL, NULL))
        {
            continue;
        }

        ret &= ApplyScreenWallpaper(imagePath, iScreen);
    }
#endif

    return ret;
}

#if WINVER >= WINVER_8 && defined(UNICODE)
_Success_(*nPaths > 0)
static
void ReadThemeSlideshowPaths(_In_ const TCHAR *themePath,
    _Out_writes_(*nPaths) TCHAR ***paths, _Out_ UINT *nPaths)
{
    TCHAR path[MAX_PATH];
    TCHAR valueName[14];

    *paths = NULL;  /* Silence MinGW warning */
    *nPaths = 0;

    while (*nPaths < 1000)
    {
        if (wsprintf(valueName, TEXT("Item%uPath"), *nPaths) < 9)
            break;

        if (!ReadThemeExpandedPath(themePath,
            szSlideshowKey, valueName, path, NULL, NULL))
        {
            break;
        }

        (*nPaths)++;
        if ((*nPaths) == 1)
        {
            *paths = Alloc(0, sizeof(TCHAR **));
            if (!*paths)
                return;
        }
        else
        {
            TCHAR **newPaths = ReAlloc(0, *paths, sizeof(TCHAR **) * *nPaths);
            if (!newPaths)
            {
                (*nPaths)--;
                break;
            }
            *paths = newPaths;
        }

        (*paths)[*nPaths - 1] = tcsdup(path);
        if (!(*paths)[*nPaths - 1])
        {
            (*nPaths)--;
            TCHAR **newPaths = ReAlloc(0, *paths, sizeof(TCHAR **) * *nPaths);
            if (newPaths)
                *paths = newPaths;
        }
    }

    if (*nPaths > 0)
    {
        /* Individual image paths */
        return;
    }

    /* Check for folder path */
    if (!ReadThemeExpandedPath(themePath,
        szSlideshowKey, szSlideshowDirValue, path, NULL, NULL))
    {
        return;
    }

    *paths = Alloc(0, sizeof(TCHAR **));
    if (!*paths)
        return;

    (*paths)[0] = tcsdup(path);
    if (!(*paths)[0])
    {
        Free(*paths);
        *paths = NULL;
        *nPaths = 0;
    }

    *nPaths = 1;
    return;
}

_Success_(return)
static
BOOL ApplyThemeSlideshow(_In_ const TCHAR *themePath)
{
    UINT interval = GetPrivateProfileInt(
        szSlideshowKey, szSlideshowIntervalValue, 0, themePath);

    /* Windows sets it to 10 seconds even if it was 0.
     * I disable the slideshow instead.
     */
    if (interval == 0)
        return FALSE;

    if (interval < 10000)
        interval = 10000;

    UINT shuffle = GetPrivateProfileInt(
        szSlideshowKey, szSlideshowShuffleValue, 0, themePath);

    if (!ApplySlideshowSettings(interval, shuffle > 0))
        return FALSE;

    TCHAR **paths;
    UINT nPaths;
    BOOL ret = FALSE;

    ReadThemeSlideshowPaths(themePath, &paths, &nPaths);
    if (nPaths > 0)
    {
        ret = ApplySlideshow(paths, nPaths);

        for (UINT nPath = 0; nPath < nPaths; nPath++)
            Free(paths[nPath]);
        Free(paths);
    }

    return ret;
}
#endif  /* WINVER >= WINVER_8 && defined(UNICODE) */

_Success_(return)
static
BOOL ReadThemeIconPathFromSection(
    _In_ const TCHAR *themePath, _In_ const TCHAR *szSection,
    _In_opt_ const TCHAR *szRegValue, _Out_writes_(MAX_PATH) TCHAR *iconPath)
{
    /* I could store the unexpanded path as REG_EXPAND_SZ, but this makes it
     * easier to expand %ResourceDir%.
     */

    TCHAR realIconPath[MAX_PATH - 2];
    BOOL bPathFound;
    int resourceId;
    if (szRegValue)
    {
        bPathFound = ReadThemeExpandedPath(
            themePath, szSection, szRegValue, iconPath,
            realIconPath, &resourceId);
    }
    else
    {
        bPathFound = ReadThemeExpandedPath(
            themePath, szSection, szDefaultValue, iconPath,
            realIconPath, &resourceId);
    }

    if (!bPathFound)
        return FALSE;

    /* Check that the icon with the found id really exists */
    HICON hIcon = NULL;
    UINT nIcons = ExtractIconEx(realIconPath, resourceId, &hIcon, NULL, 1);
    if (!hIcon)
        return FALSE;

    DestroyIcon(hIcon);
    return nIcons == 1;
}

_Success_(return)
static
BOOL ReadThemeIconPathFromUuid(
    _In_ const TCHAR *themePath, _In_ const TCHAR *szIconUuid,
    _In_opt_ const TCHAR *szRegValue, _Out_writes_(MAX_PATH) TCHAR *iconPath)
{
    TCHAR szSection[74];
    if (wsprintf(szSection, TEXT("CLSID\\{%s}\\DefaultIcon"), szIconUuid) != 56)
        return FALSE;

    if (ReadThemeIconPathFromSection(themePath, szSection, szRegValue,
        iconPath))
    {
        return TRUE;
    }

    if (wsprintf(szSection, TEXT("Software\\Classes\\CLSID\\{%s}\\DefaultIcon"),
        szIconUuid) != 73)
    {
        return FALSE;
    }

    return ReadThemeIconPathFromSection(themePath, szSection, szRegValue,
        iconPath);
}

_Success_(return)
static
BOOL GetDefaultIconPath(_In_ const TCHAR *szIconUuid,
    _In_opt_ const TCHAR *szRegValue, _Out_writes_(MAX_PATH) TCHAR *path)
{
    TCHAR szKey[74];
    if (wsprintf(szKey,
        TEXT("SOFTWARE\\Classes\\CLSID\\{%s}\\DefaultIcon"), szIconUuid) != 73)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        szKey, 0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    DWORD dwType;
    DWORD cbData = MAX_PATH * sizeof(TCHAR);
    status = RegQueryValueEx(hKey, szRegValue, NULL, &dwType,
        (BYTE *)path, &cbData);

    RegCloseKey(hKey);

    if (!(status == ERROR_SUCCESS &&
        (dwType == REG_SZ || dwType == REG_EXPAND_SZ) && cbData > 0))
    {
        return FALSE;
    }

    path[MAX_PATH - 1] = TEXT('\0');
    return TRUE;
}

_Success_(return)
static
BOOL ReadThemeIconPath(_In_ const TCHAR *themePath,
    _In_ const TCHAR *szIconUuid, _In_opt_ const TCHAR *szFallbackUuid,
    _In_opt_ const TCHAR *szRegValue, _Out_writes_(MAX_PATH) TCHAR *iconPath)
{
    if (ReadThemeIconPathFromUuid(
        themePath, szIconUuid, szRegValue, iconPath))
    {
        return TRUE;
    }

    if (szFallbackUuid && ReadThemeIconPathFromUuid(
        themePath, szFallbackUuid, szRegValue, iconPath))
    {
        return TRUE;
    }

    return GetDefaultIconPath(szIconUuid, szRegValue, iconPath);
}

/* Returns if the icon has changed. */
static
BOOL ApplyThemeIcon(_In_ const TCHAR *themePath,
    _In_ const TCHAR *szIconUuid, _In_opt_ const TCHAR *szFallbackUuid)
{
    TCHAR szKey[108];
    if (wsprintf(szKey,
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CLSID\\{%s}\\DefaultIcon"),
        szIconUuid) != 107)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szKey, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR iconPath[MAX_PATH];
    DWORD cbData;
    BOOL changed = FALSE;

    if (lstrcmp(szIconUuid, szRecycleBinIconUuid) == 0)
    {
        /* Recycle Bin icons */

        if (ReadThemeIconPath(themePath, szIconUuid, szFallbackUuid,
            szRecycleBinEmptyValue, iconPath))
        {
            cbData = (lstrlen(iconPath) + 1) * sizeof(TCHAR);

            /* Windows also sets the Empty bin value as the default value */
            RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)iconPath, cbData);

            changed |= RegSetValueIfDifferent(hKey, szRecycleBinEmptyValue,
                REG_SZ, (BYTE *)iconPath, cbData);
        }

        if (ReadThemeIconPath(themePath, szIconUuid, szFallbackUuid,
            szRecycleBinFullValue, iconPath))
        {
            cbData = (lstrlen(iconPath) + 1) * sizeof(TCHAR);
            changed |= RegSetValueIfDifferent(hKey, szRecycleBinFullValue,
                REG_SZ, (BYTE *)iconPath, cbData);
        }
    }
    else
    {
        if (ReadThemeIconPath(themePath, szIconUuid, szFallbackUuid,
            NULL, iconPath))
        {
            cbData = (lstrlen(iconPath) + 1) * sizeof(TCHAR);
            changed |= RegSetValueIfDifferent(hKey, NULL,
                REG_SZ, (BYTE *)iconPath, cbData);
        }
    }

    RegCloseKey(hKey);
    return changed;
}

static
BOOL ApplyThemeDesktopIcons(_In_ const TCHAR *themePath)
{
    BOOL changed = FALSE;

#define Apply(uuid, fallbackUuid) \
    changed |= ApplyThemeIcon(themePath, uuid, fallbackUuid)

    Apply(szComputerIconUuid, NULL);
    Apply(szUserFilesIconUuid, szUserFilesIconUuidOld);
    Apply(szNetworkIconUuid, szNetworkIconUuidOld);
    Apply(szRecycleBinIconUuid, NULL);

#undef Apply

    if (changed)
    {
        /* This should refresh the icons, but it does not... */
        SystemParametersInfo(SPI_SETICONS, 0, NULL, SPIF_SENDCHANGE);

        /* But this does */
        SHChangeNotify(SHCNE_ASSOCCHANGED, 0, NULL, NULL);
    }

    return changed;
}

_Success_(return)
static
BOOL SetPointerSchemeName(_In_ const TCHAR *themePath, HKEY hKey)
{
    TCHAR rawName[MAX_PATH];
    TCHAR displayName[65];

    if (!(GetPrivateProfileString(szPointersKey, TEXT("DefaultValue.MUI"), NULL,
            rawName, _countof(rawName), themePath) > 0 &&
        SHLoadIndirectString(rawName,
            displayName, _countof(displayName), NULL) == S_OK))
    {
        if (GetPrivateProfileString(szPointersKey, szDefaultValue, NULL,
            displayName, _countof(displayName), themePath) < 1)
        {
            return FALSE;
        }
    }

    LSTATUS status = RegSetValueEx(hKey, NULL, 0, REG_SZ,
        (BYTE *)displayName, (lstrlen(displayName) + 1) * sizeof(TCHAR));
    return status == ERROR_SUCCESS;
}

static
BOOL ApplyThemePointers(_In_ const TCHAR *themePath)
{
    /* If there is no "Cursors" section, do not change anything */
    TCHAR szKeys[3];
    if (GetPrivateProfileString(szPointersKey, NULL, NULL,
        szKeys, _countof(szKeys), themePath) < 1)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szPointersKey, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR pointerPath[MAX_PATH];
    BOOL changed = FALSE;
    for (unsigned char iPointer = 0;
        iPointer < _countof(g_pointerNames);
        iPointer++)
    {
        /* If there is no valid path, use the system pointer (unskinned).
         * When unsetting pointers, Windows deletes the values when applying
         * themes, but leaves them empty when doing it from the Mouse applet.
         */
        if (!ReadThemeExpandedPath(themePath,
            szPointersKey, g_pointerNames[iPointer], pointerPath, NULL, NULL))
        {
            pointerPath[0] = TEXT('\0');;
        }

        changed |= RegSetValueIfDifferent(hKey, g_pointerNames[iPointer], REG_SZ,
            (BYTE *)pointerPath, (lstrlen(pointerPath) + 1) * sizeof(TCHAR));
    }

    if (changed && SetPointerSchemeName(themePath, hKey))
    {
        DWORD schemeSource = 2;
        RegSetValueEx(hKey, TEXT("Scheme Source"), 0, REG_DWORD,
            (BYTE *)&schemeSource, sizeof(DWORD));
    }

    RegCloseKey(hKey);

    if (changed)
        SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);

    return changed;
}

BOOL ApplyWinIniSounds(_In_ const TCHAR *iniPath);
BOOL ApplyThemeSounds(_In_ const TCHAR *themePath);

_Success_(return)
static
BOOL ApplyThemeScreenSaver(_In_ const TCHAR *themePath)
{
    TCHAR ssPath[MAX_PATH];

    if (!ReadThemeExpandedPath(themePath,
        szScreenSaverSection, szScreenSaverValue, ssPath, NULL, NULL))
    {
        ssPath[0] = TEXT('\0');
    }

    return ApplyScreenSaver(ssPath);
}

_Success_(return)
BOOL ApplyTheme(_In_ const TCHAR *filePath)
{
    ReadThemeChangesAllowed();

    SCHEME_DATA schemeData;
    /* Load defaults from the current scheme */
    LoadCurrentScheme(&schemeData);

    BOOL isWin3;
    BOOL isWinIni = ReadWinIniColors(filePath, schemeData.colors, &isWin3);

    if (!isWinIni)
    {
        /* System .Theme file.
         * All system theme files should have a "MTSM" value:
         * "DABJDKT" up to Windows 7
         * "RJSPBS" from Windows 8
         */
        TCHAR szMtsm[8];
        if (GetPrivateProfileString(szMtsKey, szMtsValue, NULL,
            szMtsm, _countof(szMtsm), filePath) < 6)
        {
            return FALSE;
        }

        SetEnvironmentVariables();
    }

    if (g_themeChangesAllowed.style)
    {
        if (isWinIni)
        {
            SetWin3Style(&schemeData);
            if (!isWin3)
            {
                schemeData.colors[COLOR_MENUHILIGHT] =
                    InvertColor(schemeData.colors[COLOR_MENUBAR]);
            }
        }
        else
        {
#if WINVER >= WINVER_XP
            LoadUxThemeFunctions();
#endif

#if WINVER >= WINVER_2K
            ApplyThemeHighContrast(filePath);
#endif
            ApplyThemeStyle(filePath, &schemeData);

            ReadThemeColors(filePath, szColorsKey, schemeData.colors, FALSE);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#if WINVER >= WINVER_2K
            if ((schemeData.colors[COLOR_ACTIVECAPTION] !=
                    schemeData.colors[COLOR_GRADIENTACTIVECAPTION]) ||
                (schemeData.colors[COLOR_INACTIVECAPTION] !=
                    schemeData.colors[COLOR_GRADIENTINACTIVECAPTION]))
            {
                schemeData.bGradientCaptions = TRUE;
            }
#endif

#if WINVER >= WINVER_XP
            ReadFlatMenuSetting(filePath, &schemeData.bFlatMenus);
#endif
#endif

#if WINVER >= WINVER_7
            COLORREF color;
            if (ReadThemeDwmColor(filePath, &color))
                SetAccentColor(color);
#endif

#if WINVER >= WINVER_10
            ApplyImmersiveSettings(filePath);
#endif

#if WINVER >= WINVER_XP
            UnloadUxThemeFunctions();
#endif
        }  /* !isWinIni */
    }

    if (g_themeChangesAllowed.metrics)
    {
        if (isWinIni)
            SetWin16Metrics(&schemeData);
        else
        {
            ReadThemeNcMetrics(filePath, &schemeData.ncMetrics);
            ReadThemeIconLogFont(filePath, &schemeData.lfIconFont);
        }
    }

    if (g_themeChangesAllowed.style ||
        g_themeChangesAllowed.metrics)
    {
        ApplyScheme(&schemeData, NULL);
    }

    if (g_themeChangesAllowed.background)
    {
#if WINVER >= WINVER_8 && defined(UNICODE)
        BOOL bComInitialized = CreateIWallpaperInstance();
#endif

        TCHAR szPattern[(3 + 1) * 8];
        if (GetPrivateProfileString(
            (isWinIni ? szWinIniDesktopSection : szDesktopKey),
            szPatternValue, NULL, szPattern, _countof(szPattern), filePath) <
            2 * 8 - 1)
        {
            szPattern[0] = TEXT('\0');
        }

        /* The background position is only changed if it is found in the theme
         * file.
         * This should be applied before applying the wallpaper to update the
         * automatic color, if enabled.
         */
        if (isWinIni)
        {
            ApplyBackgroundSettings(szPattern, ReadWinIniTiled(filePath), -1);
            ApplyWinIniWallpaper(filePath);
        }
        else
        {
            ApplyBackgroundSettings(szPattern,
                ReadThemeBackgroundStyle(filePath)
    #if WINVER >= WINVER_8
                , ReadThemeDwmAutoColor(filePath)
    #endif
            );

            ApplyThemeWallpapers(filePath);
        }

#if WINVER >= WINVER_8 && defined(UNICODE)
        if (!isWinIni)
            ApplyThemeSlideshow(filePath);

        if (bComInitialized)
            CoUninitialize();
#endif
    }

    if (!isWinIni && g_themeChangesAllowed.icons)
        ApplyThemeDesktopIcons(filePath);

    if (!isWinIni && g_themeChangesAllowed.pointers)
        ApplyThemePointers(filePath);

    if (g_themeChangesAllowed.sounds)
    {
        if (isWinIni)
            ApplyWinIniSounds(filePath);
        else
            ApplyThemeSounds(filePath);
    }

    if (!isWinIni && g_themeChangesAllowed.screenSaver)
        ApplyThemeScreenSaver(filePath);

    return TRUE;
}

#undef UNSET_VALUE

#endif  /* defined(WITH_THEMES) */
