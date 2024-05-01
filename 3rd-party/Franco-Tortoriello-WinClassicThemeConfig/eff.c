/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Handling of visual effects
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if !defined(MINIMAL)

#include "eff.h"

#include "reg.h"
#include "mincrt.h"

static const TCHAR szMetricsKey[] =
    TEXT("Control Panel\\Desktop\\WindowMetrics");

static const TCHAR szIconSizeValue[] =
    TEXT("Shell Icon Size");

#if WINVER >= WINVER_XP
static const TCHAR szExplorerAdvKey[] =
    TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");

static const TCHAR szIconLabelShadowValue[] =
    TEXT("ListviewShadow");

#if WINVER >= WINVER_VISTA
static const TCHAR szTransSelRectValue[] =
    TEXT("ListviewAlphaSelect");
#endif
#endif

static const EFFECTS g_defaults = {
    /* iconSize    */ 32,

    /* Spacing including iconSize, as returned by SystemParametersInfo().
     * Defaults to 61 on Windows 11.
     */
    /* iconXMargin */ 43,
    /* iconYMargin */ 43,

#if WINVER >= WINVER_XP
    /* labelShadow  */ TRUE,
#endif
#if WINVER >= WINVER_VISTA
    /* transSelRect */ TRUE,
#endif

#if WINVER >= WINVER_2K
    /* effects       */ TRUE,
#endif
#if WINVER >= WINVER_XP
    /* menuShadow    */ TRUE,  /* FALSE on Server */
#if WINVER >= WINVER_8
    /* pointerShadow */ FALSE,
#else
    /* pointerShadow */ TRUE,
#endif
#endif  /* WINVER >= WINVER_XP */

    /* All animations default to FALSE on Server */
#if WINVER >= WINVER_2K
    /* menuOpenAnim     */ TRUE,
    /* menuOpenAnimFade */ TRUE,
    /* menuSelAnim      */ TRUE,
    /* toolTipAnim      */ TRUE,
    /* toolTipAnimFade  */ TRUE,
    /* comboBoxAnim     */ TRUE,
    /* listBoxAnim      */ TRUE,
#endif
    /* winAnim      */ TRUE,
#if WINVER >= WINVER_VISTA
    /* controlAnim  */ TRUE,
#endif

#if WINVER >= WINVER_2K
    /* dragFullWin       */ TRUE,  /* FALSE on Server */
#endif
#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
    /* fontSmoothing     */ TRUE,
#endif
#if WINVER >= WINVER_XP
    /* fontSmoothingType */ FE_FONTSMOOTHINGCLEARTYPE,
#endif

#if WINVER >= WINVER_2K
    /* keyboardCues        */ FALSE,
#if !defined(WINVER_IS_98)
    /* rightAlignPopupMenu */ FALSE,
#endif
    /* highContrast        */ FALSE,
#endif
};

EFFECTS g_currentEffects;
EFFECTS g_appliedEffects;

_Success_(return)
static
BOOL ApplyIconSize(int size)
{
    TCHAR szSize[3];
    if (wsprintf(szSize, TEXT("%d"), size) != 2)
        return FALSE;

    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szMetricsKey, 0,
        KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    status = RegSetValueEx(hKey, szIconSizeValue, 0, REG_SZ,
        (BYTE *)szSize, sizeof(szSize));

    RegCloseKey(hKey);

    return status == ERROR_SUCCESS;
}

#if WINVER >= WINVER_XP
_Success_(return)
static
BOOL ApplyExplorerSettings(void)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szExplorerAdvKey, 0,
        KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL ret = TRUE;

    if (g_currentEffects.labelShadow != g_appliedEffects.labelShadow)
    {
        if (RegSetValueEx(hKey, szIconLabelShadowValue, 0, REG_DWORD,
            (BYTE *)&g_currentEffects.labelShadow, sizeof(BOOL)) !=
            ERROR_SUCCESS)
        {
            ret = FALSE;
            g_currentEffects.labelShadow = g_appliedEffects.labelShadow;
        }
    }

#if WINVER >= WINVER_VISTA
    if (g_currentEffects.transSelRect !=
        g_appliedEffects.transSelRect)
    {
        if (RegSetValueEx(hKey, szTransSelRectValue, 0, REG_DWORD,
            (BYTE *)&g_currentEffects.transSelRect, sizeof(BOOL)) !=
            ERROR_SUCCESS)
        {
            ret = FALSE;
            g_currentEffects.transSelRect =
                g_appliedEffects.transSelRect;
        }
    }
#endif

    RegCloseKey(hKey);

    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE,
        0L, 0L, SMTO_ABORTIFHUNG, 30000, NULL);
    return ret;
}
#endif  /* WINVER >= WINVER_XP */

_Success_(return)
static
BOOL LoadWinAnimations(void)
{
    ANIMATIONINFO info;
    info.cbSize = sizeof(ANIMATIONINFO);

    if (!SystemParametersInfo(
        SPI_GETANIMATION, sizeof(ANIMATIONINFO), &info, 0))
    {
        return FALSE;
    }

    g_currentEffects.winAnim = (info.iMinAnimate != 0);
    return TRUE;
}

_Success_(return)
static
BOOL ApplyWinAnimations(void)
{
    ANIMATIONINFO info;
    info.cbSize = sizeof(ANIMATIONINFO);
    info.iMinAnimate = g_currentEffects.winAnim;

    return SystemParametersInfo(
        SPI_SETANIMATION, sizeof(ANIMATIONINFO), &info, 0);
}

/* Populates g_appliedEffects with current system settings, or default values
 * if there is an error.
 */
void LoadCurrentSettings(void)
{
#define LoadValue(action, var, member) \
    if (!SystemParametersInfo(action, 0, &var, 0)) \
        var = g_defaults.member; \
    g_currentEffects.member = var

    int iValue;

    if (!LoadIconSize())
        g_currentEffects.iconSize = g_defaults.iconSize;
    LoadValue(SPI_ICONHORIZONTALSPACING, iValue, iconXMargin);
    LoadValue(SPI_ICONVERTICALSPACING,   iValue, iconYMargin);
    g_currentEffects.iconXMargin -= g_currentEffects.iconSize;
    g_currentEffects.iconYMargin -= g_currentEffects.iconSize;

#if WINVER >= WINVER_XP
    g_currentEffects.labelShadow = g_defaults.labelShadow;
#if WINVER >= WINVER_VISTA
    g_currentEffects.transSelRect = g_defaults.transSelRect;
#endif
    LoadExplorerSettings();
#endif

#if WINVER >= WINVER_2K
    BOOL bValue;

    LoadValue(SPI_GETUIEFFECTS, bValue, effects);
#if WINVER >= WINVER_XP
    LoadValue(SPI_GETDROPSHADOW, bValue, menuShadow);
    LoadValue(SPI_GETCURSORSHADOW, bValue, pointerShadow);
#endif

    LoadValue(SPI_GETMENUANIMATION, bValue, menuOpenAnim);
    LoadValue(SPI_GETMENUFADE, bValue, menuOpenAnimFade);
    LoadValue(SPI_GETSELECTIONFADE, bValue, menuSelAnim);
    LoadValue(SPI_GETTOOLTIPANIMATION, bValue, toolTipAnim);
    LoadValue(SPI_GETTOOLTIPFADE, bValue, toolTipAnimFade);
    LoadValue(SPI_GETCOMBOBOXANIMATION, bValue, comboBoxAnim);
    LoadValue(SPI_GETLISTBOXSMOOTHSCROLLING, bValue, listBoxAnim);
#endif  /* WINVER >= WINVER_2K */

    /* These are independent from UIEFFECTS */
    if (!LoadWinAnimations())
        g_currentEffects.winAnim = g_defaults.winAnim;

#if WINVER >= WINVER_VISTA
    LoadValue(SPI_GETCLIENTAREAANIMATION, bValue, controlAnim);
#endif

#if WINVER >= WINVER_2K
    LoadValue(SPI_GETDRAGFULLWINDOWS, bValue, dragFullWin);

#if !defined(WINVER_IS_98)
    LoadValue(SPI_GETFONTSMOOTHING, bValue, fontSmoothing);
#endif
#if WINVER >= WINVER_XP
    UINT uiValue;
    LoadValue(SPI_GETFONTSMOOTHINGTYPE, uiValue, fontSmoothingType);
#endif

    LoadValue(SPI_GETKEYBOARDCUES, bValue, keyboardCues);
#if !defined(WINVER_IS_98)
    LoadValue(SPI_GETMENUDROPALIGNMENT, bValue, rightAlignPopupMenu);
#endif

    if (!LoadHighContrastMode())
        g_currentEffects.highContrast = g_defaults.highContrast;
#endif  /* WINVER >= WINVER_2K */

#undef LoadValue

    g_appliedEffects = g_currentEffects;
}

void LoadDefaultEffects(void)
{
    g_currentEffects = g_defaults;
}

/* Applies only the settings which were changed.
 * Returns if any setting was applied.
 */
BOOL ApplySettings(void)
{
    BOOL applied = FALSE;

#define Restore(member) \
    g_currentEffects.member = g_appliedEffects.member

#define Changed(member) \
    g_currentEffects.member != g_appliedEffects.member

#define _ApplyUiParam(action, member) \
    if (SystemParametersInfo(action, g_currentEffects.member, NULL, \
            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) \
        applied = TRUE; \
    else \
        Restore(member)

#define _ApplyPvParam(action, member) \
    if (SystemParametersInfo(action, 0, IntToPtr(g_currentEffects.member), \
            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) \
        applied = TRUE; \
    else \
        Restore(member)

#define ApplyUiParam(action, member) \
    if (Changed(member)) { _ApplyUiParam(action, member); }

#define ApplyPvParam(action, member) \
    if (Changed(member)) { _ApplyPvParam(action, member); }

#define ApplyIconMargin(action, member) \
    if (Changed(member) || Changed(iconSize)) { \
        if (SystemParametersInfo(action, \
                g_currentEffects.member + g_currentEffects.iconSize, \
                0L, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE)) \
            applied = TRUE; \
        else \
            Restore(member); \
    }

    if (Changed(iconSize))
    {
        if (ApplyIconSize(g_currentEffects.iconSize))
            applied = TRUE;
        else
            Restore(iconSize);
    }

    ApplyIconMargin(SPI_ICONHORIZONTALSPACING, iconXMargin);
    ApplyIconMargin(SPI_ICONVERTICALSPACING,   iconYMargin);
    if (Changed(iconSize) || Changed(iconXMargin) || Changed(iconYMargin))
    {
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE,
            SPI_SETNONCLIENTMETRICS, 0L, SMTO_ABORTIFHUNG, 30000, NULL);
    }

#if WINVER >= WINVER_XP
    if (Changed(labelShadow)
#if WINVER >= WINVER_VISTA
        || Changed(transSelRect)
#endif
    )
    {
        if (ApplyExplorerSettings())
            applied = TRUE;
    }
#endif

#if WINVER >= WINVER_2K
    if (Changed(effects))
#endif
    {
#if WINVER >= WINVER_2K
        _ApplyPvParam(SPI_SETUIEFFECTS, effects);
#endif

#if WINVER >= WINVER_VISTA
        /* Workaround Vista+ bug where DWM shadows are not changed */
        _ApplyPvParam(SPI_SETDROPSHADOW, menuShadow);
        g_appliedEffects.menuShadow = g_currentEffects.menuShadow;
#endif

#if WINVER >= WINVER_2K
        if (!g_currentEffects.effects)
        {
            /* Disable these too for consistency */
            g_currentEffects.winAnim = FALSE;
#if WINVER >= WINVER_VISTA
            g_currentEffects.controlAnim = FALSE;
#endif
        }
#endif
    }
#if WINVER >= WINVER_VISTA
    else
#endif
    {
#if WINVER >= WINVER_XP
        ApplyPvParam(SPI_SETDROPSHADOW, menuShadow);
#endif
    }

#if WINVER >= WINVER_XP
    ApplyPvParam(SPI_SETCURSORSHADOW, pointerShadow);
#endif

#if WINVER >= WINVER_2K
    ApplyPvParam(SPI_SETMENUANIMATION, menuOpenAnim);
    ApplyPvParam(SPI_SETMENUFADE, menuOpenAnimFade);
    ApplyPvParam(SPI_SETSELECTIONFADE, menuSelAnim);
    ApplyPvParam(SPI_SETTOOLTIPANIMATION, toolTipAnim);
    ApplyPvParam(SPI_SETTOOLTIPFADE, toolTipAnimFade);
    ApplyPvParam(SPI_SETCOMBOBOXANIMATION, comboBoxAnim);
    ApplyPvParam(SPI_SETLISTBOXSMOOTHSCROLLING, listBoxAnim);
#endif

    if (ApplyWinAnimations())
        applied = TRUE;
    else
        Restore(winAnim);
#if WINVER >= WINVER_VISTA
    ApplyPvParam(SPI_SETCLIENTAREAANIMATION, controlAnim);
#endif

#if WINVER >= WINVER_2K
    ApplyUiParam(SPI_SETDRAGFULLWINDOWS, dragFullWin);

#if !defined(WINVER_IS_98)
    ApplyUiParam(SPI_SETFONTSMOOTHING, fontSmoothing);
#endif
#if WINVER >= WINVER_XP
    if (g_currentEffects.fontSmoothing &&
        g_currentEffects.fontSmoothingType > 0)
    {
        ApplyPvParam(SPI_SETFONTSMOOTHINGTYPE, fontSmoothingType);
    }
#endif

    ApplyPvParam(SPI_SETKEYBOARDCUES, keyboardCues);
#if !defined(WINVER_IS_98)
    ApplyUiParam(SPI_SETMENUDROPALIGNMENT, rightAlignPopupMenu);
#endif

    if (Changed(highContrast))
    {
        if (ApplyHighContrastMode(g_currentEffects.highContrast))
            applied = TRUE;
        else
            Restore(highContrast);
    }
#endif  /* WINVER >= WINVER_2K */

#undef Changed
#undef _ApplyUiParam
#undef _ApplyPvParam
#undef ApplyUiParam
#undef ApplyPvParam
#undef ApplyIconMargin

    if (applied)
        g_appliedEffects = g_currentEffects;

    return applied;
}

_Success_(return)
BOOL LoadIconSize(void)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szMetricsKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    TCHAR szSize[3];
    DWORD cbData = sizeof(szSize);
    status = RegQueryString(hKey, szIconSizeValue, szSize, &cbData);
    if (status == ERROR_SUCCESS)
        g_currentEffects.iconSize = (int)ttoui(szSize, NULL);
    else
        return FALSE;

    RegCloseKey(hKey);
    return TRUE;
}

#if WINVER >= WINVER_XP
_Success_(return)
BOOL LoadExplorerSettings(void)
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, szExplorerAdvKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return FALSE;

    BOOL ret = TRUE;

    g_currentEffects.labelShadow = RegQueryBool(hKey, szIconLabelShadowValue,
        g_defaults.labelShadow);

#if WINVER >= WINVER_VISTA
    g_currentEffects.transSelRect = RegQueryBool(hKey, szTransSelRectValue,
        g_defaults.transSelRect);
#endif

    RegCloseKey(hKey);
    return ret;
}
#endif  /* WINVER >= WINVER_XP */

#if WINVER >= WINVER_2K
_Success_(return)
BOOL LoadHighContrastMode(void)
{
    HIGHCONTRAST hc;
    hc.cbSize = sizeof(HIGHCONTRAST);

    if (!SystemParametersInfo(
        SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0))
    {
       return FALSE;
    }

    g_currentEffects.highContrast = ((hc.dwFlags & HCF_HIGHCONTRASTON) != 0);
    return TRUE;
}

_Success_(return)
BOOL ApplyHighContrastMode(BOOL bEnable)
{
    HIGHCONTRAST hc;
    hc.cbSize = sizeof(HIGHCONTRAST);

    if (!SystemParametersInfo(
        SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0))
    {
        return FALSE;
    }

    if (bEnable == ((hc.dwFlags & HCF_HIGHCONTRASTON) != 0))
    {
        /* No change */
        return TRUE;
    }

    if (bEnable)
        hc.dwFlags |= HCF_HIGHCONTRASTON;
    else
        hc.dwFlags &= ~HCF_HIGHCONTRASTON;
    hc.dwFlags |= HCF_OPTION_NOTHEMECHANGE;

    return SystemParametersInfo(SPI_SETHIGHCONTRAST, sizeof(HIGHCONTRAST),
        &hc, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}
#endif  /* WINVER >= WINVER_2K */

#endif  /* !defined(MINIMAL) */
