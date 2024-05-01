/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Save and rename classic scheme dialog
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "classtyl.h"
#include "resource.h"
#include "util.h"
#include "wndclas.h"

static HWND g_hDlg;

_Success_(return)
static
BOOL TryToRenameSchemeSize(void)
{
    TCHAR newName[SIZENAME_MAX_SIZE];
    if (!SanitizeNewSize(g_hDlg, newName))
        return FALSE;

    if (lstrcmpi(g_schemes->selected.size->name, newName) == 0)
        /* Nothing to do */
        return TRUE;

    if (FindScheme(g_schemes->selected.color->sizes, newName))
    {
        ShowMessageFromResource(
            g_hDlg, IDS_RENAMESIZE_CONFLICT, IDS_RENAMESCHEME, MB_OK);
        return FALSE;
    }

    if (!RenameSchemeSize(
        g_schemes->selected.color, g_schemes->selected.size, newName))
    {
        goto Error;
    }

    return TRUE;

Error:
    ShowMessageFromResource(
        g_hDlg, IDS_RENAMESIZE_ERROR, IDS_ERROR, MB_OK | MB_ICONERROR);

    return FALSE;
}

_Success_(return)
static
BOOL TryToRenameScheme(void)
{
    if (!TryToRenameSchemeSize())
        return FALSE;

    TCHAR newName[SCHEMENAME_MAX_SIZE];
    if (!SanitizeNewName(g_hDlg, newName))
        return FALSE;

    if (lstrcmpi(g_schemes->selected.color->name, newName) == 0)
        return TRUE;

    if (FindScheme(g_schemes->schemes, newName))
    {
        ShowMessageFromResource(
            g_hDlg, IDS_RENAMESCHEME_CONFLICT, IDS_RENAMESCHEME, MB_OK);
        return FALSE;
    }

    if (!RenameScheme(g_schemes->selected.color, newName))
        goto Error;

    return TRUE;

Error:
    ShowMessageFromResource(
        g_hDlg, IDS_RENAMESCHEME_ERROR, IDS_ERROR, MB_OK | MB_ICONERROR);

    return FALSE;
}

static
void InitDialog(void)
{
    HWND hEdit;

    /* Default to current names */

    hEdit = GetDlgItem(g_hDlg, IDC_CLASSIC_NEWNAME);
    if (hEdit)
    {
        SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)(SCHEMENAME_MAX_SIZE - 1), 0L);
        SetWindowText(hEdit, g_schemes->selected.color->name);
    }

    hEdit = GetDlgItem(g_hDlg, IDC_CLASSIC_NEWSIZE);
    if (hEdit)
    {
        SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)(SIZENAME_MAX_SIZE - 1), 0L);
        SetWindowText(hEdit, g_schemes->selected.size->name);
    }
}

INT_PTR CALLBACK RenameSchemeDlgProc(
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
                if (TryToRenameScheme())
                    EndDialog(hWnd, (INT_PTR)TRUE);
                break;

            case IDCANCEL:
                EndDialog(hWnd, (INT_PTR)FALSE);
                break;
            }
            break;
        }

        return TRUE;
    }

    return FALSE;
}
