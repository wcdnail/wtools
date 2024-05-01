/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   SHLWAPI registry functions
 *
 * PROGRAMMERS: Juergen Schmied
 *              Guy Albertelli
 *              Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "mincrt.h"

#if WINVER < WINVER_2K

/* Delete a registry key and any sub keys/values present.
 * Returns ERROR_SUCCESS if the key is deleted; otherwise, an error code from
 * RegOpenKeyEx(), RegQueryInfoKey(), RegEnumKeyEx() or RegDeleteKey().
 */
_Success_(return == ERROR_SUCCESS)
LSTATUS SHDeleteKeyW(HKEY hKey, _In_opt_ const WCHAR *subKey)
{
    HKEY hSubKey = NULL;
    LSTATUS status = RegOpenKeyExW(hKey, subKey, 0,
        KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hSubKey);
    if (status != ERROR_SUCCESS)
        return status;

    /* Find the maximum subkey length so that we can allocate a buffer */
    DWORD maxSubKeyLen = 0;
    status = RegQueryInfoKeyW(hSubKey, NULL, NULL, NULL, NULL,
        &maxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);
    if (status != ERROR_SUCCESS)
    {
        RegCloseKey(hSubKey);
        return status;
    }

    WCHAR nameBuffer[MAX_PATH];
    WCHAR *name = nameBuffer;
    maxSubKeyLen++;
    if (maxSubKeyLen > MAX_PATH)
    {
        /* Name too big: alloc a buffer for it */
        name = Alloc(0, maxSubKeyLen * sizeof(WCHAR));

        if (!name)
        {
            RegCloseKey(hSubKey);
            return ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    DWORD nameLen;
    while (status == ERROR_SUCCESS)
    {
        nameLen = maxSubKeyLen;
        status = RegEnumKeyExW(
            hSubKey, 0, name, &nameLen, NULL, NULL, NULL, NULL);

        if (status == ERROR_SUCCESS || status == ERROR_MORE_DATA)
            status = SHDeleteKeyW(hSubKey, name);
    }
    if (status == ERROR_NO_MORE_ITEMS)
        status = ERROR_SUCCESS;

    /* Free buffer if allocated */
    if (name != nameBuffer)
        Free(name);

    RegCloseKey(hSubKey);
    return RegDeleteKeyW(hKey, subKey);
}

/* See SHDeleteKeyW.
 * This function forwards to the unicode version directly, to avoid
 * handling subkeys that are not representable in ASCII.
 */
_Success_(return == ERROR_SUCCESS)
LSTATUS SHDeleteKeyA(HKEY hKey, _In_opt_ const CHAR *subKey)
{
    WCHAR subKeyW[MAX_PATH];

    if (!MultiByteToWideChar(CP_ACP, 0, subKey, -1, subKeyW, _countof(subKeyW)))
        return (LSTATUS)GetLastError();

    return SHDeleteKeyW(hKey, subKeyW);
}

#endif  /* WINVER < WINVER_2K */

_Success_(return == ERROR_SUCCESS)
LSTATUS RegQueryDWord(
    HKEY hKey,
    _In_opt_ const TCHAR *szValue,
    _Out_ DWORD *pdwData)
{
    DWORD dwType;
    DWORD cbData = sizeof(DWORD);

    LSTATUS status = RegQueryValueEx(hKey, szValue, NULL,
        &dwType, (BYTE *)pdwData, &cbData);

    if (status != ERROR_SUCCESS)
        return status;

    if (dwType != REG_DWORD)
        return ERROR_BAD_FORMAT;

    return status;
}

_Success_(return == ERROR_SUCCESS)
LSTATUS RegQueryQWord(
    HKEY hKey,
    _In_opt_ const TCHAR *szValue,
    _Out_ UINT64 *pqwData)
{
    DWORD dwType;
    DWORD cbData = sizeof(UINT64);

    LSTATUS status = RegQueryValueEx(hKey, szValue, NULL,
        &dwType, (BYTE *)pqwData, &cbData);

    if (status != ERROR_SUCCESS)
        return status;

    if (dwType != REG_QWORD)
        return ERROR_BAD_FORMAT;

    return status;
}

BOOL RegQueryBool(HKEY hKey, _In_opt_ const TCHAR *szValue, BOOL bDefaultValue)
{
    DWORD dwValue;
    if (RegQueryDWord(hKey, szValue, &dwValue) == ERROR_SUCCESS)
        return dwValue > 0;

    return bDefaultValue;
}

/* Query a REG_SZ or REG_EXPAND_SZ value from the registry, expanding
 * REG_EXPAND_SZ values.
 */
_Success_(return == ERROR_SUCCESS)
LSTATUS RegQueryString(
    HKEY hKey,
    _In_opt_ const TCHAR *szValue,
    _Out_ TCHAR *szData,
    _Inout_opt_ DWORD *pcbData)
{
    DWORD bufferChars = *pcbData / sizeof(TCHAR);
    DWORD dwType;

    LSTATUS status = RegQueryValueEx(hKey, szValue, NULL,
        &dwType, (BYTE *)szData, pcbData);

    if (status != ERROR_SUCCESS)
        return status;

    if (dwType != REG_SZ && dwType != REG_EXPAND_SZ)
        return ERROR_BAD_FORMAT;

    if (*pcbData < sizeof(TCHAR))
    {
        /* Null string; RegQueryValueEx() does not change the returned data */
        /*szData[0] = TEXT('\0');*/
        return ERROR_FILE_NOT_FOUND;
    }

    /* It is not guaranteed that the string is null-terminated; terminate it */
    szData[bufferChars - 1] = TEXT('\0');

    if (dwType != REG_EXPAND_SZ)
        return status;

    /* Expand string */

    TCHAR *szOrigData = Alloc(0, bufferChars);
    if (!szOrigData)
        return ERROR_NOT_ENOUGH_MEMORY;

    if (!lstrcpy(szOrigData, szData))
    {
        Free(szOrigData);
        return ERROR_NOT_ENOUGH_MEMORY;
    }

#if !defined(UNICODE)
    /* Required by ExpandEnvironmentStringsA() */
    bufferChars--;
#endif
    DWORD len = ExpandEnvironmentStrings(szOrigData, szData, bufferChars);
    if (len == 0 || len > bufferChars)
        return ERROR_NOT_ENOUGH_MEMORY;

    *pcbData = len * sizeof(TCHAR);
    return status;
}

/* Returns TRUE if the new value was set and it is different than the previous
 * one.
 * The key must be already opened with at least the KEY_QUERY_VALUE and
 * KEY_SET_VALUE access rights.
 */
BOOL RegSetValueIfDifferent(
    HKEY hKey,
    _In_opt_ const TCHAR *lpValueName,
    DWORD dwType,
    _In_ const BYTE *lpData,
    DWORD cbData)
{
    DWORD dwOldType;
    DWORD cbOldData = cbData;
    BYTE *lpOldData = Alloc(0, cbOldData);
    LSTATUS result;

    if (lpOldData)
    {
        result = RegQueryValueEx(hKey, lpValueName,
            NULL, &dwOldType, lpOldData, &cbOldData);

        BOOL sameData = (result == ERROR_SUCCESS && dwOldType == dwType &&
            cbOldData == cbData && memcmp(lpOldData, lpData, cbOldData) == 0);

        Free(lpOldData);

        if (sameData)
            return FALSE;
    }

    result = RegSetValueEx(hKey, lpValueName, 0, dwType, lpData, cbData);
    return result == ERROR_SUCCESS;
}
