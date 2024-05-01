#pragma once
#if !defined(WNDCLAS_H)
#define WNDCLAS_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Indexes of the elements of the combo box list */
enum element
{
    ELEMENT_DESKTOP,
    ELEMENT_APPBACKGROUND,
    ELEMENT_WINDOW,
    ELEMENT_MENU,             /* The width affects the icon and caption buttons
                               * of MDI children */
    ELEMENT_ACTIVECAPTION,    /* Title bar of active windows */
    ELEMENT_INACTIVECAPTION,  /* Title bar of inactive windows */
    ELEMENT_SMCAPTION,        /* Title bar of palette (small) windows */
    ELEMENT_ACTIVEBORDER,     /* Border of active resizable windows */
    ELEMENT_INACTIVEBORDER,   /* Border of inactive resizable windows */
    ELEMENT_SCROLLBAR,
    ELEMENT_3DOBJECT,
    ELEMENT_3DSHADOW,         /* Not in official applets */
    ELEMENT_3DLIGHT,          /* Not in official applets */
    ELEMENT_SELECTEDITEM,     /* Also used for text selection */
    ELEMENT_DISABLEDITEM,
    ELEMENT_TOOLTIP,
    ELEMENT_MSGBOX,
#if WINVER >= WINVER_2K
    ELEMENT_HYPERLINK,
#endif
#if WINVER >= WINVER_XP
    ELEMENT_FLATMENUBAR,
#endif
#if WINVER >= WINVER_VISTA
/* Border of windows, including property sheets for some reason.
 * If SIZE_PADDEDBORDER is > 0, the windows with fixed borders affected also
 * include SIZE_BORDER
 */
    ELEMENT_PADDEDBORDER,
#endif
    NUM_ELEMENTS
};

BOOL AddSchemesToCombo(
    HWND hCombo, _In_ const SCHEME *pSchemes, _In_opt_ const SCHEME *pSelected);

/* In dlgcsave.c */
_Success_(return)
BOOL SanitizeNewName(HWND hWnd, _Out_writes_(SCHEMENAME_MAX_SIZE) TCHAR *name);

_Success_(return)
BOOL SanitizeNewSize(HWND hWnd, _Out_writes_(SIZENAME_MAX_SIZE) TCHAR *name);

#endif  /* !defined(WNDCLAS_H) */
