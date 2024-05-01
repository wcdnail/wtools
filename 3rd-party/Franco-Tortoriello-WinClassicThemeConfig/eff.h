#pragma once
#if !defined(WNDEFF_H)
#define WNDEFF_H

#include "config.h"

#if !defined(MINIMAL)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct tagEFFECTS
{
    int iconSize;
    int iconXMargin; /* Without including iconSize */
    int iconYMargin;

#if WINVER >= WINVER_XP
    BOOL labelShadow;
#endif
#if WINVER >= WINVER_VISTA
    BOOL transSelRect;
#endif

#if WINVER >= WINVER_2K
    BOOL effects;  /* Master setting */
#endif
#if WINVER >= WINVER_XP
    BOOL menuShadow;  /* On 2000 it applies but with no visible effect */
    BOOL pointerShadow;
#endif

#if WINVER >= WINVER_2K
    BOOL menuOpenAnim;
    BOOL menuOpenAnimFade;
    BOOL menuSelAnim;
    BOOL toolTipAnim;
    BOOL toolTipAnimFade;
    BOOL comboBoxAnim;
    BOOL listBoxAnim;
#endif
    BOOL winAnim;
#if WINVER >= WINVER_VISTA
    BOOL controlAnim;
#endif

#if WINVER >= WINVER_2K
    BOOL dragFullWin;
#endif
#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
    BOOL fontSmoothing;
#endif
#if WINVER >= WINVER_XP
    UINT fontSmoothingType;
#endif

#if WINVER >= WINVER_2K
    BOOL keyboardCues;
#if !defined(WINVER_IS_98)
    BOOL rightAlignPopupMenu;  /* On 9x it applies but with no visible effect */
#endif
    BOOL highContrast;  /* On 95 it changes the scheme too */
#endif
} EFFECTS;

extern EFFECTS g_currentEffects;
extern EFFECTS g_appliedEffects;

void LoadCurrentSettings(void);
void LoadDefaultEffects(void);
BOOL ApplySettings(void);

_Success_(return)
BOOL LoadIconSize(void);

#if WINVER >= WINVER_XP
_Success_(return)
BOOL LoadExplorerSettings(void);
#endif

#if WINVER >= WINVER_2K
_Success_(return)
BOOL LoadHighContrastMode(void);

_Success_(return)
BOOL ApplyHighContrastMode(BOOL bEnable);
#endif

#endif  /* !defined(MINIMAL) */
#endif  /* !defined(WNDEFF_H) */
