/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Effects command line handling
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */
#include "pch.h"
#include "app.h"

#if defined(WITH_CMDLINE) && !defined(MINIMAL)

#include "eff.h"

/* All of these functions return if there were any changes */

static
BOOL EnableEffects(BOOL bEnable)
{
    LoadCurrentSettings();

#if WINVER >= WINVER_XP
    g_currentEffects.labelShadow = bEnable;
#endif
#if WINVER >= WINVER_VISTA
    g_currentEffects.transSelRect = bEnable;
#endif

#if WINVER >= WINVER_XP
    g_currentEffects.effects = bEnable;
#endif

    if (bEnable)
    {
#if WINVER >= WINVER_XP
        g_currentEffects.menuShadow = bEnable;
        g_currentEffects.pointerShadow = bEnable;
#endif

#if WINVER >= WINVER_XP
        g_currentEffects.menuOpenAnim = bEnable;
        g_currentEffects.menuSelAnim = bEnable;
        g_currentEffects.toolTipAnim = bEnable;
        g_currentEffects.comboBoxAnim = bEnable;
        g_currentEffects.listBoxAnim = bEnable;
#endif

        g_currentEffects.winAnim = bEnable;
#if WINVER >= WINVER_VISTA
        g_currentEffects.controlAnim = bEnable;
#endif
    }

#if WINVER >= WINVER_XP
    g_currentEffects.dragFullWin = bEnable;
#if !defined(WINVER_IS_98)
    g_currentEffects.fontSmoothing = bEnable;
#endif
#endif

    if (ApplySettings())
        return RETURN_CHANGES;
    else
        return RETURN_NO_CHANGES;
}

#if WINVER >= WINVER_XP
static
BOOL EnableHighContrast(BOOL bEnable)
{
    LoadHighContrastMode();

    if (g_currentEffects.highContrast == bEnable)
        return FALSE;

    ApplyHighContrastMode(bEnable);
    return TRUE;
}
#endif

static
BOOL RestoreDefaults(void)
{
    LoadCurrentSettings();
    LoadDefaultEffects();
    return ApplySettings();
}

UINT ParseEffectsCommandLine(int argc, _In_ WCHAR **argv)
{
    if (*argv[0] != L'/')
        return RETURN_USAGE;

    if (argc != 1)
        return RETURN_USAGE;

    WCHAR *action = &argv[0][1];

    if (lstrcmpiW(action, L"EnableEffects") == 0)
        return EnableEffects(TRUE);

    if (lstrcmpiW(action, L"DisableEffects") == 0)
        return EnableEffects(FALSE);

#if WINVER >= WINVER_XP
    if (lstrcmpiW(action, L"EnableHighContrast") == 0)
        return EnableHighContrast(TRUE);

    if (lstrcmpiW(action, L"DisableHighContrast") == 0)
        return EnableHighContrast(FALSE);
#endif

    if (lstrcmpiW(action, L"RestoreDefaults") == 0)
        return RestoreDefaults();

    return RETURN_USAGE;
}

#endif  /* defined(WITH_CMDLINE) && !defined(MINIMAL) && WINVER >= WINVER_2K */
