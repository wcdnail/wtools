#pragma once
#if !defined(UTIL_H)
#define UTIL_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

_Success_(return > 0)
int AllocAndLoadString(_Out_ TCHAR **pTarget, UINT id);

int ShowMessageFromResource(HWND hWnd, int msgId, int titleMsgId, UINT type);

_Success_(return != 0)
LRESULT GetComboBoxSelectedData(HWND hCombo);

_Success_(return)
BOOL SelectComboByItemData(HWND hCombo, LRESULT itemData);

void SetUpDownOnRange(HWND hUpDown);

_Success_(return != NULL)
HWND CreateToolTip(HWND hDlg, int iControl, int ids);

_Success_(return)
BOOL UpdateToolTipText(
    HWND hDlg, int iControl, HWND hToolTip, _In_ TCHAR *newText);

_Success_(return == cbSize)
DWORD StringDataToBytes(
    _In_reads_(cbSize) TCHAR *s,
    _Out_writes_all_(cbSize) BYTE *pData,
    DWORD cbSize);

_Success_(return != CLR_INVALID)
COLORREF ColorStringToColor(_In_ TCHAR *szColorData);

#if !defined(MINIMAL)
void StrSanitize(_Inout_ TCHAR *str);
#endif

#if defined(WITH_THEMES)
_Success_(return)
BOOL GetAbsolutePath(_In_ const TCHAR *baseDir,
    _Inout_updates_(chars) TCHAR *path, DWORD chars);
#endif

#if defined(WITH_CMDLINE)
_Success_(return)
BOOL PrintResource(int msgId);

_Success_(return)
BOOL PrintLastErrorMessage(void);

_Success_(return)
BOOL SimulateEnterInput(void);
#endif  /* defined(WITH_CMDLINE) */

#endif  /* !defined(UTIL_H) */
