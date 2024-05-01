/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Program entry point
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "mincrt.h"
#include "preview.h"
#include "resource.h"
#include "util.h"

#include <CommCtrl.h>
#include <shellapi.h>
#include <prsht.h>

PROPSHEET g_propSheet;

/* Property sheet dialog proc forward definitions */

INT_PTR CALLBACK ClassicStylePageProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#if !defined(MINIMAL)

INT_PTR CALLBACK EffSettingsPageProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif

#if defined(WITH_CMDLINE)
/* Property sheet command line functions */
#if defined(WITH_THEMES)
UINT ApplyThemeFromCommandLine(_In_ WCHAR *filePath);
#endif

UINT ParseClassicStyleCommandLine(int argc, _In_ WCHAR **argv);

#if !defined(MINIMAL) && WINVER >= WINVER_2K
UINT ParseEffectsCommandLine(int argc, _In_ WCHAR **argv);
#endif
#endif

static const struct tagPAGE
{
    WORD idDlg;
    DLGPROC dlgProc;
    WCHAR *name;  /* Used for selecting a page with the "@name" parameter */
} g_pages[] =
{
    { IDD_CLASSIC, ClassicStylePageProc, PAGENAME_CLASSIC },
#if !defined(MINIMAL)
    { IDD_EFF, EffSettingsPageProc, PAGENAME_EFF },
#endif
};

enum PageNums
{
    PAGE_CLASSIC,
#if !defined(MINIMAL)
    PAGE_EFF,
#endif
};

static
void InitPage(PROPSHEETHEADER *ppsh, WORD idDlg, DLGPROC dlgProc)
{
    PROPSHEETPAGE psp;

    memset(&psp, 0, sizeof(PROPSHEETPAGE));
    psp.dwSize = sizeof(PROPSHEETPAGE);
    psp.dwFlags = PSP_DEFAULT;
    psp.hInstance = g_propSheet.hInstance;
    psp.pszTemplate = MAKEINTRESOURCE(idDlg);
    psp.pfnDlgProc = dlgProc;

    HPROPSHEETPAGE hPage = CreatePropertySheetPage(&psp);
    if (hPage)
        ppsh->phpage[ppsh->nPages++] = hPage;
}

static
void SetLargeIcon(void)
{
    HICON hIcon = LoadImage(
        g_propSheet.hInstance, MAKEINTRESOURCE(IDI_PROGRAM),
        IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
    SendMessage(g_propSheet.hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

static
int CALLBACK PropSheetProc(HWND hWnd, UINT uMsg, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (uMsg)
    {
    case PSCB_INITIALIZED:
        g_propSheet.hWnd = hWnd;
        SetLargeIcon();
    }

    return 0;
}

_Success_(return < RETURN_ERROR)
static
UINT DisplayPropSheet(UINT nStartPage)
{
    PROPSHEETHEADER psh;
    HPROPSHEETPAGE hpsp[_countof(g_pages)];

    memset(&psh, 0, sizeof(PROPSHEETHEADER));
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags =
        PSH_USECALLBACK | PSH_PROPTITLE | PSH_USEICONID | PSH_NOCONTEXTHELP;
    psh.hInstance = g_propSheet.hInstance;
    psh.pszIcon = MAKEINTRESOURCE(IDI_PROGRAM);
    psh.pszCaption = MAKEINTRESOURCE(IDS_PROPSHEET_NAME);
    psh.nPages = 0;
    psh.nStartPage = nStartPage;
    psh.phpage = hpsp;
    psh.pfnCallback = PropSheetProc;

    for (UINT i = 0; i < _countof(g_pages); i++)
        InitPage(&psh, g_pages[i].idDlg, g_pages[i].dlgProc);

    INT_PTR ret = PropertySheet(&psh);

    if (ret < 0)
        goto Error;

    if (ret == 0)
        return RETURN_NO_CHANGES;

    return RETURN_CHANGES;

Error:
    ShowMessageFromResource(NULL, IDS_ERROR_GENERIC, IDS_ERROR, MB_OK);
    return RETURN_ERROR;
}

static
BOOL ShowRunningInstance(void)
{
    CreateMutex(0, TRUE, TEXT("TortoClassicAppearanceConfig"));
    if (GetLastError() != ERROR_ALREADY_EXISTS)
        return FALSE;

    HWND hExistingWnd;

    hExistingWnd = FindWindowEx(NULL, NULL,
        MAKEINTATOM(0x8002), TEXT("Appearance Properties"));

    if (!hExistingWnd)
    {
        hExistingWnd = FindWindowEx(
            NULL, NULL, MAKEINTATOM(0x8002), TEXT("Propiedades de Apariencia"));
    }

    if (!hExistingWnd)
    {
        /* No window found... open it again */
        return FALSE;
    }

    SetForegroundWindow(hExistingWnd);
    return TRUE;
}

_Success_(return < RETURN_ERROR)
static
UINT InitGUI(UINT nStartPage)
{
    if (ShowRunningInstance())
        return RETURN_EXISTING_INSTANCE;

#if WINVER >= WINVER_XP
    INITCOMMONCONTROLSEX icce;
    icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icce.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_UPDOWN_CLASS | ICC_LINK_CLASS;
    InitCommonControlsEx(&icce);
#else
    InitCommonControls();
#endif

    if (!RegisterSchemePreviewControl())
    {
        ShowMessageFromResource(NULL, IDS_ERROR_GENERIC, IDS_ERROR, MB_OK);
        return RETURN_ERROR;
    }

    UINT ret = DisplayPropSheet(nStartPage);

    UnregisterSchemePreviewControl();

    return ret;
}

#if defined(WITH_CMDLINE)
_Success_(return < RETURN_ERROR)
static
UINT ParseCommandLine(int argc, WCHAR **argv)
{
    /* argc is always > 1 */

    int len = lstrlenW(argv[1]);
    if (len < 2 || argv[1][0] != L'@')
    {
        /* Does not start with "@" */

#if defined(WITH_THEMES)
        if (argc == 2 && argv[1][0] != L'/' && argv[1][0] != L'-')
            return ApplyThemeFromCommandLine(argv[1]);
#endif

        PrintResource(IDS_USAGE_GENERAL);
        return RETURN_USAGE;
    }

    WCHAR *page = &argv[1][1];

    for (UINT iPage = 0; iPage < _countof(g_pages); iPage++)
    {
        if (lstrcmpiW(page, g_pages[iPage].name) != 0)
            continue;

        /* Page match */
        if (argc <= 2)
        {
            /* No extra arguments */
            return InitGUI(iPage);
        }

        UINT ret;
        switch (iPage)
        {
        case PAGE_CLASSIC:
            ret = ParseClassicStyleCommandLine(argc - 2, &argv[2]);
            if (ret == RETURN_USAGE)
                PrintResource(IDS_USAGE_APPEARANCE);
            break;

#if !defined(MINIMAL)
        case PAGE_EFF:
            ret = ParseEffectsCommandLine(argc - 2, &argv[2]);
            if (ret == RETURN_USAGE)
                PrintResource(IDS_USAGE_EFF);
            break;
#endif

        default:
            PrintResource(IDS_USAGE_GENERAL);
            ret = RETURN_USAGE;
        }

        return ret;
    }

    /* No page match */
    PrintResource(IDS_USAGE_GENERAL);
    return RETURN_USAGE;
}
#endif

_Success_(return == 0)
static
UINT InitProgram(void)
{
    g_propSheet.heap = GetProcessHeap();
    if (!g_propSheet.heap)
        goto Error;

    g_propSheet.hInstance = GetModuleHandle(NULL);
    if (!g_propSheet.hInstance)
        goto Error;

    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen)
    {
        g_propSheet.dpi = GetDeviceCaps(hdcScreen, LOGPIXELSY);
        ReleaseDC(NULL, hdcScreen);
    }
    else
        g_propSheet.dpi = USER_DEFAULT_SCREEN_DPI;

    UINT ret;

#if defined(WITH_CMDLINE)
    int argc = 0;
    WCHAR **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc > 1)
        ret = ParseCommandLine(argc, argv);
    else
#endif
        ret = InitGUI(0);

#if defined(WITH_CMDLINE)
    LocalFree(argv);
#endif

    return ret;

Error:
    ShowMessageFromResource(NULL, IDS_ERROR_MEM, IDS_ERROR, MB_OK);
    return RETURN_ERROR;
}

#if 0
#if defined(__MINGW64__)
void WINAPI __main(void)
#else
void WINAPI _main(void)
#endif
{
    ExitProcess(InitProgram());
}
#endif