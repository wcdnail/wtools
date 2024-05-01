/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Save and rename classic scheme dialog
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "classtyl.h"
#include "resource.h"
#include "util.h"
#include "wndclas.h"

static HWND g_hDlg;

_Success_(return)
static
BOOL SetDefaultSchemeName(HWND hControl)
{
    TCHAR name[SCHEMENAME_MAX_SIZE];
    if (LoadString(g_propSheet.hInstance, IDS_SAVESCHEME_DEFAULT_NAME,
        name, SCHEMENAME_MAX_SIZE - 1) <= 0)
    {
        return FALSE;
    }

    if (SendMessage(hControl, CB_ADDSTRING, 0L, (LPARAM)name) < 0)
        return FALSE;

    SendMessage(hControl, CB_SETCURSEL, 0L, 0L);
    return TRUE;
}

_Success_(return)
static
BOOL SetDefaultSizeName(HWND hControl)
{
    TCHAR name[SIZENAME_MAX_SIZE];
    if (LoadString(g_propSheet.hInstance, IDS_SAVESCHEME_DEFAULT_SIZE,
        name, SIZENAME_MAX_SIZE - 1) <= 0)
    {
        return FALSE;
    }

    if (SendMessage(hControl, CB_ADDSTRING, 0L, (LPARAM)name) < 0)
        return FALSE;

    SendMessage(hControl, CB_SETCURSEL, 0L, 0L);
    return TRUE;
}

_Success_(return)
static
BOOL TryToSaveScheme(void)
{
    TCHAR schemeName[SCHEMENAME_MAX_SIZE];
    if (!SanitizeNewName(g_hDlg, schemeName))
        return FALSE;

    TCHAR sizeName[SIZENAME_MAX_SIZE];
    if (!SanitizeNewSize(g_hDlg, sizeName))
        return FALSE;

    SCHEME *pScheme = FindScheme(g_schemes->schemes, schemeName);
    SCHEME *pSize = NULL;
    if (pScheme)
    {
        /* Existing scheme; look for the size */
        pSize = FindScheme(pScheme->sizes, sizeName);
        if (pSize)
        {
            if (ShowMessageFromResource(
                g_hDlg, IDS_SAVESCHEME_CONFIRM_OVERWRITE, IDS_SAVESCHEME,
                MB_YESNO) != IDYES)
            {
                return FALSE;
            }
        }
    }

    if (!SaveSchemeColors(&g_schemes->scheme, schemeName))
        goto Error;

    if (!SaveSchemeSize(&g_schemes->scheme, schemeName, sizeName))
        goto Error;

    BOOL bNewScheme;
    if (!pScheme)
    {
        pScheme = CreateScheme(schemeName);
        if (!pScheme)
            goto Error;
        bNewScheme = TRUE;

        /* Insert in the beginning */
        pScheme->next = g_schemes->schemes;
        g_schemes->schemes = pScheme;
    }
    else
        bNewScheme = FALSE;

    if (!pSize)
    {
        pSize = CreateScheme(sizeName);
        if (!pSize)
        {
            if (bNewScheme)
                FreeSchemes(&pScheme);
            goto Error;
        }

        pSize->next = pScheme->sizes;
        pScheme->sizes = pSize;
    }

    g_schemes->selected.color = pScheme;
    g_schemes->selected.size = pSize;

    return TRUE;

Error:
    ShowMessageFromResource(
        g_hDlg, IDS_SAVESCHEME_ERROR, IDS_ERROR, MB_OK | MB_ICONERROR);

    return FALSE;
}

static
void LoadSizeCombobox(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_NEWNAME);
    if (!hCombo)
        return;

    SCHEME_SELECTION scheme;

    scheme.color = (SCHEME *)GetComboBoxSelectedData(hCombo);
    if (!scheme.color || !scheme.color->sizes)
        return;

    hCombo = GetDlgItem(g_hDlg, IDC_CLASSIC_NEWSIZE);
    if (!hCombo)
        return;

    AddSchemesToCombo(hCombo, scheme.color->sizes, scheme.color->sizes);
}

static
void InitDialog(void)
{
    HWND hControl;

    hControl = GetDlgItem(g_hDlg, IDC_CLASSIC_NEWNAME);
    if (hControl)
    {
        if (g_schemes->schemes)
        {
            AddSchemesToCombo(
                hControl, g_schemes->schemes, g_schemes->selected.color);
        }
        else
            SetDefaultSchemeName(hControl);

        SendMessage(
            hControl, CB_LIMITTEXT, (WPARAM)(SCHEMENAME_MAX_SIZE - 1), 0L);
    }

    hControl = GetDlgItem(g_hDlg, IDC_CLASSIC_NEWSIZE);
    if (hControl)
    {
        SendMessage(
            hControl, CB_LIMITTEXT, (WPARAM)(SIZENAME_MAX_SIZE - 1), 0L);

        if (g_schemes->selected.color)
        {
            AddSchemesToCombo(hControl, g_schemes->selected.color->sizes,
                g_schemes->selected.size);
        }
        else
            SetDefaultSizeName(hControl);
    }
}

_Success_(return)
BOOL SanitizeNewName(HWND hWnd, _Out_writes_(SCHEMENAME_MAX_SIZE) TCHAR *name)
{
    HWND hEdit = GetDlgItem(hWnd, IDC_CLASSIC_NEWNAME);
    if (!hEdit)
        goto Error;

    if (GetWindowText(hEdit, name, SCHEMENAME_MAX_SIZE) <= 0)
        goto NoName;

#if !defined(MINIMAL)
    StrSanitize(name);
    SetWindowText(hEdit, name);
#endif

    if (lstrlen(name) <= 0)
        goto NoName;

    return TRUE;

NoName:
    ShowMessageFromResource(
        hWnd, IDS_SAVESCHEME_NO_NAME, IDS_SAVESCHEME, MB_OK);
    return FALSE;

Error:
    ShowMessageFromResource(
        hWnd, IDS_SAVESCHEME_ERROR, IDS_ERROR, MB_OK | MB_ICONERROR);
    return FALSE;
}

_Success_(return)
BOOL SanitizeNewSize(HWND hWnd, _Out_writes_(SIZENAME_MAX_SIZE) TCHAR *name)
{
    HWND hEdit = GetDlgItem(hWnd, IDC_CLASSIC_NEWSIZE);
    if (!hEdit)
        goto Error;

    if (GetWindowText(hEdit, name, SIZENAME_MAX_SIZE) <= 0)
        goto NoName;

#if !defined(MINIMAL)
    StrSanitize(name);
    SetWindowText(hEdit, name);
#endif

    if (lstrlen(name) <= 0)
        goto NoName;

    return TRUE;

NoName:
    ShowMessageFromResource(
        hWnd, IDS_SAVESCHEME_NO_SIZE, IDS_SAVESCHEME, MB_OK);
    return FALSE;

Error:
    ShowMessageFromResource(
        hWnd, IDS_SAVESCHEME_ERROR, IDS_ERROR, MB_OK | MB_ICONERROR);
    return FALSE;
}

/* Note: Unlike a WNDPROC, a DLCPROC should NOT call DefWindowProc.
 * It should return TRUE when a message is processed, or FALSE otherwise
 */
INT_PTR CALLBACK SaveSchemeDlgProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hDlg = hWnd;

        InitDialog();
        return TRUE;

    case WM_COMMAND:
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            switch (LOWORD(wParam))
            {
            case IDOK:
                if (TryToSaveScheme())
                    EndDialog(hWnd, (INT_PTR)TRUE);
                break;

            case IDCANCEL:
                EndDialog(hWnd, (INT_PTR)FALSE);
                break;
            }
            break;

        case CBN_SELCHANGE:
            switch (LOWORD(wParam))
            {
            case IDC_CLASSIC_NEWNAME:
                LoadSizeCombobox();
                break;
            }
            break;
        }

        return TRUE;
    }

    return FALSE;
}
