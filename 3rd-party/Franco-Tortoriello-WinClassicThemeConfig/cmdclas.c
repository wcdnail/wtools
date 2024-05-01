/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Classic Visual Style command line handling
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */
#include "pch.h"
#include "app.h"

#if defined(WITH_CMDLINE)

#include "classtyl.h"
#include "mincrt.h"
#include "resource.h"
#include "util.h"

#include <Shlwapi.h>

static const TCHAR szWin3ColorsKey[] = TEXT("Color Schemes");

#if WINVER >= WINVER_XP
/* List all schemes and sizes to a 80-column console */
static
UINT ListSchemes(void)
{
#define PrintNewLine() \
    WriteConsole(hStdOut, TEXT(NEWLINE), lstrlenA(NEWLINE), NULL, NULL)

#define SIZES_CHARS (SIZENAME_MAX_SIZE - 1) * 3 + 4

    UINT ret = RETURN_ERROR;

    TCHAR *szScheme;
    TCHAR *szSizes;

    g_schemes = (SCHEMES_DATA *)Alloc(HEAP_ZERO_MEMORY, sizeof(SCHEMES_DATA));
    if (!g_schemes)
    {
        PrintResource(IDS_ERROR_MEM);
        return ret;
    }

    if (!AllocAndLoadString(&szScheme, IDS_SCHEME))
    {
        PrintResource(IDS_ERROR_MEM);
        return ret;
    }

    if (!AllocAndLoadString(&szSizes, IDS_SIZES))
    {
        Free(szScheme);
        PrintResource(IDS_ERROR_MEM);
        return ret;
    }

    if (!AttachConsole(ATTACH_PARENT_PROCESS))
        goto FreeStrings;

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
    {
        FreeConsole();
        goto FreeStrings;
    }

    /* Do not start writing to the right of the prompt */
    PrintNewLine();

    TCHAR wzLineFormat[12];
    wsprintf(wzLineFormat, TEXT(" %%-%ds | %%s"), SCHEMENAME_MAX_SIZE - 1);

    TCHAR wzSizeFormat[12];
    wsprintf(wzSizeFormat, TEXT("%%-%ds"), SIZENAME_MAX_SIZE - 1);

    TCHAR szOut[(SCHEMENAME_MAX_SIZE - 1) + 3 + SIZES_CHARS + 1];

    /* Header */
    wsprintf(szOut, wzLineFormat, szScheme, szSizes);
    WriteConsole(hStdOut, szOut, lstrlen(szOut), NULL, NULL);
    PrintNewLine();

    for (unsigned int i = 0; i < SCHEMENAME_MAX_SIZE; i++)
        WriteConsole(hStdOut, TEXT("-"), 1, NULL, NULL);
    WriteConsole(hStdOut, TEXT(" + "), 3, NULL, NULL);
    for (unsigned int i = 0; i <= SIZES_CHARS; i++)
        WriteConsole(hStdOut, TEXT("-"), 1, NULL, NULL);
    PrintNewLine();

    LoadSchemesList();

    for (const SCHEME *pCurrentScheme = g_schemes->schemes;
        pCurrentScheme;
        pCurrentScheme = pCurrentScheme->next)
    {
        wsprintf(szOut, wzLineFormat, pCurrentScheme->name, TEXT("\0"));
        WriteConsole(hStdOut, szOut, lstrlen(szOut), NULL, NULL);

        unsigned int iSize = 1;
        for (const SCHEME *pCurrentSize = pCurrentScheme->sizes;
            pCurrentSize;
            pCurrentSize = pCurrentSize->next)
        {
            if (iSize > 1)
                WriteConsole(hStdOut, TEXT("  "), 2, NULL, NULL);

            wsprintf(szOut, wzSizeFormat, pCurrentSize->name);
            WriteConsole(hStdOut, szOut, lstrlen(szOut), NULL, NULL);

            if (iSize >= 3)
                /* No room */
                break;

            iSize++;
        }

        PrintNewLine();
    }

    FreeSchemes(&g_schemes->schemes);
    FreeConsole();
    SimulateEnterInput();
    ret = RETURN_NO_CHANGES;

FreeStrings:
    Free(szScheme);
    Free(szSizes);
    Free(g_schemes);
    return ret;

#undef PrintNewLine
#undef SIZES_CHARS
}
#endif

static
UINT ApplySchemeFromCommand(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName)
{
    SCHEME_SELECTION selection;

    selection.color = CreateScheme(schemeName);
    if (!selection.color)
    {
        PrintResource(IDS_ERROR_MEM);
        return RETURN_ERROR;
    }

    selection.size = CreateScheme(sizeName);
    if (!selection.size)
    {
        FreeScheme(&selection.color);
        PrintResource(IDS_ERROR_MEM);
        return RETURN_ERROR;
    }

    SCHEME_DATA schemeData;

    /* Load sane fallback defaults from the current scheme */
    LoadCurrentScheme(&schemeData);

    if (!LoadScheme(&schemeData, &selection))
    {
        FreeScheme(&selection.color);
        FreeScheme(&selection.size);
        PrintResource(IDS_SCHEME_LOAD_ERROR);
        return RETURN_ERROR;
    }

    ApplyScheme(&schemeData, &selection);

    FreeScheme(&selection.color);
    FreeScheme(&selection.size);

    return RETURN_CHANGES;
}

static
UINT SaveCurrentScheme(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName)
{
    SCHEME_DATA schemeData;
    LoadCurrentScheme(&schemeData);

    TCHAR newSchemeName[SCHEMENAME_MAX_SIZE];
    memcpy(newSchemeName, schemeName,
        min(SCHEMENAME_MAX_SIZE - 1, lstrlen(schemeName) + 1) * sizeof(TCHAR));
    newSchemeName[SCHEMENAME_MAX_SIZE - 1] = TEXT('\0');
    StrSanitize(newSchemeName);

    TCHAR newSizeName[SIZENAME_MAX_SIZE];
    memcpy(newSizeName, sizeName,
        min(SIZENAME_MAX_SIZE - 1, lstrlen(sizeName) + 1) * sizeof(TCHAR));
    newSizeName[SIZENAME_MAX_SIZE - 1] = TEXT('\0');
    StrSanitize(newSizeName);

    if (!SaveSchemeColors(&schemeData, newSchemeName))
    {
        PrintResource(IDS_SAVESCHEME_ERROR);
        return RETURN_ERROR;
    }

    if (!SaveSchemeSize(&schemeData, newSchemeName, newSizeName))
    {
        PrintResource(IDS_SAVESCHEME_ERROR);
        return RETURN_ERROR;
    }

    SaveSchemeSelection(newSchemeName, newSizeName);

    return RETURN_CHANGES;
}

/* Windows 3 CONTROL.INI format:
 * In the section "[color schemes]", Name=color0, color1, ..., color20
 * Where the colors are ordered almost like they are shown in the GUI, except
 * for "Inactive Title Bar Text" and "Button Highlight" for some reason...
 *  0 Desktop
 *  1 Application Workspace
 *  2 Window Background
 *  3 Window Text
 *  4 Menu Bar
 *  5 Menu Text
 *  6 Active Title Bar
 *  7 Inactive Title Bar
 *  8 Active Title Bar Text
 *  9 Active Border
 * 10 Inactive Border
 * 11 Window Frame
 * 12 Scroll Bars
 * 13 Button Face
 * 14 Button Shadow
 * 15 Button Text
 * 16 Disabled Text
 * 17 Highlight
 * 18 Highlighted Text
 * 19 Inactive Title Bar Text
 * 20 Button Highlight
 */

#define WIN3_SCHEME_SIZE 21 * 9 + 1

static
UINT ImportSchemeFromWin3(
    _In_ const TCHAR *iniPath, _In_ const TCHAR *schemeName)
{
    TCHAR szColors[WIN3_SCHEME_SIZE];
    if (GetPrivateProfileString(szWin3ColorsKey, schemeName, NULL,
        szColors, WIN3_SCHEME_SIZE, iniPath) < 21 * 2)
    {
        return RETURN_ERROR;
    }

    SCHEME_DATA data;
    TCHAR *pszColor = szColors;

#define ReadColor(iColor) \
    data.colors[iColor] = thextoul(pszColor, &pszColor) & 0x00FFFFFF; \
    if (!pszColor || pszColor[0] != TEXT(',')) \
        return RETURN_ERROR; \
    pszColor++

    ReadColor(COLOR_DESKTOP);
    ReadColor(COLOR_APPWORKSPACE);
    ReadColor(COLOR_WINDOW);
    ReadColor(COLOR_WINDOWTEXT);
    ReadColor(COLOR_MENU);
    ReadColor(COLOR_MENUTEXT);
    ReadColor(COLOR_ACTIVECAPTION);
    ReadColor(COLOR_INACTIVECAPTION);
    ReadColor(COLOR_CAPTIONTEXT);
    ReadColor(COLOR_ACTIVEBORDER);
    ReadColor(COLOR_INACTIVEBORDER);
    ReadColor(COLOR_WINDOWFRAME);
    ReadColor(COLOR_SCROLLBAR);
    ReadColor(COLOR_3DFACE);
    ReadColor(COLOR_3DSHADOW);
    ReadColor(COLOR_BTNTEXT);
    ReadColor(COLOR_GRAYTEXT);
    ReadColor(COLOR_HIGHLIGHT);
    ReadColor(COLOR_HIGHLIGHTTEXT);
    ReadColor(COLOR_INACTIVECAPTIONTEXT);
    data.colors[COLOR_3DHILIGHT] = thextoul(pszColor, &pszColor) & 0x00FFFFFF;

    /* Enable this for solid scrollbar backgrounds */
#if 0
    data.colors[COLOR_3DHILIGHT] = data.colors[COLOR_SCROLLBAR];
#endif

    SetWin3Style(&data);
    SetWin16Metrics(&data);

    /* Limit name length.
     * I support up to 28 chars, and Win3 32.
     * The file could also can be modified by hand.
     */
    TCHAR exportedName[SCHEMENAME_MAX_SIZE];
    memcpy(&exportedName, schemeName,
        min(SCHEMENAME_MAX_SIZE - 1, lstrlen(schemeName) + 1) * sizeof(TCHAR));
    exportedName[SCHEMENAME_MAX_SIZE - 1] = L'\0';

    SaveSchemeColors(&data, exportedName);
    SaveSchemeSize(&data, exportedName, TEXT("Normal"));

    return RETURN_CHANGES;
}

/* Copy the file to avoid API calls to redirect to the registry if the file is
 * named CONTROL.INI, regardless of the file path, and to support relative
 * paths.
 */
_Success_(return)
static
BOOL GetTempIniPath(_Out_writes_(MAX_PATH) TCHAR *iniPath)
{
    DWORD tempPathLen = GetTempPath(MAX_PATH - 12, iniPath);
    if (tempPathLen == 0)
    {
        PrintLastErrorMessage();
        return FALSE;
    }

    if (tempPathLen < 5 || tempPathLen > MAX_PATH - 12 - 1)
    {
        PrintResource(IDS_ERROR_MEM);
        return FALSE;
    }

    if (!lstrcpy(&iniPath[tempPathLen], TEXT("Win3Schm.ini")))
    {
        PrintResource(IDS_ERROR_MEM);
        return FALSE;
    }

    return TRUE;
}

static
UINT ImportWin3Schemes(_In_ const TCHAR *origIniPath)
{
    TCHAR iniPath[MAX_PATH];
    if (!GetTempIniPath(iniPath))
        return RETURN_ERROR;

    if (!CopyFile(origIniPath, iniPath, FALSE))
    {
        PrintLastErrorMessage();
        return RETURN_ERROR;
    }

#define WIN3_NAMES_SIZE (32 + 1) * 30 + 1  /* Keep low to avoid requiring the CRT */

    TCHAR names[WIN3_NAMES_SIZE];  /* Multiple null-terminated strings */
    UINT ret = RETURN_NO_CHANGES;

    if (GetPrivateProfileString(szWin3ColorsKey, NULL, NULL,
        names, WIN3_NAMES_SIZE, iniPath) < 1)
    {
        PrintResource(IDS_NO_SCHEMES);
        ret = RETURN_ERROR;
        goto Cleanup;
    }

#undef WIN3_NAMES_SIZE

    TCHAR *pName = names;
    while (*pName)
    {
        if (ImportSchemeFromWin3(iniPath, pName) != RETURN_ERROR)
            ret = RETURN_CHANGES;
        pName += lstrlen(pName) + 1;
    }

Cleanup:
    DeleteFile(iniPath);
    return ret;
}

static
UINT ExportWin3Schemes(_In_ const TCHAR *origIniPath)
{
    g_schemes = (SCHEMES_DATA *)Alloc(HEAP_ZERO_MEMORY, sizeof(SCHEMES_DATA));
    if (!g_schemes)
    {
        PrintResource(IDS_ERROR_MEM);
        return RETURN_ERROR;
    }

    TCHAR iniPath[MAX_PATH];
    if (!GetTempIniPath(iniPath))
        return RETURN_ERROR;

    if (PathFileExists(origIniPath) &&
        !CopyFile(origIniPath, iniPath, FALSE))
    {
        PrintLastErrorMessage();
        return RETURN_ERROR;
    }

    SCHEME_DATA data;

    SCHEME_SELECTION selection;
    selection.size = NULL;

    TCHAR szColors[WIN3_SCHEME_SIZE];

    LoadSchemesList();

    BOOL ret = RETURN_NO_CHANGES;

    for (SCHEME *pCurrentScheme = g_schemes->schemes;
        pCurrentScheme;
        pCurrentScheme = pCurrentScheme->next)
    {
        selection.color = pCurrentScheme;
        if (!LoadScheme(&data, &selection))
            continue;

        wsprintf(szColors,
            TEXT("%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X"),
            data.colors[COLOR_DESKTOP],
            data.colors[COLOR_APPWORKSPACE],
            data.colors[COLOR_WINDOW],
            data.colors[COLOR_WINDOWTEXT],
            data.colors[COLOR_MENU],
            data.colors[COLOR_MENUTEXT],
            data.colors[COLOR_ACTIVECAPTION],
            data.colors[COLOR_INACTIVECAPTION],
            data.colors[COLOR_CAPTIONTEXT],
            data.colors[COLOR_ACTIVEBORDER],
            data.colors[COLOR_INACTIVEBORDER],
            data.colors[COLOR_WINDOWFRAME],
            data.colors[COLOR_SCROLLBAR],
            data.colors[COLOR_3DFACE],
            data.colors[COLOR_3DSHADOW],
            data.colors[COLOR_BTNTEXT],
            data.colors[COLOR_GRAYTEXT],
            data.colors[COLOR_HIGHLIGHT],
            data.colors[COLOR_HIGHLIGHTTEXT],
            data.colors[COLOR_INACTIVECAPTIONTEXT],
            data.colors[COLOR_3DHILIGHT]);

        if (!WritePrivateProfileString(szWin3ColorsKey, pCurrentScheme->name,
            szColors, iniPath))
        {
            PrintLastErrorMessage();
            goto Cleanup;
        }

        ret = RETURN_CHANGES;
    }

Cleanup:
    FreeSchemes(&g_schemes->schemes);
    Free(g_schemes);

    if (!DeleteFile(origIniPath))
    {
        PrintLastErrorMessage();
        return RETURN_ERROR;
    }

    if (!MoveFile(iniPath, origIniPath))
    {
        PrintLastErrorMessage();
        ret = RETURN_ERROR;
    }

    return ret;
}

#undef WIN3_SCHEME_SIZE

UINT ParseClassicStyleCommandLine(int argc, _In_ WCHAR **argv)
{
    if (*argv[0] != L'/')
        return RETURN_USAGE;

    WCHAR *action = &argv[0][1];

#if WINVER >= WINVER_XP
    if (lstrcmpiW(action, L"List") == 0)
    {
        if (argc != 1)
            return RETURN_USAGE;

        return ListSchemes();
    }
#endif

    if (lstrcmpiW(action, L"Apply") == 0)
    {
        if (argc != 3)
            return RETURN_USAGE;

#if defined(UNICODE)
        return ApplySchemeFromCommand(argv[1], argv[2]);
#else
        TCHAR schemeName[SCHEMENAME_MAX_SIZE];
        TCHAR sizeName[SIZENAME_MAX_SIZE];

        WideCharToMultiByte(CP_ACP, 0, argv[1], -1, schemeName,
            SCHEMENAME_MAX_SIZE * sizeof(TCHAR), NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, argv[2], -1, sizeName,
            SIZENAME_MAX_SIZE * sizeof(TCHAR), NULL, NULL);

        return ApplySchemeFromCommand(schemeName, sizeName);
#endif
    }

    if (lstrcmpiW(action, L"SaveCurrent") == 0)
    {
        if (argc != 3)
            return RETURN_USAGE;

#if defined(UNICODE)
        return SaveCurrentScheme(argv[1], argv[2]);
#else
        TCHAR schemeName[SCHEMENAME_MAX_SIZE];
        TCHAR sizeName[SIZENAME_MAX_SIZE];

        WideCharToMultiByte(CP_ACP, 0, argv[1], -1, schemeName,
            SCHEMENAME_MAX_SIZE * sizeof(TCHAR), NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, argv[2], -1, sizeName,
            SIZENAME_MAX_SIZE * sizeof(TCHAR), NULL, NULL);

        return SaveCurrentScheme(schemeName, sizeName);
#endif
    }

    if (lstrcmpiW(action, L"ImportWin3") == 0)
    {
        if (argc != 2)
            return RETURN_USAGE;

#if defined(UNICODE)
        return ImportWin3Schemes(argv[1]);
#else
        TCHAR iniPath[MAX_PATH];
        WideCharToMultiByte(CP_ACP, 0, argv[1], -1, iniPath,
            MAX_PATH * sizeof(TCHAR), NULL, NULL);
        return ImportWin3Schemes(iniPath);
#endif
    }

    if (lstrcmpiW(action, L"ExportWin3") == 0)
    {
        if (argc != 2)
            return RETURN_USAGE;

#if defined(UNICODE)
        return ExportWin3Schemes(argv[1]);
#else
        TCHAR iniPath[MAX_PATH];
        WideCharToMultiByte(CP_ACP, 0, argv[1], -1, iniPath,
            MAX_PATH * sizeof(TCHAR), NULL, NULL);
        return ExportWin3Schemes(iniPath);
#endif
    }

    return RETURN_USAGE;
}

#endif  /* defined(WITH_CMDLINE) */
