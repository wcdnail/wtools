#pragma once
#if !defined(REG_H)
#define REG_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if WINVER >= WINVER_2K

#include <Shlwapi.h>

#elif defined(WINVER_IS_95)

/* RegDeleteKey() deletes subkeys only on 95 */
#define SHDeleteKey   RegDeleteKey
#define SHDeleteKeyA  RegDeleteKeyA
#define SHDeleteKeyW  RegDeleteKeyW

#else  /* NT4 */

/* Custom implementations */
_Success_(return == ERROR_SUCCESS)
LSTATUS SHDeleteKeyA(HKEY hkey, _In_opt_ const CHAR *pszSubKey);

_Success_(return == ERROR_SUCCESS)
LSTATUS SHDeleteKeyW(HKEY hkey, _In_opt_ const WCHAR *pszSubKey);

#ifdef UNICODE
#define SHDeleteKey  SHDeleteKeyW
#else
#define SHDeleteKey  SHDeleteKeyA
#endif

#endif  /* NT 4 */

_Success_(return == ERROR_SUCCESS)
LSTATUS RegQueryDWord(
    HKEY hKey,
    _In_opt_ const TCHAR *szValue,
    _Out_ DWORD *pdwData);

_Success_(return == ERROR_SUCCESS)
LSTATUS RegQueryQWord(
    HKEY hKey,
    _In_opt_ const TCHAR *szValue,
    _Out_ UINT64 *pqwData);

BOOL RegQueryBool(HKEY hKey, _In_opt_ const TCHAR *szValue, BOOL bDefaultValue);

_Success_(return == ERROR_SUCCESS)
LSTATUS RegQueryString(
    HKEY hKey,
    _In_opt_ const TCHAR *szValue,
    _Out_ TCHAR *szData,
    _Inout_opt_ DWORD *pcbData);

BOOL RegSetValueIfDifferent(
    HKEY hKey,
    _In_opt_ const TCHAR *lpValueName,
    DWORD dwType,
    _In_ const BYTE *lpData,
    DWORD cbData);

#endif  /* !defined(REG_H) */
