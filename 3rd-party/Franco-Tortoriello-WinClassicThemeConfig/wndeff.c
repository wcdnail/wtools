/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Effects property page
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if !defined(MINIMAL)

#include "eff.h"
#include "resource.h"
#include "util.h"

#include <CommCtrl.h>
#include <shellapi.h>

/* Implemented in dlgcsave.c */
INT_PTR CALLBACK AnimationsDlgProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct tagEFF_RANGE
{
    int min;
    int max;
    int def;  /* System default */
    int udControl;
} EFF_RANGE;

static HWND g_hDlg;
static BOOL g_ready = FALSE;

#define UpdateCheckBox(iControl, member) \
    SendDlgItemMessage(g_hDlg, iControl, \
        BM_SETCHECK, (WPARAM)g_currentEffects.member, 0L)

#define UpdateUpDown(iControl, member) \
    SendDlgItemMessage(g_hDlg, iControl, \
        UDM_SETPOS, 0L, (LPARAM)g_currentEffects.member)

static
void SetRanges(void)
{
#define SetRange(iControl, min, max) \
    SendDlgItemMessage(g_hDlg, iControl, UDM_SETRANGE, 0L, MAKELONG(max, min))

    SetRange(IDC_EFF_ICONS_SIZE_UD, 16, 72);
    SetRange(IDC_EFF_ICONS_XMARGIN_UD, 0, 150);
    SetRange(IDC_EFF_ICONS_YMARGIN_UD, 0, 150);

#undef SetRanges
}

#if WINVER >= WINVER_XP
static
void LoadFontSmoothingCombo(void)
{
    HWND hCombo = GetDlgItem(g_hDlg, IDC_EFF_FONTSMOOTHING_TYPE);
    if (!hCombo)
        return;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    TCHAR text[20];
    int index;

#define AddSmoothingCombo(ids, data) \
    if (LoadString(g_propSheet.hInstance, ids, text, 19) > 0) \
    { \
        index = (int)SendMessage(hCombo, CB_ADDSTRING, 0L, (LPARAM)text); \
        if (index >= 0) \
            SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)index, (LPARAM)data); \
    }

    AddSmoothingCombo(IDS_FONTSMOOTHING_DEFAULT, FE_FONTSMOOTHINGSTANDARD);
    AddSmoothingCombo(IDS_FONTSMOOTHING_CT,      FE_FONTSMOOTHINGCLEARTYPE);

#undef AddSmoothingCombo
}
#endif

#if WINVER >= WINVER_2K
static
void EnableEffectsControls(void)
{
#define EnableCtrl(iControl) \
    EnableWindow(GetDlgItem(g_hDlg, iControl), g_currentEffects.effects)

#if WINVER >= WINVER_XP
    EnableCtrl(IDC_EFF_MENUSHADOW);
    EnableCtrl(IDC_EFF_POINTERSHADOW);
#endif
    EnableCtrl(IDC_EFF_ANIMATIONS);

#undef EnableCtrl
}
#endif

static
void UpdateControls(void)
{
    g_ready = FALSE;

    UpdateUpDown(IDC_EFF_ICONS_SIZE_UD,    iconSize);
    UpdateUpDown(IDC_EFF_ICONS_XMARGIN_UD, iconXMargin);
    UpdateUpDown(IDC_EFF_ICONS_YMARGIN_UD, iconYMargin);

#if WINVER >= WINVER_XP
    UpdateCheckBox(IDC_EFF_ICONS_LABELSHADOW,  labelShadow);
#endif
#if WINVER >= WINVER_VISTA
    UpdateCheckBox(IDC_EFF_ICONS_TRANSSELRECT, transSelRect);
#endif

#if WINVER >= WINVER_2K
    UpdateCheckBox(IDC_EFF_MASTER, effects);
    EnableEffectsControls();
#endif
#if WINVER >= WINVER_XP
    UpdateCheckBox(IDC_EFF_MENUSHADOW,    menuShadow);
    UpdateCheckBox(IDC_EFF_POINTERSHADOW, pointerShadow);
#endif

#if WINVER >= WINVER_2K
    UpdateCheckBox(IDC_EFF_DRAGFULLWIN, dragFullWin);
#endif
#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
    UpdateCheckBox(IDC_EFF_FONTSMOOTHING, fontSmoothing);
#endif

#if WINVER >= WINVER_XP
    HWND hCombo = GetDlgItem(g_hDlg, IDC_EFF_FONTSMOOTHING_TYPE);
    EnableWindow(hCombo, g_currentEffects.fontSmoothing);
    if (!(g_currentEffects.fontSmoothing && SelectComboByItemData(hCombo,
        (LRESULT)g_currentEffects.fontSmoothingType)))
    {
        SendMessage(hCombo, CB_SETCURSEL, 0L, 0L);
    }
#endif

#if WINVER >= WINVER_2K
    UpdateCheckBox(IDC_EFF_KEYBOARDCUES, keyboardCues);
#if !defined(WINVER_IS_98)
    UpdateCheckBox(IDC_EFF_RIGHTALIGNMENU, rightAlignPopupMenu);
#endif
    UpdateCheckBox(IDC_EFF_HIGHCONTRAST, highContrast);
#endif

    g_ready = TRUE;
}

#if WINVER >= WINVER_2K
static
void CreateToolTips(void)
{
#if WINVER >= WINVER_XP
    CreateToolTip(g_hDlg, IDC_EFF_MENUSHADOW,     IDS_EFF_MENUSHADOW_TT);
#endif
    CreateToolTip(g_hDlg, IDC_EFF_KEYBOARDCUES,   IDS_EFF_KEYBOARDCUES_TT);
#if !defined(WINVER_IS_98)
    CreateToolTip(g_hDlg, IDC_EFF_RIGHTALIGNMENU, IDS_EFF_RIGHTALIGNMENU_TT);
#endif
    CreateToolTip(g_hDlg, IDC_EFF_HIGHCONTRAST,   IDS_EFF_HIGHCONTRAST_TT);
}
#endif

static
void InitPage(void)
{
    LoadCurrentSettings();
    SetRanges();

#if WINVER >= WINVER_XP
    LoadFontSmoothingCombo();
#endif

    UpdateControls();
#if WINVER >= WINVER_2K
    CreateToolTips();
#endif
}

static
void HandleButtonPress(WORD iControl)
{
#define GetChecked(iCheckBox) (SendDlgItemMessage( \
    g_hDlg, iCheckBox, BM_GETCHECK, 0L, 0L) == BST_CHECKED)

#define CaseCheck(iCaseControl, member) \
    case iCaseControl: \
        g_currentEffects.member = GetChecked(iControl); \
        break

    switch (iControl)
    {
#if WINVER >= WINVER_XP
    CaseCheck(IDC_EFF_ICONS_LABELSHADOW, labelShadow);
#endif
#if WINVER >= WINVER_VISTA
    CaseCheck(IDC_EFF_ICONS_TRANSSELRECT, transSelRect);
    case IDC_EFF_ICONS_CUSTOMIZE:
        ShellExecute(NULL, TEXT("open"),
            TEXT("rundll32.exe"),
            TEXT("shell32.dll,Control_RunDLL desk.cpl,Web,0"),
            NULL, SW_SHOWNORMAL);

        /* Do not set the property sheet as changed */
        return;
#endif

#if WINVER >= WINVER_2K
    case IDC_EFF_MASTER:
        g_currentEffects.effects = GetChecked(iControl);
        EnableEffectsControls();
        break;
#endif

#if WINVER >= WINVER_XP
    CaseCheck(IDC_EFF_MENUSHADOW, menuShadow);
    CaseCheck(IDC_EFF_POINTERSHADOW, pointerShadow);
#endif

    case IDC_EFF_ANIMATIONS:
        DialogBoxParam(
            g_propSheet.hInstance, MAKEINTRESOURCE(IDD_EFF_ANIM),
            g_hDlg, AnimationsDlgProc, (LPARAM)&g_currentEffects);
        break;

#if WINVER >= WINVER_2K
    CaseCheck(IDC_EFF_DRAGFULLWIN, dragFullWin);
#endif
#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
    case IDC_EFF_FONTSMOOTHING:
    {
        g_currentEffects.fontSmoothing = GetChecked(iControl);
#if WINVER >= WINVER_XP
        HWND hCombo = GetDlgItem(g_hDlg, IDC_EFF_FONTSMOOTHING_TYPE);
        EnableWindow(hCombo, g_currentEffects.fontSmoothing);
#endif
        break;
    }
#endif

#if WINVER >= WINVER_2K
    CaseCheck(IDC_EFF_KEYBOARDCUES, keyboardCues);
#if !defined(WINVER_IS_98)
    CaseCheck(IDC_EFF_RIGHTALIGNMENU, rightAlignPopupMenu);
#endif
    CaseCheck(IDC_EFF_HIGHCONTRAST, highContrast);
#endif

    case IDC_EFF_RESET:
        LoadDefaultEffects();
        UpdateControls();
        break;

    default:
        return;
    }

#undef GetChecked
#undef CaseCheck

    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

static
void HandleEditChange(WORD iControl)
{
#define GetUdPos(iUd) \
    (int)LOWORD(SendDlgItemMessage(g_hDlg, iUd, UDM_GETPOS, 0L, 0L))

    switch (iControl)
    {
    case IDC_EFF_ICONS_SIZE_E:
        g_currentEffects.iconSize = GetUdPos(IDC_EFF_ICONS_SIZE_UD);
        break;

    case IDC_EFF_ICONS_XMARGIN_E:
        g_currentEffects.iconXMargin = GetUdPos(IDC_EFF_ICONS_XMARGIN_UD);
        break;

    case IDC_EFF_ICONS_YMARGIN_E:
        g_currentEffects.iconYMargin = GetUdPos(IDC_EFF_ICONS_YMARGIN_UD);
        break;

    default:
        return;
    }

#undef GetUdPos

    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}

#if WINVER >= WINVER_XP
static
void HandleComboBoxSelChange(WORD iControl)
{
    HWND hCombo = GetDlgItem(g_hDlg, iControl);

    switch (iControl)
    {
    case IDC_EFF_FONTSMOOTHING_TYPE:
        g_currentEffects.fontSmoothingType =
            (UINT)GetComboBoxSelectedData(hCombo);
        break;

    default:
        return;
    }

    PropSheet_Changed(g_propSheet.hWnd, g_hDlg);
}
#endif

INT_PTR CALLBACK EffSettingsPageProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hDlg = hWnd;
        InitPage();
        return 0;

    case WM_COMMAND:
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            HandleButtonPress(LOWORD(wParam));
            break;

        case EN_CHANGE:
            /* Only process if this is a user change, not on init or when system
             * settings change.
             */
            if (g_ready)
                HandleEditChange(LOWORD(wParam));
            break;

#if WINVER >= WINVER_XP
        case CBN_SELCHANGE:
            HandleComboBoxSelChange(LOWORD(wParam));
            break;
#endif

        case EN_KILLFOCUS:
            /* Note: This assumes that the id of the UD follows the Edit one */
            SetUpDownOnRange(GetDlgItem(g_hDlg, LOWORD(wParam + 1)));
            break;
        }
        return 0;

    case WM_NOTIFY:
        switch (((NMHDR *)lParam)->code)
        {
        case PSN_APPLY:
            ApplySettings();
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG_PTR)PSNRET_NOERROR);
            return TRUE;

        case PSN_KILLACTIVE:
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG_PTR)FALSE);
            return TRUE;
        }
        return 0;

    case WM_SETTINGCHANGE:
        g_ready = FALSE;

#define GetNewSetting(action, member) \
    SystemParametersInfo(action, 0, &g_currentEffects.member, 0)

#define UpdateCheckBoxFromSys(action, member, iControl) \
    if (GetNewSetting(action, member)) { \
        UpdateCheckBox(iControl, member); \
        g_appliedEffects.member = g_currentEffects.member; \
    }

#define UpdateSettingFromSys(action, member) \
    if (GetNewSetting(action, member)) \
        g_appliedEffects.member = g_currentEffects.member

        switch (wParam)
        {
        case SPI_SETNONCLIENTMETRICS:
            if (!LoadIconSize())
                break;

            UpdateUpDown(IDC_EFF_ICONS_SIZE_UD, iconSize);
            g_appliedEffects.iconSize = g_currentEffects.iconSize;

            if (GetNewSetting(SPI_ICONHORIZONTALSPACING, iconXMargin))
            {
                g_currentEffects.iconXMargin -= g_currentEffects.iconSize;
                UpdateUpDown(IDC_EFF_ICONS_XMARGIN_UD, iconXMargin);
                g_appliedEffects.iconXMargin = g_currentEffects.iconXMargin;
            }

            if (GetNewSetting(SPI_ICONVERTICALSPACING, iconYMargin))
            {
                g_currentEffects.iconYMargin -= g_currentEffects.iconSize;
                UpdateUpDown(IDC_EFF_ICONS_YMARGIN_UD, iconYMargin);
                g_appliedEffects.iconYMargin = g_currentEffects.iconYMargin;
            }
            break;

        case SPI_ICONHORIZONTALSPACING:
            /* Handled in SPI_SETNONCLIENTMETRICS */
            break;

        case SPI_ICONVERTICALSPACING:
            /* Handled in SPI_SETNONCLIENTMETRICS */
            break;

#if WINVER >= WINVER_XP
        case 0:
            if (!LoadExplorerSettings())
                break;

            UpdateCheckBox(IDC_EFF_ICONS_LABELSHADOW, labelShadow);
            g_appliedEffects.labelShadow = g_currentEffects.labelShadow;

#if WINVER >= WINVER_VISTA
            UpdateCheckBox(IDC_EFF_ICONS_TRANSSELRECT, transSelRect);
            g_appliedEffects.transSelRect = g_currentEffects.transSelRect;
#endif

            break;
#endif

#if WINVER >= WINVER_2K
        case SPI_SETUIEFFECTS:
            UpdateCheckBoxFromSys(SPI_GETUIEFFECTS, effects,
                IDC_EFF_MASTER);
            EnableEffectsControls();
            break;

#if WINVER >= WINVER_XP
        case SPI_SETDROPSHADOW:
            UpdateCheckBoxFromSys(SPI_GETDROPSHADOW, menuShadow,
                IDC_EFF_MENUSHADOW);
            break;

        case SPI_SETCURSORSHADOW:
            UpdateCheckBoxFromSys(SPI_GETCURSORSHADOW, pointerShadow,
                IDC_EFF_POINTERSHADOW);
            break;
#endif

        case SPI_SETMENUANIMATION:
            UpdateSettingFromSys(SPI_GETMENUANIMATION, menuOpenAnim);
            break;

        case SPI_SETMENUFADE:
            UpdateSettingFromSys(SPI_GETMENUFADE, menuOpenAnimFade);
            break;

        case SPI_SETSELECTIONFADE:
            UpdateSettingFromSys(SPI_GETSELECTIONFADE, menuSelAnim);
            break;

        case SPI_SETTOOLTIPANIMATION:
            UpdateSettingFromSys(SPI_GETTOOLTIPANIMATION, toolTipAnim);
            break;

        case SPI_SETTOOLTIPFADE:
            UpdateSettingFromSys(SPI_GETTOOLTIPFADE, toolTipAnimFade);
            break;

        case SPI_SETCOMBOBOXANIMATION:
            UpdateSettingFromSys(SPI_GETCOMBOBOXANIMATION, comboBoxAnim);
            break;

        case SPI_SETLISTBOXSMOOTHSCROLLING:
            UpdateSettingFromSys(SPI_GETLISTBOXSMOOTHSCROLLING, listBoxAnim);
            break;
#endif

#if 0
        case SPI_SETANIMATION:
            /* Message never received */
            break;
#endif

#if WINVER >= WINVER_VISTA
        case SPI_SETCLIENTAREAANIMATION:
            UpdateSettingFromSys(SPI_GETCLIENTAREAANIMATION, controlAnim);
            break;
#endif

#if WINVER >= WINVER_2K
        case SPI_SETDRAGFULLWINDOWS:
            UpdateCheckBoxFromSys(SPI_GETDRAGFULLWINDOWS, dragFullWin,
                IDC_EFF_DRAGFULLWIN);
            break;

#if !defined(WINVER_IS_98)
        case SPI_SETFONTSMOOTHING:
            UpdateCheckBoxFromSys(SPI_GETFONTSMOOTHING, fontSmoothing,
                IDC_EFF_FONTSMOOTHING);
            break;
#endif

#if WINVER >= WINVER_XP
        case SPI_SETFONTSMOOTHINGTYPE:
            if (!GetNewSetting(SPI_GETFONTSMOOTHINGTYPE, fontSmoothingType))
                break;

            if (!SelectComboByItemData(
                GetDlgItem(g_hDlg, IDC_EFF_FONTSMOOTHING_TYPE),
                (LRESULT)g_currentEffects.fontSmoothingType))
            {
                break;
            }

            g_appliedEffects.fontSmoothingType =
                g_currentEffects.fontSmoothingType;
            break;
#endif

        case SPI_SETKEYBOARDCUES:
            UpdateCheckBoxFromSys(SPI_GETKEYBOARDCUES, keyboardCues,
                IDC_EFF_KEYBOARDCUES);
            break;

#if !defined(WINVER_IS_98)
        case SPI_SETMENUDROPALIGNMENT:
            UpdateCheckBoxFromSys(SPI_GETMENUDROPALIGNMENT, rightAlignPopupMenu,
                IDC_EFF_RIGHTALIGNMENU);
            break;
#endif

        case SPI_SETHIGHCONTRAST:
            if (!LoadHighContrastMode())
                break;

            UpdateCheckBox(IDC_EFF_HIGHCONTRAST, highContrast);
            g_appliedEffects.highContrast = g_currentEffects.highContrast;
            break;
#endif  /* WINVER >= WINVER_2K */

#if 0
        default:
        {
            TCHAR szAction[6];
            wsprintf(szAction, TEXT("Unhandled message: 0x%lx"), wParam);
            MessageBox(g_hDlg, szAction, TEXT("Debug"), MB_APPLMODAL | MB_OK);
        }
#endif
        }

#undef GetNewSetting
#undef UpdateCheckBoxFromSys
#undef UpdateSettingFromSys

        g_ready = TRUE;
        return 0;

    case WM_QUERYENDSESSION:
        return 0;

    case WM_CTLCOLORDLG:
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#undef UpdateCheckBox
#undef UpdateUpDown

#endif  /* !defined(MINIMAL) */
