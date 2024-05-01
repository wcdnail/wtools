/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Provides auxiliary functions
 *
 * PROGRAMMERS: ReactOS Team
 *              Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "mincrt.h"
#include "resource.h"

#include <CommCtrl.h>

#if defined(WITH_THEMES)
#include <Shlwapi.h>
#endif

_Success_(return >= 0)
static
int LengthOfStrResource(HINSTANCE hInst, UINT id)
{
    if (!hInst)
        return -1;

    /* There are always blocks of 16 strings */
    TCHAR *name = MAKEINTRESOURCE((id >> 4) + 1);

    /* Find the string table block */
    HRSRC hrSrc = FindResource(hInst, name, RT_STRING);
    if (!hrSrc)
        return -1;

    HGLOBAL hRes = LoadResource(hInst, hrSrc);
    if (!hRes)
        return -1;

    /* Note: Always use WCHAR because the resources are in Unicode */
    WCHAR *pStrLen = (WCHAR *)LockResource(hRes);
    if (!pStrLen)
        return -1;

    /* Find the string we're looking for */
    id &= 0xF; /* Position in the block, same as % 16 */
    for (UINT x = 0; x < id; x++)
        pStrLen += (*pStrLen) + 1;

    /* Found the string */
    return (int)(*pStrLen);
}

_Success_(return > 0)
int AllocAndLoadString(_Out_ TCHAR **pTarget, UINT id)
{
    int len = LengthOfStrResource(g_propSheet.hInstance, id);
    if (len++ > 0)
    {
        (*pTarget) = (TCHAR *)Alloc(0, len * sizeof(TCHAR));
        if (*pTarget)
        {
            int ret = LoadString(g_propSheet.hInstance, id, *pTarget, len);
            if (ret > 0)
                return ret;

            /* Could not load the string */
            Free((HLOCAL)(*pTarget));
        }
    }

    *pTarget = NULL;
    return 0;
}

_Success_(return != 0)
int ShowMessageFromResource(HWND hWnd, int msgId, int titleMsgId, UINT type)
{
    TCHAR *msg;
    TCHAR *msgTitle;

    if (!AllocAndLoadString(&msg, msgId))
        return 0;

    if (!AllocAndLoadString(&msgTitle, titleMsgId))
    {
        Free(msg);
        return 0;
    }

    int ret = MessageBox(hWnd, msg, msgTitle, MB_APPLMODAL | type);

    Free(msg);
    Free(msgTitle);

    return ret;
}

_Success_(return != 0)
LRESULT GetComboBoxSelectedData(HWND hCombo)
{
    INT_PTR sel = SendMessage(hCombo, CB_GETCURSEL, 0L, 0L);
    if (sel < 0)
        return 0;

    return SendMessage(hCombo, CB_GETITEMDATA, (WPARAM)sel, 0L);
}

_Success_(return)
BOOL SelectComboByItemData(HWND hCombo, LRESULT itemData)
{
    INT_PTR nCount = SendMessage(hCombo, CB_GETCOUNT, 0L, 0L);

    for (INT_PTR iListItem = 0; iListItem < nCount; iListItem++)
    {
        if (SendMessage(hCombo, CB_GETITEMDATA, (WPARAM)iListItem, 0L) ==
            itemData)
        {
            SendMessage(hCombo, CB_SETCURSEL, (WPARAM)iListItem, 0L);
            return TRUE;
        }
    }

    return FALSE;
}

/* Sets the value of an up-down control inside the valid range */
void SetUpDownOnRange(HWND hUpDown)
{
    LRESULT result = SendMessage(hUpDown, UDM_GETPOS, 0L, 0L);
    if (HIWORD(result) != 0)
        SendMessage(hUpDown, UDM_SETPOS, 0L, LOWORD(result));
}

/* If ids is positive, it creates a ToolTip with the text from the given
 * resource id.
 * Otherwise, it creates it with empty text.
 */
_Success_(return != NULL)
HWND CreateToolTip(HWND hDlg, int iControl, int ids)
{
    HWND hControl = GetDlgItem(hDlg, iControl);
    if (!hControl)
        return NULL;

    TCHAR szToolTip[200];
    if (ids > 0)
    {
        if (LoadString(g_propSheet.hInstance, ids, szToolTip, 200 - 1) <= 0)
            return NULL;
    }
    else
        szToolTip[0] = TEXT('\0');

    HWND hToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hDlg, NULL,
        g_propSheet.hInstance, NULL);
    if (!hToolTip)
        return NULL;

    TOOLINFO toolInfo;

    toolInfo.cbSize = sizeof(TOOLINFO);
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.hwnd = hDlg;
    toolInfo.uId = (UINT_PTR)hControl;
    toolInfo.hinst = g_propSheet.hInstance;
    toolInfo.lpszText = szToolTip;
    toolInfo.lParam = 0;

    SendMessage(hToolTip, TTM_ADDTOOL, 0L, (LPARAM)&toolInfo);

    return hToolTip;
}

_Success_(return)
BOOL UpdateToolTipText(
    HWND hDlg, int iControl, HWND hToolTip, _In_ TCHAR *newText)
{
    HWND hControl = GetDlgItem(hDlg, iControl);
    if (!hControl)
        return FALSE;

    TOOLINFO toolInfo;

    toolInfo.cbSize = sizeof(TOOLINFO);
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.hwnd = hDlg;
    toolInfo.uId = (UINT_PTR)hControl;
    toolInfo.hinst = g_propSheet.hInstance;
    toolInfo.lpszText = newText;
    toolInfo.lParam = 0;

    SendMessage(hToolTip, TTM_UPDATETIPTEXT, 0L, (LPARAM)&toolInfo);

    return TRUE;
}

/* Converts a sequence of numbers separated by spaces, such as "0 127 255", to
 * a byte array.
 * Returns the number of bytes stored.
 */
_Success_(return == cbSize)
DWORD StringDataToBytes(
    _In_reads_(cbSize) TCHAR *s,
    _Out_writes_all_(cbSize) BYTE *pData,
    DWORD cbSize)
{
    TCHAR *pNumStart = s;
    TCHAR *pAfterNum = NULL;
    unsigned int num;
    DWORD cbWritten = 0;

    while (cbWritten < cbSize)
    {
        num = ttoui(pNumStart, &pAfterNum);
        if (pNumStart == pAfterNum)
        {
            /* No number read */
            break;
        }

        if (num > 0xFF)
        {
            /* Invalid data, bigger than a byte */
            break;
        }

        pData[cbWritten++] = (BYTE)num;

        if (*pAfterNum == TEXT('\0') && cbWritten < cbSize)
        {
            /* Unexpected end of string */
            break;
        }

        pNumStart = pAfterNum;
        /* Skip whitespace */
        while (*pNumStart == TEXT(' '))
            pNumStart++;
    }

    return cbWritten;
}

_Success_(return != CLR_INVALID)
COLORREF ColorStringToColor(_In_ TCHAR *szColorData)
{
    BYTE rgb[3];
    if (StringDataToBytes(szColorData, rgb, 3) != 3)
        return CLR_INVALID;

    return RGB(rgb[0], rgb[1], rgb[2]);
}

#if !defined(MINIMAL)
/* Trims and removes special characters from the given string.
 */
void StrSanitize(_Inout_ TCHAR *str)
{
#define IsInvalidChar(c) ((c >= 0x01 && c <= 0x1F) || c == TEXT('\\'))
#define IsWhitespace(c) (c == TEXT(' ') || IsInvalidChar(c))

    /* Left trim */
    TCHAR *strTmp = str;
    while (IsWhitespace(*strTmp))
        strTmp++;

    /* Right trim */
    int len = lstrlen(strTmp);
    if (len > 0)
    {
        TCHAR *end = strTmp + len - 1;
        while (IsWhitespace(*end))
            end--;
        end[1] = TEXT('\0');
    }

    len = lstrlen(strTmp);
    size_t bufferSize = ((size_t)len + 1) * sizeof(TCHAR);
    TCHAR *buffer = Alloc(0, bufferSize);
    if (!buffer)
        return;

    TCHAR *bufferTmp = buffer;

    while (*strTmp)
    {
        /* Condense consecutive whitespaces into one */
        if (! (IsWhitespace(strTmp[0]) && IsWhitespace(strTmp[1])) )
        {
            /* Replace invalid chars by spaces */
            if (IsInvalidChar(*strTmp))
                *bufferTmp++ = TEXT(' ');
            else
                *bufferTmp++ = *strTmp;
        }

        strTmp++;
    }
    *bufferTmp = TEXT('\0');

    memcpy(str, buffer, bufferSize);

#undef IsWhitespace
#undef IsInvalidChar
}
#endif  /* !defined(MINIMAL) */

#if defined(WITH_THEMES)
/* Converts a relative path to an absolute one.
 * baseDir must be absolute.
 * chars includes the NULL terminator.
 */
_Success_(return)
BOOL GetAbsolutePath(_In_ const TCHAR *baseDir,
    _Inout_updates_(chars) TCHAR *path, DWORD chars)
{
    if (!PathIsRelative(path))
        return TRUE;

    TCHAR *origPath = Alloc(0, chars * sizeof(TCHAR));
    if (!origPath)
        return FALSE;

    BOOL ret = FALSE;

    if (!lstrcpy(origPath, baseDir))
        goto Cleanup;

    if (!PathRemoveFileSpec(origPath))
        goto Cleanup;

    int len = lstrlen(origPath);
    if (len + 1 + (DWORD)lstrlen(path) >= chars)
        goto Cleanup;

    origPath[len] = L'\\';
    if (!lstrcpy(&origPath[len + 1], path))
        goto Cleanup;

    if (!lstrcpy(path, origPath))
        goto Cleanup;

    ret = TRUE;

Cleanup:
    Free(origPath);
    return ret;
}
#endif

#if defined(WITH_CMDLINE)

_Success_(return)
BOOL SimulateEnterInput(void)
{
    INPUT inputs[2];
    memset(&inputs, 0, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_RETURN;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_RETURN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    return SendInput(_countof(inputs), inputs, sizeof(INPUT)) ==
        _countof(inputs);
}

_Success_(return)
static
BOOL PrintLine(TCHAR *text)
{
#if WINVER >= WINVER_XP
    if (!AttachConsole(ATTACH_PARENT_PROCESS))
    {
#endif
        int ret = MessageBox(NULL, text, TEXT("Themes"), MB_APPLMODAL | MB_OK);
        return ret != 0;
#if WINVER >= WINVER_XP
    }

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
    {
        FreeConsole();
        return FALSE;
    }

    /* Do not start writing to the right of the prompt */
    WriteConsole(hStdOut, TEXT(NEWLINE), lstrlenA(NEWLINE), NULL, NULL);

    BOOL ret = WriteConsole(hStdOut, text, lstrlen(text), NULL, NULL);

    FreeConsole();

    /* Show the prompt to avoid confusing the user */
    SimulateEnterInput();

    return ret;
#endif
}

/* Show the message of a resource in the standard output in a new line, and
 * ending with the prompt in the next line.
 */
_Success_(return)
BOOL PrintResource(int msgId)
{
    TCHAR *msg;
    if (!AllocAndLoadString(&msg, msgId))
        return FALSE;

    BOOL ret = PrintLine(msg);

    Free(msg);

    return ret;
}

_Success_(return)
BOOL PrintLastErrorMessage(void)
{
    DWORD errorId = GetLastError();
    if (errorId == 0)
        return TRUE;

    TCHAR *szMessage;

    DWORD chars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorId, 0, (TCHAR *)&szMessage, 0, NULL);

    if (chars < 1)
    {
        LocalFree(szMessage);
        return FALSE;
    }

    PrintLine(szMessage);

    LocalFree(szMessage);
    return TRUE;
}
#endif  /* defined(WITH_CMDLINE) */
