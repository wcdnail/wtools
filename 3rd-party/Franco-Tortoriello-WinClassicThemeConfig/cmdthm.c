/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Themes command line handling
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */
#include "pch.h"
#include "app.h"

#if defined(WITH_CMDLINE) && defined(WITH_THEMES)

#include "thm.h"
#include "resource.h"
#include "util.h"

#include <Shlwapi.h>

UINT ApplyThemeFromCommandLine(_In_ WCHAR *filePathW)
{
#if defined UNICODE
#define filePath filePathW
#else
    TCHAR filePath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, filePathW, -1, filePath,
        MAX_PATH * sizeof(TCHAR), NULL, NULL);
#endif

    if (!PathFileExists(filePath))
    {
        PrintLastErrorMessage();
        return RETURN_ERROR;
    }

    TCHAR absolutePath[MAX_PATH];
    TCHAR *lpFilePart;  /* Used to determine if the path is to a file or not */
    DWORD len = GetFullPathName(filePath, MAX_PATH, absolutePath, &lpFilePart);
    if (len == 0)
    {
        PrintLastErrorMessage();
        return RETURN_ERROR;
    }
    if (len > MAX_PATH)
    {
        PrintResource(IDS_ERROR_MEM);
        return RETURN_ERROR;
    }

    if (!ApplyTheme(absolutePath))
    {
        PrintResource(IDS_THEME_INVALID);
        return RETURN_ERROR;
    }

    return RETURN_CHANGES;
#undef filePath
}

#endif  /* defined(WITH_CMDLINE) && defined(WITH_THEMES) */
