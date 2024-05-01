#pragma once
#if !defined(PREVIEW_H)
#define PREVIEW_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Preview messages */
#define PVM_SETSIZE            (WM_USER+1)
#define PVM_SETFONT            (WM_USER+2)
#define PVM_UPDATESCHEME       (WM_USER+3)

/* PVM_UPDATESCHEME flags */
#define PV_UPDATE_ONLY_COLORS        0x1L
#if WINVER >= WINVER_XP
#define PV_UPDATE_EXPLORER_SETTINGS  0x2L
#endif

#if defined(WITH_THEMES)
_Success_(return)
BOOL RegisterThemePreviewControl(void);

void UnregisterThemePreviewControl(void);
#endif

_Success_(return)
BOOL RegisterSchemePreviewControl(void);

void UnregisterSchemePreviewControl(void);

#endif  /* !defined(PREVIEW_H) */
