/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Animations dialog
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"

#if !defined(MINIMAL)

#include "resource.h"
#include "eff.h"
#include "util.h"

static HWND g_hDlg;
static EFFECTS *g_pEffects;

#define UpdateCheckBox(iControl, member) \
    SendDlgItemMessage(g_hDlg, iControl, \
        BM_SETCHECK, (WPARAM)g_pEffects->member, 0L)

#if WINVER >= WINVER_2K
static
void LoadAnimationTypeCombo(int iControl)
{
    HWND hCombo = GetDlgItem(g_hDlg, iControl);
    if (!hCombo)
        return;

    SendMessage(hCombo, CB_RESETCONTENT, 0L, 0L);

    TCHAR text[20];
    int index;

#define AddTypeCombo(ids, data) \
    if (LoadString(g_propSheet.hInstance, ids, text, 19) > 0) \
    { \
        index = (int)SendMessage(hCombo, CB_ADDSTRING, 0L, (LPARAM)text); \
        if (index >= 0) \
            SendMessage(hCombo, CB_SETITEMDATA, (WPARAM)index, (LPARAM)data); \
    }

    AddTypeCombo(IDS_ANIM_SLIDE, 0);
    AddTypeCombo(IDS_ANIM_FADE,  1);

#undef AddTypeCombo
}
#endif

static
void InitDialog(void)
{
#if WINVER >= WINVER_2K
    HWND hCombo;

    LoadAnimationTypeCombo(IDC_ANIM_MENUOPEN_TYPE);
    LoadAnimationTypeCombo(IDC_ANIM_TOOLTIP_TYPE);

#if WINVER >= WINVER_VISTA
    CreateToolTip(g_hDlg, IDC_ANIM_MENUOPEN_TYPE, IDS_ANIM_TYPE_TT);
    CreateToolTip(g_hDlg, IDC_ANIM_TOOLTIP_TYPE, IDS_ANIM_TYPE_TT);
#endif

    UpdateCheckBox(IDC_ANIM_MENUOPEN, menuOpenAnim);

    hCombo = GetDlgItem(g_hDlg, IDC_ANIM_MENUOPEN_TYPE);
    EnableWindow(hCombo, g_pEffects->menuOpenAnim);
    if (!(g_currentEffects.menuOpenAnim && SelectComboByItemData(hCombo,
        (LRESULT)g_currentEffects.menuOpenAnimFade)))
    {
        SendMessage(hCombo, CB_SETCURSEL, 0L, 0L);
    }

    UpdateCheckBox(IDC_ANIM_MENUSEL,  menuSelAnim);

    UpdateCheckBox(IDC_ANIM_TOOLTIP,  toolTipAnim);

    hCombo = GetDlgItem(g_hDlg, IDC_ANIM_TOOLTIP_TYPE);
    EnableWindow(hCombo, g_pEffects->toolTipAnim);
    if (!(g_currentEffects.toolTipAnim && SelectComboByItemData(hCombo,
        (LRESULT)g_currentEffects.toolTipAnimFade)))
    {
        SendMessage(hCombo, CB_SETCURSEL, 0L, 0L);
    }

    UpdateCheckBox(IDC_ANIM_COMBOBOX, comboBoxAnim);
    UpdateCheckBox(IDC_ANIM_LISTBOX,  listBoxAnim);
#endif  /* WINVER >= WINVER_2K */
    UpdateCheckBox(IDC_ANIM_WINDOW,   winAnim);
#if WINVER >= WINVER_VISTA
    UpdateCheckBox(IDC_ANIM_CONTROL,  controlAnim);
#endif
}

static
void HandleButtonPress(WORD iControl)
{
#define GetChecked(iCheckBox) (SendDlgItemMessage( \
        g_hDlg, iCheckBox, BM_GETCHECK, 0L, 0L) == BST_CHECKED)

#define CaseCheck(iCaseControl, member) \
    case iCaseControl: \
        g_pEffects->member = GetChecked(iControl); \
        break

    switch (iControl)
    {
#if WINVER >= WINVER_2K
    case IDC_ANIM_MENUOPEN:
        g_pEffects->menuOpenAnim = GetChecked(iControl);
        EnableWindow(GetDlgItem(g_hDlg, IDC_ANIM_MENUOPEN_TYPE),
            g_pEffects->menuOpenAnim);
        break;

    CaseCheck(IDC_ANIM_MENUSEL, menuSelAnim);

    case IDC_ANIM_TOOLTIP:
        g_pEffects->toolTipAnim = GetChecked(iControl);
        EnableWindow(GetDlgItem(g_hDlg, IDC_ANIM_TOOLTIP_TYPE),
            g_pEffects->toolTipAnim);
        break;

    CaseCheck(IDC_ANIM_COMBOBOX, comboBoxAnim);
    CaseCheck(IDC_ANIM_LISTBOX, listBoxAnim);
#endif
    CaseCheck(IDC_ANIM_WINDOW, winAnim);
#if WINVER >= WINVER_VISTA
    CaseCheck(IDC_ANIM_CONTROL, controlAnim);
#endif

    case IDOK:
    case IDCANCEL:
        EndDialog(g_hDlg, (INT_PTR)TRUE);
        break;
    }

#undef CaseCheck
#undef GetChecked
}

#if WINVER >= WINVER_2K
static
void HandleComboBoxSelChange(WORD iControl)
{
    HWND hCombo = GetDlgItem(g_hDlg, iControl);

    switch (iControl)
    {
    case IDC_ANIM_MENUOPEN_TYPE:
        g_currentEffects.menuOpenAnimFade =
            (BOOL)GetComboBoxSelectedData(hCombo);
        break;

    case IDC_ANIM_TOOLTIP_TYPE:
        g_currentEffects.toolTipAnimFade =
            (BOOL)GetComboBoxSelectedData(hCombo);
        break;

    default:
        return;
    }
}
#endif

INT_PTR CALLBACK AnimationsDlgProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hDlg = hWnd;
        g_pEffects = (EFFECTS *)lParam;

        InitDialog();
        return TRUE;

    case WM_COMMAND:
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            HandleButtonPress(LOWORD(wParam));
            break;

#if WINVER >= WINVER_2K
        case CBN_SELCHANGE:
            HandleComboBoxSelChange(LOWORD(wParam));
            break;
#endif
        }
        return TRUE;

#if WINVER >= WINVER_2K
    case WM_SETTINGCHANGE:
#define GetNewSetting(action, member) \
    SystemParametersInfo(action, 0, &g_pEffects->member, 0)

#define UpdateCheckBoxFromSys(action, member, iControl) \
    if (GetNewSetting(action, member)) { \
        UpdateCheckBox(iControl, member); \
    }

        switch (wParam)
        {
        case SPI_SETMENUANIMATION:
            UpdateCheckBoxFromSys(SPI_GETMENUANIMATION, menuOpenAnim,
                IDC_ANIM_MENUOPEN);
            EnableWindow(GetDlgItem(g_hDlg, IDC_ANIM_MENUOPEN_TYPE),
                g_pEffects->menuOpenAnim);
            break;

        case SPI_SETMENUFADE:
            if (!GetNewSetting(SPI_GETMENUFADE, menuOpenAnimFade))
                break;

            SelectComboByItemData(GetDlgItem(g_hDlg, IDC_ANIM_MENUOPEN_TYPE),
                (LRESULT)g_currentEffects.menuOpenAnimFade);
            break;

        case SPI_SETSELECTIONFADE:
            UpdateCheckBoxFromSys(SPI_GETSELECTIONFADE, menuSelAnim,
                IDC_ANIM_MENUSEL);
            break;

        case SPI_SETTOOLTIPANIMATION:
            UpdateCheckBoxFromSys(SPI_GETTOOLTIPANIMATION, toolTipAnim,
                IDC_ANIM_TOOLTIP);
            EnableWindow(GetDlgItem(g_hDlg, IDC_ANIM_TOOLTIP_TYPE),
                g_pEffects->toolTipAnim);
            break;

        case SPI_SETTOOLTIPFADE:
            if (!GetNewSetting(SPI_GETTOOLTIPFADE, toolTipAnimFade))
                break;

            SelectComboByItemData(GetDlgItem(g_hDlg, IDC_ANIM_TOOLTIP_TYPE),
                (LRESULT)g_currentEffects.toolTipAnimFade);
            break;

        case SPI_SETCOMBOBOXANIMATION:
            UpdateCheckBoxFromSys(SPI_GETCOMBOBOXANIMATION, comboBoxAnim,
                IDC_ANIM_COMBOBOX);
            break;

        case SPI_SETLISTBOXSMOOTHSCROLLING:
            UpdateCheckBoxFromSys(SPI_GETLISTBOXSMOOTHSCROLLING, listBoxAnim,
                IDC_ANIM_LISTBOX);
            break;

#if WINVER >= WINVER_VISTA
        case SPI_SETCLIENTAREAANIMATION:
            UpdateCheckBoxFromSys(SPI_GETCLIENTAREAANIMATION, controlAnim,
                IDC_ANIM_CONTROL);
            break;
#endif
        }
#endif  /* WINVER >= WINVER_2K */

#undef GetNewSetting
#undef UpdateCheckBoxFromSys

        return TRUE;
    }

    return FALSE;
}

#undef UpdateCheckBox

#endif  /* !defined(MINIMAL) */
