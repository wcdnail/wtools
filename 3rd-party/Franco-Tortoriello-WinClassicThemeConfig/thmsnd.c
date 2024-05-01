/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Theme sound scheme handling
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if defined(WITH_THEMES)

#include "reg.h"
#include "util.h"

#include <Shlwapi.h>

#define INTERNALNAME_MAX_SIZE  10
#define DISPLAYNAME_MAX_SIZE   40
#define APPNAME_MAX_SIZE       50
#define EVENTNAME_MAX_SIZE     50

static const TCHAR szSchemesKey[] =
    TEXT("AppEvents\\Schemes");

static const TCHAR szAppsKey[] =
    TEXT("AppEvents\\Schemes\\Apps");

static const TCHAR szShemeNamesKey[] =
    TEXT("AppEvents\\Schemes\\Names");

static const TCHAR szSoundsSection[] =
    TEXT("Sounds");

static const TCHAR szSoundSchemeNameValue[] =
    TEXT("SchemeName");

static const TCHAR szDefaultValue[] =
    TEXT("DefaultValue");

static const TCHAR szDefaultScheme[] =
    TEXT(".Default");

static const TCHAR szCurrentScheme[] =
    TEXT(".Current");

static const TCHAR szModifiedScheme[] =
    TEXT(".Modified");

static const TCHAR szCustomScheme[] =
    TEXT("Custom");

_Success_(return)
BOOL ReadThemeExpandedPath(
    _In_ const TCHAR *themePath,
    _In_ const TCHAR *szSection,
    _In_ const TCHAR *szKey,
    _Out_writes_(MAX_PATH) TCHAR *path,
    _Out_writes_opt_(MAX_PATH - 2) TCHAR *realPath,
    _Out_opt_ int *resourceId);

_Success_(return)
static
BOOL GetEventSound(_In_ const TCHAR *szEventKey, _In_ const TCHAR *schemeName,
    _Out_writes_(MAX_PATH) TCHAR *soundPath)
{
    TCHAR szSchemeKey[_countof(szAppsKey) +
        APPNAME_MAX_SIZE + EVENTNAME_MAX_SIZE + INTERNALNAME_MAX_SIZE - 1];
    if (wsprintf(szSchemeKey, TEXT("%s\\%s"), szEventKey, schemeName) <
        (int)_countof(szAppsKey) + 5)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szSchemeKey, 0, KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    /* There are NULL values in the default values, but RegQueryString() handles
     * that.
     */
    DWORD cbData = MAX_PATH * sizeof(TCHAR);
    status = RegQueryString(hKey, NULL, soundPath, &cbData);
    RegCloseKey(hKey);

    return status == ERROR_SUCCESS;
}

_Success_(return)
static
BOOL ApplyAppSoundEvent(_In_ const TCHAR *appName, _In_ const TCHAR *eventName,
    _In_ const TCHAR *schemeName, _In_ const TCHAR *soundPath)
{
    TCHAR szSchemeKey[_countof(szAppsKey) +
        APPNAME_MAX_SIZE + EVENTNAME_MAX_SIZE + INTERNALNAME_MAX_SIZE - 1];
    if (wsprintf(szSchemeKey, TEXT("%s\\%s\\%s\\%s"),
        szAppsKey, appName, eventName, schemeName) <
            (int)_countof(szAppsKey) + 5)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegCreateKeyEx(HKEY_CURRENT_USER,
        szSchemeKey, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)soundPath,
        (lstrlen(soundPath) + 1) * sizeof(TCHAR));

    RegCloseKey(hKey);
    return status == ERROR_SUCCESS;
}

_Success_(return)
static
BOOL ApplyThemeAppSoundEvent(_In_ const TCHAR *themePath,
    _In_ const TCHAR *appName, _In_ const TCHAR *eventName,
    _In_ const TCHAR *schemeName)
{
    TCHAR szEventKey[_countof(szAppsKey) +
        APPNAME_MAX_SIZE + EVENTNAME_MAX_SIZE];
    if (wsprintf(szEventKey, TEXT("%s\\%s\\%s"),
        szAppsKey, appName, eventName) < (int)_countof(szAppsKey) + 3)
    {
        return FALSE;
    }

    BOOL isCustom = FALSE;
    TCHAR soundPath[MAX_PATH];

    if (ReadThemeExpandedPath(themePath,
        szEventKey, szDefaultValue, soundPath, NULL, NULL))
    {
        isCustom = TRUE;
        goto Apply;
    }

    /* The section could include a "\.Current" suffix */
    TCHAR szEventAltKey[_countof(szEventKey) + 9];
    if (wsprintf(szEventAltKey, TEXT("%s\\%s"), szEventKey, szCurrentScheme) >=
            (int)_countof(szAppsKey) + 12 &&
        ReadThemeExpandedPath(themePath,
            szEventAltKey, szDefaultValue, soundPath, NULL, NULL))
    {
        isCustom = TRUE;
        goto Apply;
    }

    if (schemeName[0] != TEXT('\0') &&
        GetEventSound(szEventKey, schemeName, soundPath))
    {
        goto Apply;
    }

    if (GetEventSound(szEventKey, szDefaultScheme, soundPath))
        goto Apply;

    return FALSE;

Apply:
    if (schemeName[0] == TEXT('\0'))
        ApplyAppSoundEvent(appName, eventName, szCustomScheme, soundPath);
    else
        ApplyAppSoundEvent(appName, eventName, szModifiedScheme, soundPath);

    return ApplyAppSoundEvent(appName, eventName, szCurrentScheme,
        soundPath) && isCustom;
}

_Success_(return)
static
BOOL ApplyThemeAppSoundEvents(_In_ const TCHAR *themePath, HKEY hkApps,
    _In_ const TCHAR *appName, _In_ const TCHAR *schemeName)
{
    HKEY hkApp;
    LSTATUS status = RegOpenKeyEx(hkApps, appName, 0,
        KEY_ENUMERATE_SUB_KEYS, &hkApp);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL ret = FALSE;
    TCHAR eventName[EVENTNAME_MAX_SIZE];
    DWORD cbEventName;
    DWORD iEvent = 0;

    status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        cbEventName = _countof(eventName);
        status = RegEnumKeyEx(hkApp, iEvent, eventName, &cbEventName,
            NULL, NULL, NULL, NULL);
        iEvent++;
        if (status != ERROR_SUCCESS)
            continue;

        ret |= ApplyThemeAppSoundEvent(themePath,
            appName, eventName, schemeName);
    }

    RegCloseKey(hkApp);
    return ret;
}

_Success_(return)
static
BOOL GetSoundSchemeInternalName(_In_ const TCHAR *displayName,
    _Out_writes_(INTERNALNAME_MAX_SIZE) TCHAR *internalName)
{
    HKEY hkNames;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szShemeNamesKey, 0, KEY_ENUMERATE_SUB_KEYS, &hkNames);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL ret = FALSE;
    DWORD cbData;
    DWORD iName = 0;

    TCHAR currentDisplayName[DISPLAYNAME_MAX_SIZE];
    HKEY hkName;

    status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        cbData = INTERNALNAME_MAX_SIZE;
        status = RegEnumKeyEx(hkNames, iName, internalName, &cbData,
            NULL, NULL, NULL, NULL);
        iName++;
        if (status != ERROR_SUCCESS)
            continue;

        if (RegOpenKeyEx(hkNames, internalName, 0, KEY_QUERY_VALUE, &hkName) !=
            ERROR_SUCCESS)
        {
            continue;
        }

        cbData = sizeof(currentDisplayName);
        if (RegQueryString(hkName, NULL, currentDisplayName, &cbData) !=
            ERROR_SUCCESS)
        {
            continue;
        }

        RegCloseKey(hkName);

        if (lstrcmp(currentDisplayName, displayName) == 0)
        {
            ret = TRUE;
            break;
        }
    }

    RegCloseKey(hkNames);

    if (!ret)
        internalName[0] = TEXT('\0');
    return ret;
}

#if defined(UNICODE)
/* Gets the sound scheme name set in the .Theme file */
_Success_(return)
static
BOOL ReadThemeSoundSchemeName(_In_ const TCHAR *themePath,
    _Out_writes_(INTERNALNAME_MAX_SIZE) TCHAR *internalName,
    _Out_writes_(DISPLAYNAME_MAX_SIZE) TCHAR *displayName)
{
    TCHAR rawName[MAX_PATH];
    if (GetPrivateProfileString(szSoundsSection, szSoundSchemeNameValue, NULL,
        rawName, MAX_PATH, themePath) > 0 &&
        SHLoadIndirectString(rawName, displayName, DISPLAYNAME_MAX_SIZE, NULL) == S_OK)
    {
        GetSoundSchemeInternalName(displayName, internalName);
        return TRUE;
    }

    return FALSE;
}
#endif

_Success_(return)
static
BOOL GetCustomSchemeName(_Out_writes_(DISPLAYNAME_MAX_SIZE) TCHAR *displayName)
{
    /* Localized */
    HMODULE hMod = LoadLibraryEx(TEXT("themeui.dll"), NULL,
        LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE |
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hMod)
    {
        int len = LoadString(hMod, 2057, displayName, DISPLAYNAME_MAX_SIZE);
        FreeLibrary(hMod);
        if (len > 0)
            return TRUE;
    }

    /* Hardcoded */
    if (lstrcpy(displayName, szCustomScheme))
        return TRUE;

    return FALSE;
}

/* Returns a translated "displayName (modified)" string. */
/* Gets the sound scheme name set in the .Theme file */
_Success_(return)
static
BOOL GetSchemeNameModified(_Inout_ TCHAR *displayName)
{
    int origLen = lstrlen(displayName);

    /* Limit length to truncate the format string if necessary */
    int len = DISPLAYNAME_MAX_SIZE - origLen + 2;
    if (len < 14)
        return FALSE;

    TCHAR origName[DISPLAYNAME_MAX_SIZE];
    if (!lstrcpy(origName, displayName))
        return FALSE;

    HMODULE hMod = LoadLibraryEx(TEXT("mmsys.cpl"), NULL,
        LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE |
        LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (hMod)
    {
        TCHAR *format = Alloc(0, len * sizeof(TCHAR));
        if (format)
        {
            len = LoadString(hMod, 423, format, len);
            FreeLibrary(hMod);

            if (len > 0)
                len = wsprintf(displayName, format, origName);

            Free(format);
        }
    }
    else
        len = wsprintf(displayName, TEXT("%s (modified)"), origName);

    return len > origLen;
}

_Success_(return)
static
BOOL SetSoundSchemeName(_In_ const TCHAR *name)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szSchemesKey, 0, KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, NULL, 0, REG_SZ,
        (BYTE *)name, (lstrlen(name) + 1) * sizeof(TCHAR));

    RegCloseKey(hKey);
    return status == ERROR_SUCCESS;
}

_Success_(return)
static
BOOL CreateSoundScheme(
    _In_ const TCHAR *internalName, _In_ const TCHAR *displayName)
{
    TCHAR szKey[_countof(szShemeNamesKey) + INTERNALNAME_MAX_SIZE - 1];
    if (wsprintf(szKey, TEXT("%s\\%s"), szShemeNamesKey, internalName) <
        (int)_countof(szShemeNamesKey) + 1)
    {
        return FALSE;
    }

    HKEY hKey;
    LSTATUS status = RegCreateKeyEx(HKEY_CURRENT_USER,
        szKey, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, NULL, 0, REG_SZ,
        (BYTE *)displayName, (lstrlen(displayName) + 1) * sizeof(TCHAR));

    RegCloseKey(hKey);
    return status == ERROR_SUCCESS;
}

/* Applies to the ".Current" and "Custom" sound event schemes all sounds found
 * in the .Theme file, leaving the default system sounds for those not
 * specified.
 * Returns TRUE if any sound was changed from the default.
 * This differs from Windows in that if specific sounds are given and also a
 * scheme name, the sounds are not ignored.
 */
BOOL ApplyThemeSounds(_In_ const TCHAR *themePath)
{
    HKEY hkApps;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER,
        szAppsKey, 0, KEY_ENUMERATE_SUB_KEYS, &hkApps);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR schemeInternalName[DISPLAYNAME_MAX_SIZE];
    TCHAR schemeDisplayName[DISPLAYNAME_MAX_SIZE];
    schemeInternalName[0] = TEXT('\0');
    schemeDisplayName[0] = TEXT('\0');
#if defined(UNICODE)
    ReadThemeSoundSchemeName(themePath, schemeInternalName, schemeDisplayName);
#endif

    BOOL isCustom = FALSE;
    TCHAR appName[APPNAME_MAX_SIZE];
    DWORD cbAppName;
    DWORD iApp = 0;

    status = ERROR_SUCCESS;
    while (status != ERROR_NO_MORE_ITEMS)
    {
        cbAppName = _countof(appName);
        status = RegEnumKeyEx(hkApps, iApp, appName, &cbAppName,
            NULL, NULL, NULL, NULL);
        iApp++;
        if (status != ERROR_SUCCESS)
            continue;

        isCustom |= ApplyThemeAppSoundEvents(themePath,
            hkApps, appName, schemeInternalName);
    }

    RegCloseKey(hkApps);

    if (!isCustom)
    {
        if (schemeInternalName[0] != TEXT('\0'))
            SetSoundSchemeName(schemeInternalName);
        return isCustom;
    }

    /* With modified sounds */

    if (schemeInternalName[0] == TEXT('\0'))
    {
        /* No system name specified */
        if (schemeDisplayName[0] == TEXT('\0'))
        {
            /* "Custom" */
            if (!GetCustomSchemeName(schemeDisplayName))
                return isCustom;
        }

        if (CreateSoundScheme(szCustomScheme, schemeDisplayName))
            SetSoundSchemeName(szCustomScheme);

        return isCustom;
    }

    /* Modified system scheme */
    if (GetSchemeNameModified(schemeDisplayName))
    {
        SetSoundSchemeName(szModifiedScheme);
        CreateSoundScheme(szModifiedScheme, schemeDisplayName);
    }

    return isCustom;
}

#undef INTERNALNAME_MAX_SIZE
#undef DISPLAYNAME_MAX_SIZE

static
BOOL ApplyWinIniSound(_In_ const TCHAR *iniPath, _In_ const TCHAR *eventName)
{
    /* Format:
     * EventName=file.wav,Display Name
     */
    TCHAR soundPath[MAX_PATH + 14];
    if (GetPrivateProfileString(szSoundsSection, eventName, NULL,
        soundPath, _countof(soundPath), iniPath) < 10)
    {
        return FALSE;
    }

    unsigned int c;
    for (c = lstrlen(soundPath) - 2; c > 0; c--)
    {
        if (soundPath[c] == TEXT(','))
        {
            soundPath[c] = TEXT('\0');
            break;
        }
    }
    if (c < 1)
    {
        /* No comma found */
        return FALSE;
    }

    GetAbsolutePath(iniPath, soundPath, MAX_PATH);

    if (!PathFileExists(soundPath))
        return FALSE;

    ApplyAppSoundEvent(szDefaultScheme, eventName, szCustomScheme, soundPath);
    return ApplyAppSoundEvent(szDefaultScheme, eventName, szCurrentScheme,
        soundPath);
}

BOOL ApplyWinIniSounds(_In_ const TCHAR *iniPath)
{
    BOOL changed = FALSE;

#define Apply(name) changed |= ApplyWinIniSound(iniPath, TEXT(name))

    Apply("SystemAsterisk");
    Apply("SystemHand");
    Apply("SystemDefault");
    Apply("SystemExclamation");
    Apply("RingIn");
    Apply("RingOut");
    Apply("SystemQuestion");
    Apply("SystemExit");
    Apply("SystemStart");

#undef Apply

    if (!changed)
        return FALSE;

    if (CreateSoundScheme(szCustomScheme, TEXT("Windows 3")))
        SetSoundSchemeName(szCustomScheme);

    return TRUE;
}

#endif  /* defined(WITH_THEMES) */
