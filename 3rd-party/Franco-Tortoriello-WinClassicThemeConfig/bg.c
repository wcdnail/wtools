/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Desktop background pattern / image (wallpaper) functionality.
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES)

#include "bg.h"
#include "reg.h"
#include "mincrt.h"
#include "util.h"

const TCHAR szDesktopKey[] =
    TEXT("Control Panel\\Desktop");

const TCHAR szPatternValue[] =
    TEXT("Pattern");

#if defined(WITH_THEMES)
const TCHAR szTileValue[] =
    TEXT("TileWallpaper");

const TCHAR szWallpaperStyleValue[] =
    TEXT("WallpaperStyle");

const TCHAR szWallpaperValue[] =
    TEXT("Wallpaper");
#endif

#if WINVER >= WINVER_8 && defined(UNICODE) && defined(WITH_THEMES)
static IDesktopWallpaper *pWallpaperInst = NULL;
#endif

#if defined(WITH_THEMES)

_Success_(return)
static
BOOL ApplyBgStyle(HKEY hKey, char bgStyle)
{
    unsigned char tiled;
    unsigned char style;

    switch (bgStyle)
    {
    case BG_POS_CENTER:
        tiled = 0;
        style = BG_POSREG_CENTER;
        break;

    case BG_POS_TILE:
        tiled = 1;
        style = BG_POSREG_CROP;  /* Unused */
        break;

    case BG_POS_STRETCH:
        tiled = 0;
        style = BG_POSREG_STRETCH;
        break;

    case BG_POS_CROP:
        tiled = 0;
        style = BG_POSREG_CROP;
        break;

    case BG_POS_FIT:
        tiled = 0;
        style = BG_POSREG_FIT;
        break;

    case BG_POS_SPAN:
        tiled = 0;
        style = BG_POSREG_SPAN;
        break;

    default:
        return FALSE;
    }

    TCHAR szValue[3];
    LSTATUS status;
    BOOL ret = TRUE;
    if (wsprintf(szValue, TEXT("%u"), tiled) >= 1)
    {
        status = RegSetValueEx(hKey, szTileValue, 0, REG_SZ,
            (BYTE *)szValue, (lstrlen(szValue) + 1) * sizeof(TCHAR));
        ret &= (status == ERROR_SUCCESS);
    }
    else
        ret = FALSE;

    if (tiled > 0)
        return ret;

    if (wsprintf(szValue, TEXT("%u"), style) >= 1)
    {
        status = RegSetValueEx(hKey, szWallpaperStyleValue, 0, REG_SZ,
            (BYTE *)szValue, (lstrlen(szValue) + 1) * sizeof(TCHAR));
        ret &= (status == ERROR_SUCCESS);
    }
    else
        ret = FALSE;

    return ret;
}

#if WINVER >= WINVER_8 && defined(UNICODE)
_Success_(return)
static
BOOL PathsToItemArray(
    _In_reads_(nPaths) TCHAR **paths, UINT nPaths, _Out_ IShellItemArray **ppsia)
{
    PIDLIST_ABSOLUTE pidl;
    PCIDLIST_ABSOLUTE_ARRAY pidls = NULL;
    HRESULT hr;
    UINT nItems = 0;

    for (UINT nPath = 0; nPath < nPaths; nPath++)
    {
        hr = SHParseDisplayName(paths[nPath], NULL, &pidl, 0L, NULL);
        if (hr != S_OK)
            continue;

        nItems++;
        if (nItems == 1)
        {
            pidls = Alloc(0, sizeof(PIDLIST_ABSOLUTE));
            if (!pidls)
                return FALSE;
        }
        else
        {
            PCIDLIST_ABSOLUTE_ARRAY newPidls = ReAlloc(
                0, pidls, sizeof(PIDLIST_ABSOLUTE) * nItems);
            if (!newPidls)
            {
                nItems--;
                break;
            }
            pidls = newPidls;
        }

        pidls[nItems - 1] = pidl;
    }

    if (!pidls)
        return FALSE;

    hr = SHCreateShellItemArrayFromIDLists(nItems, pidls, ppsia);

    Free(pidls);
    return hr == S_OK;
}
#endif  /* WINVER >= WINVER_8 && defined(UNICODE) */

#endif  /* defined(WITH_THEMES) */

_Success_(return != NULL)
BYTE *LoadCurrentDesktopPattern(void)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szDesktopKey, 0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return NULL;

    TCHAR szPattern[(3 + 1) * 8];
    DWORD cbData = sizeof(szPattern);
    status = RegQueryString(hKey, szPatternValue, szPattern, &cbData);
    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS)
        return NULL;

    BYTE *pattern = Alloc(0, 8 * sizeof(BYTE));
    if (!pattern)
        return NULL;

    if (StringDataToBytes(szPattern, pattern, 8) != 8)
    {
        Free(pattern);
        return NULL;
    }

    return pattern;
}

#if defined(WITH_THEMES)

/* Parameters:
 * - szPattern: String representation of the background pattern (8x8 bitmap)
 * - bgStyle:   Values based on Windows 8 .Theme files.
 *              Invalid values keep the current registry settings.
 * - autoColor: Windows 8 and later. Determines if the DWM color is based on
 *              the wallpaper colors.
 *              Invalid values keep the current registry settings.
 */
_Success_(return)
BOOL ApplyBackgroundSettings(_In_ const TCHAR *szPattern, char bgStyle
#if WINVER >= WINVER_8
    , char autoColor
#endif
)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szDesktopKey,
        0, KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL ret = TRUE;

    status = RegSetValueEx(hKey, szPatternValue, 0, REG_SZ,
        (BYTE *)szPattern, (lstrlen(szPattern) + 1) * sizeof(TCHAR));
    if (status == ERROR_SUCCESS)
        ret &= SystemParametersInfo(SPI_SETDESKPATTERN, 0, NULL, 0);
    else
        ret = FALSE;

    ret &= ApplyBgStyle(hKey, bgStyle);

    if (autoColor == 0 || autoColor == 1)
    {
        DWORD dwValue = autoColor;
        status = RegSetValueEx(hKey, TEXT("AutoColorization"), 0, REG_DWORD,
            (BYTE *)&dwValue, sizeof(DWORD));
        ret &= (status == ERROR_SUCCESS);
    }

    RegCloseKey(hKey);
    return ret;
}

#if WINVER >= WINVER_8 && defined(UNICODE)
/* Returns if CoInitializeEx() succeeded */
_Success_(return)
BOOL CreateIWallpaperInstance(void)
{
    if (pWallpaperInst)
    {
        /* Already created */
        return FALSE;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (hr != S_OK)
        return FALSE;

    hr = CoCreateInstance(&CLSID_DesktopWallpaper, NULL,
        CLSCTX_ALL, &IID_IDesktopWallpaper, (void **)&pWallpaperInst);

    return hr == S_OK;
}
#endif

_Success_(return)
BOOL ApplyWallpaper(_In_ TCHAR *path)
{
#if WINVER >= WINVER_8 && defined(UNICODE)
    if (!pWallpaperInst)
#endif
    {
        return SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, path,
            SPIF_UPDATEINIFILE);
    }

#if WINVER >= WINVER_8 && defined(UNICODE)
    /* Setting the wallpaper with this API changes it smoothly, unlike when
     * using SystemParametersInfo(), which shows the wallpaper immediately
     * and then abruptly shows one slideshow wallpaper and then another one.
     */
    HRESULT hr = pWallpaperInst->lpVtbl->SetWallpaper(pWallpaperInst,
        NULL, path);
    return hr == S_OK;
#endif
}

#if WINVER >= WINVER_8 && defined(UNICODE)
_Success_(return > 0)
UINT GetScreenCount(void)
{
    if (!pWallpaperInst)
        return 0;

    UINT nScreens;
    HRESULT hr = pWallpaperInst->lpVtbl->GetMonitorDevicePathCount(
        pWallpaperInst, &nScreens);
    if (hr != S_OK)
        return 0;

    return nScreens;
}

_Success_(return)
BOOL ApplyScreenWallpaper(_In_ const TCHAR *path, UINT screenIndex)
{
    if (!pWallpaperInst)
        return FALSE;

    WCHAR *szScreenId;

    HRESULT hr = pWallpaperInst->lpVtbl->GetMonitorDevicePathAt(pWallpaperInst,
        screenIndex, &szScreenId);
    if (hr != S_OK)
        return FALSE;

    hr = pWallpaperInst->lpVtbl->SetWallpaper(pWallpaperInst, szScreenId, path);
    return hr == S_OK;
}

_Success_(return)
BOOL ApplySlideshowSettings(UINT intervalMilliseconds, BOOL bShuffle)
{
    if (!pWallpaperInst)
        return FALSE;

    HRESULT hr = pWallpaperInst->lpVtbl->SetSlideshowOptions(pWallpaperInst,
        (bShuffle ? DSO_SHUFFLEIMAGES : 0), intervalMilliseconds);
    return hr == S_OK;
}

_Success_(return)
BOOL ApplySlideshow(_In_reads_(nPaths) TCHAR **paths, UINT nPaths)
{
    if (!pWallpaperInst)
        return FALSE;

    IShellItemArray *psia = NULL;
    PathsToItemArray(paths, nPaths, &psia);

    HRESULT hr = pWallpaperInst->lpVtbl->SetSlideshow(pWallpaperInst, psia);
    return hr == S_OK;
}

#endif  /* WINVER >= WINVER_8 && defined(UNICODE) */

#endif  /* defined(WITH_THEMES) */

#endif  /* defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES) */
