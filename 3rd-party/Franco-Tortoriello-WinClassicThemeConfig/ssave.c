/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Screen Saver functionality.
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if defined(WITH_THEMES)

#include "bg.h"

const TCHAR szScreenSaverValue[] =
    TEXT("SCRNSAVE.EXE");

_Success_(return)
BOOL ApplyScreenSaver(_In_ const TCHAR *path)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szDesktopKey, 0,
        KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    int len = lstrlen(path);
    const BOOL enableSSaver = (len > 3);

    if (enableSSaver)
    {
        status = RegSetValueEx(hKey, szScreenSaverValue, 0, REG_SZ,
            (BYTE *)path, (len + 1) * sizeof(TCHAR));
    }
    else
        status = RegDeleteValue(hKey, szScreenSaverValue);

    RegCloseKey(hKey);

    if (status != ERROR_SUCCESS)
        return FALSE;

    return SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, enableSSaver, 0,
        SPIF_UPDATEINIFILE);
}

#endif  /* defined(WITH_THEMES) */
